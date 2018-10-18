#include "../DeviceThreads/ThreadDgPlc.h"
#include "./LineModel/DeviceConfig.h"
#include "./DataHelper/MemoryDBHelper.h"

using namespace std;

/**
 * @brief 构造函数
 * @param di
 */
ThreadDgPlc::ThreadDgPlc(DeviceInfo di): DeviceThread(di)
{      
    //大工PLC默认端口502
    if (di.Port.empty()) _di.Port = "502";
    _stopprocess = false;
    myDevice = new DagongPlc(atoi(_di.Port.data()),_di.Ip,_di.Name);
}

/**
 * @brief 析构函数
 */
ThreadDgPlc::~ThreadDgPlc()
{
    if(myDevice != NULL)
    {
        delete myDevice;
        myDevice = NULL;
    }
}

/**
 * @brief 处理采集点
 * @param 采集单元信息
 */
void ThreadDgPlc::ProcessUnit(UnitInfo ui)
{
    string pvs;
    //从PLC读取起始地址开始，指定长度的点位状态
    pvs = myDevice->Read(ui.StartAddress,ui.Length).data();
    //遍历采集点
    for(map<string,Tag>::reverse_iterator itert = ui.Tags.rbegin();itert!= ui.Tags.rend();++itert)
    {
        Tag tag = itert->second;
        if(tag.Name.empty())
            continue;
        string key = tag.TagName;
        tag.PlcValue = pvs.data()[tag.Address - ui.StartAddress];
        tag.MemortValue = m_db.Read_TagMValue(key);
//        if(tag.LogicalMode == "MCD" && tag.TagName == "3106$E0000017$JR1")
//            cout<<"tag.TagName = "<<tag.TagName<<" ---- tag.MemortValue = "<<tag.MemortValue<<endl;
        //处理控制逻辑为空的点位
        if(!tag.LogicalMode.compare("None"))
        {
             m_db.Write_TagMValue(tag.TagName,tag.PlcValue);
             //向关联控制点写1
             for (list<string>::reverse_iterator i = tag.CTagNames.rbegin();i != tag.CTagNames.rend();i++)
             {
                 string cTagName = *i;
                 m_db.Write_TagMValue(cTagName,tag.PlcValue);
             }
        }
        //处理控制逻辑为设备控制内存数据库的点位
//        else if(!tag.LogicalMode.compare("DCM"))
//        {
//            if(!tag.PlcValue.compare("1"))
//            {
//                //push plc value to memorydb
//                m_db.Write_TagMValue(tag.TagName,tag.PlcValue);
//                _log.LOG_DEBUG("ThreadDgPlc 【DCM模式】 检测到 【%s】 【%s】 地址【%d】 PLC值为1",_di.Name.data(),tag.Name.data(),tag.Address);
////                printf("DagongPlc Threads 【%s】 检测到 %s PLC值为 1 \n",\
////                       _di.Name.data(),tag.Name.data());
//                m_db.Write_TagMValue("采集点名",tag.Name);
//                myDevice->Write(tag.Address, "0");
//                //向关联控制点写1
//                for (list<string>::reverse_iterator i = tag.CTagNames.rbegin();i != tag.CTagNames.rend();i++)
//                {
//                    string cTagName = *i;
//                    m_db.Write_TagMValue(cTagName,"1");
//                }
//            }
//        }
        else if(!tag.LogicalMode.compare("DCM"))
        {
            if(!tag.PlcValue.compare("1"))
            {
                _log.LOG_DEBUG("ThreadFxPlc 【DCM模式】 检测到 【%s】 【%s】 地址【%d】 PLC值为1",_di.Name.data(),tag.Name.data(),tag.Address);
                m_db.Write_TagMValue(key,tag.PlcValue);
                //界面显示PLC状态
                m_db.Write_TagMValue("采集点名", tag.Name + "(M" + IntToString(tag.Address) + ")");
                //检测到PLC值为1 重置PLC值为0
                myDevice->Write(tag.Address, "0");

                _log.LOG_DEBUG("ThreadFxPlc 【DCM模式】 复位PLC点位【%s】 PLC地址【%d】",tag.Name.data(),tag.Address);

                //向关联控制点写1
                for (list<string>::reverse_iterator i = tag.CTagNames.rbegin();i != tag.CTagNames.rend();i++)
                {
                    string cTagName = *i;
                    m_db.Write_TagMValue(cTagName,tag.PlcValue);
                }

                //清空界面(display.cpp)PLC的状态
                if(tag.TagCode == "END")
                {
                    usleep(500*1000);
                    m_db.Write_TagMValue("采集点名", "");
                }
            }
        }
        else if(!tag.LogicalMode.compare("MCD"))
        {
            if (!tag.MemortValue.compare("1"))
            {
                //M101为PLC心跳检测点，每隔1秒置位(不打印)
                if(tag.Address != 101)
                {
                    _log.LOG_DEBUG("ThreadFxPlc 【MCD模式】 检测到 【%s】 【%s】 地址【%d】 内存数据库值为1",_di.Name.data(),tag.Name.data(),tag.Address);
                    //检测到实时库值为1 重置实时库值为0
                    m_db.Write_TagMValue(key,"0");
                    //界面显示PLC状态
                    m_db.Write_TagMValue("采集点名", tag.Name + "(M" + IntToString(tag.Address) + ")");
                }

                //向关联控制点写1
                for (list<string>::reverse_iterator i = tag.CTagNames.rbegin();i != tag.CTagNames.rend();i++)
                {
                    string cTagName = *i;
                    m_db.Write_TagMValue(cTagName,"1");
                }
                //向PLC地址写1
//                bool res = myDevice->WriteBits(tag.Address, "1", "M");
                bool res = myDevice->Write(tag.Address, "1");
                _log.LOG_DEBUG("ThreadFxPlc 【MCD模式】 向PLC【%s】 PLC地址【%d】 置位结果为【%d】",tag.Name.data(),tag.Address,res);
            }
        }

        //处理控制逻辑为内存数据库控制设备的点位
//        else if(!tag.LogicalMode.compare("MCD"))
//        {
//          if (!tag.MemortValue.compare("1"))
//            {
//                //检测到实时库值为1 重置实时库值为0
//              _log.LOG_DEBUG("ThreadDgPlc 【MCD模式】 检测到 【%s】 【%s】 地址【%d】 内存数据库值为1",_di.Name.data(),tag.Name.data(),tag.Address);
//                m_db.Write_TagMValue(tag.TagName,"0");
//                //向关联控制点写1
//                for (list<string>::reverse_iterator i = tag.CTagNames.rbegin();i != tag.CTagNames.rend();i++)
//                {
//                    string cTagName = *i;
//                    m_db.Write_TagMValue(cTagName,"1");
//                }
//                //向PLC地址写1
//                myDevice->Write(tag.Address, "1");
//            }
//        }
    }
}

/**
 * @brief 驱动线程处理函数
 */
void ThreadDgPlc::threadprocess()
{
    _counter = 0;
    _log.LOG_DEBUG("【%s】线程驱动已启动......",_di.Name.data());
    while (!_stopprocess)
    {
        for (map<string,UnitInfo>::reverse_iterator it = _di.Units.rbegin();it != _di.Units.rend();it++)
        {
            UnitInfo unit = it->second;
            //根据PLC指令最短间隔时间，设置此处延迟时间 30ms
            usleep(30 * 1000);
            ProcessUnit(unit);
        }
    }
    _log.LOG_DEBUG("【%s】线程驱动已停止......", _di.Name.data());
}

/**
 * @brief PLC设备驱动处理接口
 * @param arg
 * @return
 */
void* ThreadDgPlc::Start_Thread(void* arg)
{
    ThreadDgPlc *th = (ThreadDgPlc*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief 启动PLC设备驱动线程
 * @return
 */
bool ThreadDgPlc::Start()
{
    int ret = pthread_create(&pth,NULL,ThreadDgPlc::Start_Thread,this);
    if(ret != 0)
        return false;
    else
        return true;
}

/**
 * @brief 关闭PLC设备驱动线程
 * @return
 */
bool ThreadDgPlc::Stop()
{
    _stopprocess = true;
    pthread_detach(pth);
    pthread_cancel(pth);
    myDevice->Close();
    return true;
}

