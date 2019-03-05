#include "ThreadXse6.h"
#include "main.h"


ThreadXse6::ThreadXse6()        //气压表
{}

/**
 * @brief 构造函数
 * @param di
 */
ThreadXse6::ThreadXse6(DeviceInfo di) : DeviceThread(di)
{
    _di = di;
    endFlag = "";
    myTcpDevice = new Xse6Tcp(atoi(_di.Port.data()), _di.Ip, _di.Name);
    _stopprocess = false;
    _stopColl = true;
}

/**
 * @brief 析构函数
 */
ThreadXse6::~ThreadXse6()
{
    if(myTcpDevice != NULL)
    {
        delete myTcpDevice;
        myTcpDevice = NULL;
    }
}


/**
 * @brief 设备线程启动
 * @return
 */
bool ThreadXse6::Start()
{
    int ret = pthread_create(&pth,NULL,ThreadXse6::StartThread,this);
    if(ret != 0)
    {
        _log.LOG_ERROR("ThreadXse6 【%s】 数显表采集线程创建失败！",_di.Name.data());
        return false;
    }
    return true;
}

/**
 * @brief 线程启动接口
 * @param arg
 * @return
 */
void *ThreadXse6::StartThread(void* arg)
{
    ThreadXse6* th = (ThreadXse6*)arg;
    th->ProcessStartThread();
    return NULL;
}

void ThreadXse6::processEndFlag()
{
    endFlag = m_db.Read_TagMValue(_di.EndFlag);
    //检测到测试结束信号，清空上一个产品信息
    if(!endFlag.empty() && !endFlag.compare("1"))
    {
        usleep(500 * 1000);
        _log.LOG_DEBUG("ThreadXse6 【%s】 检测到结束采集信号，清空瞬时值，特征值，复位结束采集信号点",_di.Name.data());
        _iValue.clear();
        _cValue.clear();
        pi.Clear();
        m_db.Write_TagMValue(_di.EndFlag, "");
        m_db.Write_TagMValue(_di.JudgeResult, "");
        m_db.Write_TagMValue(_di.MAV, "");
        m_db.Write_TagMValue(_di.IdFlag, "");
        m_db.Write_TagMValue(_di.SnFlag, "");
        m_db.Write_TagMValue(_di.iValue, "");
        m_db.Write_TagMValue(_di.EigenValue, "");
        m_db.Write_TagMValue(_di.DeviceState, "测试结束");
    }
}

/**
 * @brief ThreadXse6::CommunicateTest
 */
void ThreadXse6::CommunicateTest()
{
    _connectstatus = myTcpDevice->CanAcess() ? "1" : "0";
    m_db.Write_TagMValue(_di.Alarm, _connectstatus);
    if(_connectstatus == "0")
    {
        _log.LOG_ERROR("ThreadXse6 【%s】通信检测【失败】 ",_di.Name.data());
        m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
    }
    else
    {
        m_db.Write_TagMValue(_num + "$" + "NT","");
//                                _log.LOG_ERROR("ThreadXse6 【%s】通信检测【正常】 ",_di.Name.data());
    }
}

void ThreadXse6::clearData(string tagName)
{
    m_db.Write_TagMValue(tagName,"0");
    _iValue.clear();
    _cValue.clear();
    pi.Clear();
    m_db.Write_TagMValue(_di.JudgeResult, "");
    m_db.Write_TagMValue(_di.MAV, "");
    m_db.Write_TagMValue(_di.iValue, "");
    m_db.Write_TagMValue(_di.EigenValue, "");
    _log.LOG_DEBUG("ThreadXse6 【%s】 检测到开始采集信号! ",_di.Name.data());
}

/**
 * @brief ThreadXse6::judgeXse6Check
 * @param testItemJudgeResult
 */
void ThreadXse6::judgeXse6Check(int testItemJudgeResult)
{
    if(testItemJudgeResult == 1)
    {
        _log.LOG_DEBUG("ThreadXse6 【%s】 判定为【良品】 .... ", _di.Name.data());
        m_db.Write_TagMValue(_di.JudgeResult, "1");
    }
    else
    {
        _log.LOG_ERROR("ThreadXse6 【%s】 判定为【不良品】 .... ", _di.Name.data());
        m_db.Write_TagMValue(_di.JudgeResult, "0");
    }
}

/**
 * @brief ThreadXse6::processXse6Check
 * @param partSeqNo
 * @param partNoId
 * @param tag
 */
void ThreadXse6::processXse6Check(string partSeqNo, string partNoId, Tag& tag)
{
    if(!partNoId.empty() && !partSeqNo.empty())
    {
        //气压表只有一条测试项数据，故index = 0
        ProcessCValue(pi, 0, pressJudge, _cValue);

        //向气压的关联控制点 写入平均值
        if(!tag.CTagNames.empty())
        {
            for(list<string>::iterator it = tag.CTagNames.begin();it != tag.CTagNames.end();it++)
            {
                string key = *it;
                m_db.Write_TagMValue(key, pi.testItemEigenValue);
            }
        }

        judgeXse6Check(pi.testItemJudgeResult);
        //在显示界面上实时更新特征值
        m_db.Write_TagMValue(_di.iValue, "气压表采集值为：【" + pi.testItemEigenValue + "】");

        //设备判定使能为0时，直接将判定结果置为1
        noJudge();
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadXse6 【%s】 获取序列号、id为【空】，不进行测试，直接判定为【不良品】",_di.Name.data());
        string svalue;
        svalue += partNoId.empty()?("产品ID为空，"):("");
        svalue += partSeqNo.empty()?("序列号为空，"):("");
        svalue += "直接判定为【不良品】";
        m_db.Write_TagMValue(_di.iValue,svalue);
    }
}

/**
 * @brief 线程处理函数
 */
void ThreadXse6::ProcessStartThread()
{
    _log.LOG_INFO("ThreadXse6 【%s】 驱动线程已启动......",_di.Name.data());
    int ret = pthread_create(&_collectThread,NULL,ThreadXse6::CollectThread,this);
    if(ret != 0)
    {
        _log.LOG_ERROR("ThreadXse6 【%s】 数显表采集线程创建失败！",_di.Name.data());
        return ;
    }

    pi.init();

    //烧程良品点
    m_PointOk = D3GetPLCPointOK();
    //烧程不良品点
    m_PointNG = D3GetPLCPointNG();
    getDeviceNum();

    while(!_stopprocess)
    {
        usleep(100 * 1000);
        //设备不使能时,默认设备的判定结果为良品
        if(!getDeviceEnable())
            continue;
        processEndFlag();

        //遍历采集点
        for(map<string,UnitInfo>::iterator it = _di.Units.begin();it != _di.Units.end();it++)
        {
            for(map<string,Tag>::iterator im = it->second.Tags.begin();im != it->second.Tags.end();im++)
            {
                Tag& tag = im->second;
                string name = tag.TagCode;
                tag.MemortValue = m_db.Read_TagMValue(tag.TagName);
                //检测到开始采集信号
                if(!name.compare("SC"))
                {
                    if(!tag.MemortValue.compare("1"))
                    {
                        clearData(tag.TagName);
                        _stopColl = false;
                        sleep(5);
                        _stopColl = true;
                        _log.LOG_DEBUG("ThreadXse6 【%s】 检测到停止采集!",_di.Name.data());
                        pi.Clear();

                        string partNoId = m_db.Read_TagMValue(_di.IdFlag);
                        string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                        _log.LOG_DEBUG("ThreadXse6 【%s】设备 产品ID为【%s】 序列号为【%s】",_di.Name.data(),partNoId.data(),partSeqNo.data());

                        _log.LOG_DEBUG("ThreadXse6 【%s】强制给OK信号，判定良品. 序列号为【%s】",_di.Name.data(),partSeqNo.data());
                        m_db.Write_TagMValue(_di.IdFlag,"强制给OK信号，直接判定为【良品】");
                        m_db.Write_TagMValue(m_PointOk,"1");

                        //processXse6Check(partSeqNo,partNoId,tag);

                        //将产品信息存储到数据库
                        saveToSql(partNoId, partSeqNo,0);

                        //将视觉检测结果上传至上位机
                        sendValueToTcpServer(pi.testItemEigenValue);
                    }
                }
                //处理通信状态
                else if(!name.compare("CS"))
                {
                    if (!_stopColl)
                    {
                        _connectstatus = "1";
                        _counter = 0;
                    }
                    else
                    {
                        if (_counter % 30 == 0)
                        {
                            CommunicateTest();
                            _counter = 1;
                        }
                    }
//                   m_db.Write_TagMValue(tag.TagName, _connectstatus);
                }
            }
        }
        _counter++;
    }
    _stopColl = true;
}

/**
 * @brief 采集线程启动接口
 * @param arg
 * @return
 */
void *ThreadXse6::CollectThread(void *arg)
{
    ThreadXse6* th = (ThreadXse6*)arg;
    th->CollectData();
    return NULL;
}

/**
 * @brief 停止设备驱动线程
 * @return
 */
bool ThreadXse6::Stop()
{
    _stopprocess = true;
    _stopColl = true;
    pthread_detach(pth);
    pthread_cancel(pth);
    myTcpDevice->Close();
    myTcpDevice->CloseReconThread();
    return true;
}

/**
 * @brief 采集线程处理
 */
void ThreadXse6::CollectData()
{
    while(!_stopprocess)
    {
        while(!_stopColl)
        {
            double press = myTcpDevice->GetValue();
#ifdef _Debug
            string partNoId = m_db.Read_TagMValue(_di.IdFlag);
            if((atoi(partNoId.data()) % 2) == 0)
            {
                if((atoi(partNoId.data()) % 3) == 0)
                {
                    press = 10.405884;
                }
                else
                {
                    press = 35.803921;
                }
            }
            else
            {
                if((atoi(partNoId.data()) % 3) == 0)
                {
                    press = 39.405884;
                }
                else
                {
                    press = 39.007843;
                }
            }
#endif
            _iValue = DoubleToString(press, "%0.2f");
            //在显示界面上实时更新瞬时值
            m_db.Write_TagMValue(_di.iValue, "气压表采集值为：【" + _iValue + "】");
            //tcpServer
            sendValueToTcpServer(_iValue);
            _cValue.append(_iValue + "/");
            usleep(100*1000);
        }
        usleep(100*1000);
    }
}

/**
 * @brief 获取气压平均值
 * @return
 */
double ThreadXse6::GetAverageValue()
{
    vector<double> valueArray;
    split(_cValue,"/",valueArray);
    return Average(valueArray);
}

string ThreadXse6::D3GetPLCPointOK()                                //D3线获取PLC“扫码不良”点位
{
    string plcScanOk;
    for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++)
    {
        DeviceInfo di = it->second;
        if(di.Name == "1#PLC")
        {
            for(map<string, UnitInfo>::iterator it = di.Units.begin();it != di.Units.end();it++)
            {
                map<string, Tag> tags = it->second.Tags;
                for(map<string, Tag>::iterator im = tags.begin();im != tags.end();im++)
                {
                    Tag tag = im->second;
                    if(tag.TagCode == "JGOK")
                        plcScanOk = (tag.TagName);
                }
            }
        }
    }
    return plcScanOk;
}

string ThreadXse6::D3GetPLCPointNG()                                //D3线获取PLC“扫码良”点位
{
    string plcScanNG;
    for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++)
    {
        DeviceInfo di = it->second;
        if(di.Name == "1#PLC")
        {
            for(map<string, UnitInfo>::iterator it = di.Units.begin();it != di.Units.end();it++)
            {
                map<string, Tag> tags = it->second.Tags;
                for(map<string, Tag>::iterator im = tags.begin();im != tags.end();im++)
                {
                    Tag tag = im->second;
                    if(tag.TagCode == "JGNG")
                        plcScanNG = (tag.TagName);
                }
            }
        }
    }
    return plcScanNG;
}
