#include "ThreadItechPower.h"


ThreadItechPower::ThreadItechPower()        //稳压电源
{}

/**
 * @brief 构造函数
 * @param di
 */
ThreadItechPower::ThreadItechPower(DeviceInfo di): DeviceThread(di)
{
    _stopprocess = false;
    if(_di.type == "1")
        myTcpDevice = new ItechPowerTcp(atoi(_di.Port.data()),_di.Ip,_di.Name);
    else if(_di.type == "2")
    {
        myComDevice = new ItechPower(SerialPortParameters::GetSpp(_di),_di.Name);
    }


}

/**
 * @brief 析构函数
 */
ThreadItechPower::~ThreadItechPower()
{
    if(_di.type == "1")
    {
        if(myTcpDevice != NULL)
        {
            delete myTcpDevice;
            myTcpDevice = NULL;
        }
    }
    else if(_di.type == "2")
    {
        if(myComDevice != NULL)
        {
            delete myComDevice;
            myComDevice = NULL;
        }
    }
}

/**
 * @brief 驱动线程处理函数
 */
void ThreadItechPower::threadprocess()
{
    _counter = 0;
    _log.LOG_DEBUG("ThreadItechPower 【%s】线程驱动已启动......",_di.Name.data());

    usleep(100 * 1000);


    if(_di.type == "1")
    {
        if(!myTcpDevice->Init())
        {
            _log.LOG_ERROR("ThreadItechPower 【%s】 InitDevice 初始化【失败】",_di.Name.data());
            m_db.Write_TagMValue(_di.iValue, "初始化【失败】");
            return;
        }
    }
    else if(_di.type == "2")
    {
        if(!myComDevice->Init())
        {
            _log.LOG_ERROR("ThreadItechPower 【%s】 InitDevice 初始化【失败】",_di.Name.data());
            m_db.Write_TagMValue(_di.iValue, "初始化【失败】");
            return;
        }
    }
    _log.LOG_DEBUG("ThreadItechPower 【%s】 InitDevice 初始化【成功】",_di.Name.data());


    while(!_stopprocess)
    {
        usleep(100 * 1000);
        //设备不使能时,默认设备的判定结果为良品
        if(!_di.Enable)
        {
            m_db.Write_TagMValue(_di.JudgeResult,"1");
            _log.LOG_ERROR("ThreadItechPower 【%s】 设备未使能!!!",_di.Name.data());
            continue;
        }
        //处理设备开始信号
        string endFlag = m_db.Read_TagMValue(_di.EndFlag);
        //开始信号为"1"时，将遗留信息清空
        if((!endFlag.empty()) && (!endFlag.compare("1")))
        {
            usleep(500 * 1000);
            _log.LOG_DEBUG("ThreadItechPower 【%s】检测到测试结束信号，复位启动信号 ",_di.Name.data());
            m_db.Write_TagMValue(_di.EndFlag,"0");
        }
        // 遍历采集点
        for(map<string, UnitInfo>::reverse_iterator it = _di.Units.rbegin();it != _di.Units.rend();it++)
        {
            map<string, Tag> tags = it->second.Tags;
            for(map<string, Tag>::reverse_iterator im = tags.rbegin();im != tags.rend();im++)
            {
                Tag tag = im->second;
                string name = tag.TagCode;
                tag.MemortValue = m_db.Read_TagMValue(tag.TagName);
                //处理电源设定信号
                if (!name.compare("SSP"))
                {
                    if (!tag.MemortValue.compare("1"))
                    {
                        _log.LOG_DEBUG("ThreadItechPower 【%s】 检测到处理电源设定信号",_di.Name.data());
                        m_db.Write_TagMValue(tag.TagName, "0");
                        if(_di.type == "1")
                            myTcpDevice->SetPower();
                        else if(_di.type == "2")
                            myComDevice->SetPower();
                    }
                }
                //处理通信状态点
                else if(!name.compare("CS"))
                {
                    if (_counter % 20 == 0)
                    {
                        if(_di.type == "1")
                            _connectstatus = myTcpDevice->CanAcess() ? "1" : "0";
                        else if(_di.type == "2")
                            _connectstatus = myComDevice->CanAcess() ? "1" : "0";
                        m_db.Write_TagMValue(_di.Alarm, _connectstatus);
                        if(_connectstatus == "0")
                        {
                            _log.LOG_ERROR("ThreadItechPower 【%s】 通信检测【失败】",_di.Name.data());
                            m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
                        }
                        else
                        {
                            m_db.Write_TagMValue(_num + "$" + "NT","");
//                            _log.LOG_DEBUG("ThreadItechPower 【%s】 通信检测【成功】",_di.Name.data());
                        }
                        _counter = 1;
                    }
                }
            }
        }
    _counter++;
    }
    _log.LOG_DEBUG("ThreadItechPower 【%s】线程驱动已停止......",_di.Name.data());
}

/**
 * @brief 驱动线程处理接口
 * @param arg
 * @return
 */
void *ThreadItechPower::Start_Thread(void* arg)
{
    ThreadItechPower *th = (ThreadItechPower*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief 启动设备驱动线程
 * @return
 */
bool ThreadItechPower::Start()
{
    int ret = pthread_create(&pth,NULL,ThreadItechPower::Start_Thread,this);
    if(ret != 0)
    {
        return false;
    }
    return true;
}

/**
 * @brief 设备驱动线程关闭
 * @return
 */
bool ThreadItechPower::Stop()
{
    _stopprocess = true;
    pthread_detach(pth);
    pthread_cancel(pth);

    if(_di.type == "1")
    {
        myTcpDevice->Close();
        myTcpDevice->CloseReconThread();
    }
    else if(_di.type == "2")
    {
        myComDevice->Close();
        myComDevice->CloseReconThread();
    }

    return true;
}

