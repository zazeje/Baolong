#include "ThreadLaserMarker.h"
#include "main.h"


/**
 * @brief 构造函数
 * @param di
 */
ThreadLaserMarker::ThreadLaserMarker(DeviceInfo di): DeviceThread(di)
{    
    if (di.Port.empty()) _di.Port = "9004";
    myDevice = new LaserMarker(atoi(_di.Port.data()),_di.Ip,_di.Name);
    _stopprocess = false;
    _counter = 0;
}

/**
 * @brief 析构函数
 */
ThreadLaserMarker::~ThreadLaserMarker()
{
    if(myDevice != NULL)
    {
        delete myDevice;
        myDevice = NULL;
    }
}

/**
 * @brief 驱动线程处理函数
 */
void ThreadLaserMarker::threadprocess()
{
    _log.LOG_DEBUG("ThreadLaserMarker 【%s】线程驱动已启动......",_di.Name.data());

    //得到设备编号
    getDeviceNum();                     //1#打标机
    //获取PLC打标状态点
    getPlcStatePoint();

    while (!_stopprocess)
    {
       usleep(100 * 1000);
       //设备不使能时,默认设备的判定结果为良品
       if(!getDeviceEnable())
           continue;
       //处理信号结束点
       processEndFlag();

       // 遍历采集点
       for(map<string, UnitInfo>::iterator it = _di.Units.begin();it != _di.Units.end();it++)
       {
           map<string, Tag> tags = it->second.Tags;
           for(map<string, Tag>::iterator im = tags.begin();im != tags.end();im++)
           {
               Tag tag = im->second;
               string name = tag.TagCode;
               tag.MemortValue = m_db.Read_TagMValue(tag.TagName);
               //处理打标信号
               if (!name.compare("SM"))
               {
                   if (!tag.MemortValue.compare("1"))
                   {                       
                       m_db.Write_TagMValue(tag.TagName, "0");
                       string id = m_db.Read_TagMValue(_di.IdFlag);
                       m_db.Write_TagMValue(_di.BarCode, "收到打标信号,打标ID为：【" + id + "】");
                       _log.LOG_DEBUG("ThreadLaserMarker 【%s】收到打标信号,打标ID为【%s】",_di.Name.data(),id.data());
                       //处理打标信号
                       processLM(id);
                       //写序列号和ID到关联控制点
                       writeSeqAndID(tag, id);
                   }
               }
               //处理通信状态
               else if(!name.compare("CS"))
               {
                   if (_counter % 20 == 0)
                   {
                       CommunicateTest();
                       _counter = 1;
                   }
               }
           }
       }
       _counter++;
    }
    _log.LOG_INFO("ThreadLaserMarker 【%s】线程驱动已停止......",_di.Name.data());
}

/**
 * @brief 驱动线程处理接口
 * @param arg
 * @return
 */
void *ThreadLaserMarker::Start_Thread(void* arg)
{
    ThreadLaserMarker *th = (ThreadLaserMarker*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief 启动驱动线程
 * @return
 */
bool ThreadLaserMarker::Start()
{
    int ret = pthread_create(&pth,NULL,ThreadLaserMarker::Start_Thread,this);    
    if(ret != 0)
        return false;
    else
        return true;
}

/**
 * @brief 关闭驱动线程
 * @return
 */
bool ThreadLaserMarker::Stop()
{
    _stopprocess = true;
    pthread_detach(pth);
    pthread_cancel(pth);
    myDevice->Close();
    myDevice->CloseReconThread();
    return true;
}

/**
 * @brief ThreadLaserMarker::getPlcStatePoint               获取PLC打标状态点
 */
void ThreadLaserMarker::getPlcStatePoint()
{
    for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++)
    {
        DeviceInfo di = it->second;
        if(di.Name == "1#PLC")
        {
            m_plcAlarm = di.communicateERR;           //PLC报警点位(MCD模式)
            for(map<string, UnitInfo>::iterator it = di.Units.begin();it != di.Units.end();it++)
            {
                map<string, Tag> tags = it->second.Tags;
                for(map<string, Tag>::iterator im = tags.begin();im != tags.end();im++)
                {
                    Tag tag = im->second;
                    if(tag.TagCode == "LR")
                        m_plcLR = tag.TagName;
                }
            }
        }
    }
    cout<<"----- m_plcAlarm = "<<m_plcAlarm<<" --- m_plcLR = "<<m_plcLR<<endl;
}

/**
 * @brief ThreadLaserMarker::processLM                      处理打标信号
 * @param id
 */
void ThreadLaserMarker::processLM(string id)
{
    //如果打标ID非空，则开始打标
    if (!id.empty())
    {
        string result = myDevice->DoPrint(id);
#ifdef _Debug
            if((atoi(id.data()) % 2) == 0)
            {
                if((atoi(id.data()) % 3) == 0)
                {
                    result = "1233313";
                }
                else
                {
                    result = id;
                }
            }
            else
            {
                if((atoi(id.data()) % 3) == 0)
                {
                    result = "1515551";
                }
                else
                {
                    result = id;
                }
            }
#endif
        _log.LOG_DEBUG("ThreadLaserMarker 【%s】 打标返回值为：【%s】",_di.Name.data(),result.data());
        if(!result.compare(id))
        {
            m_db.Write_TagMValue(m_plcLR, "1");
            _log.LOG_DEBUG("ThreadLaserMarker 【%s】 打标机检测【成功】，ID为【%s】",_di.Name.data(),result.data());
            m_db.Write_TagMValue(_di.BarCode, "打标机检测【成功】，ID为【" + result + "】");
        }
        else
        {
            m_db.Write_TagMValue(m_plcAlarm, "1");
            _log.LOG_ERROR("ThreadLaserMarker 【%s】 打标机检测【失败】，发送ID为【%s】,返回ID为【%s】",_di.Name.data() ,id.data(), result.data());
            m_db.Write_TagMValue(_di.BarCode, "打标失败，发送ID【" + id + "】,返回ID【" + result + "】");
        }
    }
}

/**
 * @brief ThreadLaserMarker::writeSeqAndID                  写序列号和ID到关联控制点
 * @param tag
 */
void ThreadLaserMarker::writeSeqAndID(Tag tag, string id)
{
    if(!tag.CTagNames.empty())
    {
        for(list<string>::iterator it = tag.CTagNames.begin();it != tag.CTagNames.end();it++)
        {
            string key = *it;
            if(key.find("PID") != string::npos)
                m_db.Write_TagMValue(key,id);
            else if(key.find("SN") != string::npos)
                m_db.Write_TagMValue(key,m_db.Read_TagMValue(_di.SnFlag));
        }
    }
}


/**
 * @brief ThreadLaserMarker::CommunicateTest
 */
void ThreadLaserMarker::CommunicateTest()
{
    _connectstatus = myDevice->CanAcess() ? "1" : "0";
    m_db.Write_TagMValue(_di.Alarm, _connectstatus);
    if(_connectstatus == "0")
    {
        _log.LOG_ERROR("ThreadLaserMarker 【%s】 通信检测【失败】",_di.Name.data());
        m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
    }
    else
        m_db.Write_TagMValue(_num + "$" + "NT","");
}










