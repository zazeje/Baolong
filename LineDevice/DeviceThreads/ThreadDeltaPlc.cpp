#include "ThreadDeltaPlc.h"

/**
 * @brief ThreadDeltaPlc::ThreadDeltaPlc                用设备信息构造线程类
 * @param di                                            设备信息
 * @param productModel                                  产品参数
 */
ThreadDeltaPlc::ThreadDeltaPlc(DeviceInfo di) : DeviceThread(di)
{
    _stopprocess = false;
    myDevice  = new DeltaPlc(SerialPortParameters::GetSpp(_di), _di.Name);
}

ThreadDeltaPlc::~ThreadDeltaPlc()
{
    if(myDevice != NULL)
    {
        delete myDevice;
        myDevice = NULL;
    }
}
/**
 * @brief ThreadDeltaPlc::Start                         启动PLC逻辑处理线程
 * @return
 */
bool ThreadDeltaPlc::Start()
{
    int ret = pthread_create(&pth,NULL,ThreadDeltaPlc::Start_Thread,this);
    if(ret != 0)
        return false;
    else
        return true;
}

/**
 * @brief ThreadDeltaPlc::Stop                          关闭PLC逻辑处理线程
 * @return
 */
bool ThreadDeltaPlc::Stop()
{
    _stopprocess = true;
    pthread_detach(pth);
    pthread_cancel(pth);
    myDevice->Close();
    return true;
}

/**
 * @brief ThreadDeltaPlc::Start_Thread                  处理线程接口函数
 * @param arg
 * @return
 */
void* ThreadDeltaPlc::Start_Thread(void* arg)
{
    ThreadDeltaPlc *th = (ThreadDeltaPlc*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief ThreadDeltaPlc::threadprocess                 逻辑处理函数
 */
void ThreadDeltaPlc::threadprocess()
{
    _counter = 0;
    m_log.WriteToLogFile("LineAuto ThreadDeltaPlc ProcessUnit ","检测到【" + _di.Name + "】线程驱动已启动......");
    printf("【%s】线程驱动已启动......\n",_di.Name.data());
    while (!_stopprocess)
    {
        for (map<string,UnitInfo>::reverse_iterator it = _di.Units.rbegin();it != _di.Units.rend();it++)
        {
            UnitInfo unit = it->second;
            //根据PLC指令最短间隔时间，设置此处延迟时间
            usleep(35 * 1000);
            //处理采集单元
            ProcessUnit(unit);
        }
        _counter++;
    }
    m_log.WriteToLogFile("LineAuto ThreadDeltaPlc ProcessUnit ","检测到【" + _di.Name + "】线程驱动已停止......");
    printf("【%s】线程驱动已停止......\n", _di.Name.data());
}

/**
 * @brief ThreadDeltaPlc::ProcessUnit                   处理采集单元信息
 * @param ui
 */
void ThreadDeltaPlc::ProcessUnit(UnitInfo ui)
{
    string pvs;
    //读PLC点位
    pvs = myDevice->ReadRegister(ui.StartAddress, ui.Length, ui.Name[0]);
    if(pvs.empty() || pvs.length() != ui.Length)
        return ;
    //遍历采集单元
    for(map<string,Tag>::iterator is = ui.Tags.begin();is != ui.Tags.end();is++)
    {
        Tag tag;
        tag = is->second;
        if(tag.Name.empty())
            continue;
        string key = tag.TagName;
        tag.PlcValue = pvs.data()[tag.Address - ui.StartAddress];
        tag.MemortValue = m_db.Read_TagMValue(key);
        //处理逻辑控制模式为空的点位
        if(!tag.LogicalMode.compare("None"))
        {
            m_db.Write_TagMValue(key,tag.PlcValue);
        }
        //处理逻辑控制模式为设备控制内存数据库的点位
        else if(!tag.LogicalMode.compare("DCM"))
        {
            //测试修改PLCValue -> MemortValue
            if(!tag.PlcValue.compare("1"))
            {
                m_log.WriteToLogFile("LineAuto ThreadDeltaPlc ProcessUnit ","检测到【" + _di.Name + "】 [" + tag.TagCode + "] PLC值为1 ");
                printf("\n\n检测到 【%s】  [%s] PLC值为1 \n\n",_di.Name.data(),tag.TagCode.data());
                //push plc value to memorydb
                m_db.Write_TagMValue(key,tag.PlcValue);
                //检测到PLC值为1 重置PLC值为0
                myDevice->WriteRegister(tag.Address, "0", ui.Name[0]);
                //m_log.WriteToLogFile("LineAuto ThreadDeltaPlc ProcessUnit ","WriteRegister tag.Address = " + IntToString(tag.Address) + "  0");
                if(tag.TagCode.compare("SB"))
                    m_db.Write_TagMValue("采集点名",tag.Name);
                else
                    m_db.Write_TagMValue("采集点名","");
                //向关联控制点写1
                for (list<string>::reverse_iterator i = tag.CTagNames.rbegin();i != tag.CTagNames.rend();i++)
                {
                    printf("=== 向关联控制点写1 \n");
                    string cTagName = *i;
                    printf("cTagName == %s\n",cTagName.data());
                    m_db.Write_TagMValue(cTagName,tag.PlcValue);
                }
            }
        }
        //处理逻辑控制模式为内存数据库控制设备的点位
        else if(!tag.LogicalMode.compare("MCD"))
        {
            if (!tag.MemortValue.compare("1"))
            {
                m_log.WriteToLogFile("LineAuto ThreadDeltaPlc ProcessUnit ","检测到【" + _di.Name + "】 [" + tag.TagCode + "]实时库值为1  ");
                //检测到实时库值为1 重置实时库值为0
                m_db.Write_TagMValue(key,"0");
                //向关联控制点写1
                for (list<string>::reverse_iterator i = tag.CTagNames.rbegin();i != tag.CTagNames.rend();i++)
                {
                    string cTagName = *i;
                    m_db.Write_TagMValue(cTagName,"1");
                }
                //向PLC地址写1
                myDevice->WriteRegister(tag.Address, "1", ui.Name[0]);
            }
        }
        //处理通信状态点位
        if(!tag.TagCode.compare("CS"))
        {
            if(_counter % 20 == 0)
            {
                _counter = 1;
                _connectstatus = myDevice->CanAcess() ? "1" : "0";
                m_db.Write_TagMValue(_di.Alarm,_connectstatus);
                if(_connectstatus == "0")
                {
                    m_log.WriteToLogFile("LineAuto ThreadDeltaPlc ProcessUnit ","台达PLC通信检测失败，启动断线重连...");
                    m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
                }
                else
                    m_db.Write_TagMValue(_num + "$" + "NT","");
            }
        }
    }
}
