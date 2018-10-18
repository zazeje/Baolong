#include "ThreadFx5U.h"

ThreadFx5U::ThreadFx5U(DeviceInfo di): DeviceThread(di)
{
    _stopprocess = false;
    myDevice  = new Fx5U(SerialPortParameters::GetSpp(_di),_di.Name);
}

ThreadFx5U::~ThreadFx5U()
{
    if(myDevice != NULL)
    {
        delete myDevice;
        myDevice = NULL;
    }
}

void ThreadFx5U::ProcessUnit(UnitInfo ui)
{
    string pvs;
    //读PLC点位
    pvs = myDevice->ReadBits(ui.StartAddress, ui.Length, "M");

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
            //向关联控制点写1
            for (list<string>::reverse_iterator i = tag.CTagNames.rbegin();i != tag.CTagNames.rend();i++)
            {
                string cTagName = *i;
                m_db.Write_TagMValue(cTagName,tag.PlcValue);
            }
            //PLC报警
            if(tag.Address == 115 && tag.PlcValue == "1")
            {
                //20170813 PLC报警不显示在界面，直接向上位机上传
                m_db.Write_TagMValue("PLcAlarm","0");
                _log.LOG_ERROR("ThreadFx5U 【%s】 PLC报警 ...",_di.Name.data());
            }
            else
            {
                m_db.Write_TagMValue("PLcAlarm","1");
            }
        }
        //处理逻辑控制模式为设备控制内存数据库的点位
        else if(!tag.LogicalMode.compare("DCM"))
        {
            if(!tag.PlcValue.compare("1"))
            {
                _log.LOG_DEBUG("ThreadFx5U 检测到 【%s】 【%s】 PLC值为1",_di.Name.data(),tag.Name.data());
                printf("检测到 【%s】  【%s】 PLC值为1\n",_di.Name.data(),tag.Name.data());

                //push plc value to memorydb
                m_db.Write_TagMValue(key, tag.PlcValue);
                m_db.Write_TagMValue("采集点名", tag.Name);

                //检测到PLC值为1 重置PLC值为0
                myDevice->WriteBits(tag.Address, "0", "M");
                _log.LOG_DEBUG("ThreadFx5U 复位PLC点位 地址为【%d】",tag.Address);

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
        //处理逻辑控制模式为内存数据库控制设备的点位
        else if(!tag.LogicalMode.compare("MCD"))
        {
            if (!tag.MemortValue.compare("1"))
            {
                _log.LOG_DEBUG("ThreadFx5U MCD模式 检测到【%s】 【%s】 实时库值为1",_di.Name.data(),tag.Name.data());

                //检测到实时库值为1 重置实时库值为0
                m_db.Write_TagMValue(key,"0");
                //向关联控制点写1
                for (list<string>::reverse_iterator i = tag.CTagNames.rbegin();i != tag.CTagNames.rend();i++)
                {
                    string cTagName = *i;
                    m_db.Write_TagMValue(cTagName,"1");
                }
                //向PLC地址写1
                myDevice->WriteBits(tag.Address, "1", "M");
            }
        }
        //处理通信状态点位
        if(!tag.TagCode.compare("CS"))
        {
            if(_counter % 20 == 0)
            {
                _counter = 1;
                _connectstatus = myDevice->CanAcess() ? "1" : "0";
                m_db.Write_TagMValue(_di.Alarm, _connectstatus);
                if(_connectstatus == "0")
                {
                    _log.LOG_ERROR("ThreadFx5U 【%s】 通信检测【失败】",_di.Name.data());
                    m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
                }
                else
                    m_db.Write_TagMValue(_num + "$" + "NT","");
            }
        }
    }
}

void ThreadFx5U::threadprocess()
{
    _counter = 0;
    int num = 0;
    _log.LOG_DEBUG("ThreadFx5U 【%s】线程驱动已启动......",_di.Name.data());
    printf("【%s】线程驱动已启动......\n",_di.Name.data());
    while (!_stopprocess)
    {
        for (map<string,UnitInfo>::reverse_iterator it = _di.Units.rbegin();it != _di.Units.rend();it++)
        {
            UnitInfo unit = it->second;
            //根据PLC指令最短间隔时间，设置此处延迟时间
            usleep(35 * 1000);
            ProcessUnit(unit);
        }
        _counter++;
    }
    _log.LOG_DEBUG("ThreadFx5U 【%s】线程驱动已停止......",_di.Name.data());
    printf("【%s】线程驱动已停止......\n", _di.Name.data());
}


void * ThreadFx5U::Start_Thread(void *arg)
{
    ThreadFx5U *th = (ThreadFx5U*)arg;
    th->threadprocess();
    return NULL;
}


bool ThreadFx5U::Start()
{
    int ret = pthread_create(&pth,NULL,ThreadFx5U::Start_Thread,this);
    if(ret != 0)
        return false;
    else
        return true;
}

bool ThreadFx5U::Stop()
{
    _stopprocess = true;
    pthread_detach(pth);
    pthread_cancel(pth);
    myDevice->Close();
    return true;
}
