#include "ThreadMicrometer.h"
#include "main.h"

/**
 * @brief ThreadMicrometer::ThreadMicrometer        集线器设备线程类驱动
 * @param di
 */
ThreadMicrometer::ThreadMicrometer(DeviceInfo di): DeviceThread(di)
{
    if(_di.type == "1")
        myTcpDevice = new MicrometerTcp(atoi(_di.Port.data()),_di.Ip,_di.Name);
    else if(_di.type == "2")
        myComDevice = new Micrometer(SerialPortParameters::GetSpp(_di),_di.Name);
    _stopprocess = false;
}

ThreadMicrometer::~ThreadMicrometer()
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

int ThreadMicrometer::getTestItemInfo()
{
    int num=0;
    for(int i=0; i<_di.testItemCode.size(); i++)
    {
        if(1 == atoi(_di.testItemCode.at(i).data()))    //高度检测设备的测试项条目为1（machine_test_item_info）
        {
            num = i;
            break;
        }
    }
    return num;
}

void ThreadMicrometer::threadprocess()
{
    int index = 0;
    string endFlag;
    _log.LOG_INFO("ThreadMicrometer 【%s】线程驱动已启动......",_di.Name.data());
    pi.init();

    vector<double> paraVec;
    paraVec = initDevice();

    //得到设备编号
    getDeviceNum();

    while (!_stopprocess)
    {
        usleep(100 * 1000);
        //设备不使能时,默认设备的判定结果为良品
        if(!getDeviceEnable())
            continue;
        //处理设备结束信号
        processEndFlag();
        //遍历采集点
        for(map<string, UnitInfo>::iterator kvpu = _di.Units.begin();kvpu != _di.Units.end();kvpu++)
        {
            for(map<string, Tag>::iterator kvpt = kvpu->second.Tags.begin();kvpt != kvpu->second.Tags.end();kvpt++)
            {
                Tag& tag = kvpt->second;
                string name = tag.TagCode;
                tag.MemortValue = m_db.Read_TagMValue(tag.TagName);

                if(name == "SC" && tag.MemortValue == "1")
                {

                    _log.LOG_DEBUG("ThreadMicrometer 【%s】 检测到高度检测信号!",_di.Name.data());
                    m_db.Write_TagMValue(tag.TagName, "0");
                    m_db.Write_TagMValue(_di.JudgeResult , "");
                    string partNoId = m_db.Read_TagMValue(_di.IdFlag);
                    string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                    _log.LOG_DEBUG("ThreadMicrometer 【%s】 id为【%s】 序列号为【%s】",_di.Name.data(),partNoId.data(),partSeqNo.data());

                    index = processMicrometerCheck(partSeqNo,partNoId,paraVec);

                    saveToSql(partNoId, partSeqNo,index);

                    //tcpServer
                    sendValueToTcpServer(pi.testItemEigenValue);
                }
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
    _log.LOG_INFO("ThreadMicrometer 【%s】线程驱动已停止......",_di.Name.data());
}


void *ThreadMicrometer::Start_Thread(void* arg)
{
    ThreadMicrometer* pth = (ThreadMicrometer*)arg;
    pth->threadprocess();
    return NULL;
}

/**
 * @brief ThreadMicrometer::Start
 * @return
 */
bool ThreadMicrometer::Start()
{
    pthread_attr_t pth_attr;
    pthread_attr_init(&pth_attr);
    pthread_attr_setdetachstate(&pth_attr,PTHREAD_CREATE_DETACHED);
    if(0 != pthread_create(&pth,&pth_attr,ThreadMicrometer::Start_Thread,this))
    {
        _log.LOG_ERROR("ThreadMicrometer 【%s】 高度检测设备采集线程创建失败",_di.Name.data());
        return false;
    }
    return true;
}

/**
 * @brief ThreadMicrometer::Stop
 * @return
 */
bool ThreadMicrometer::Stop()
{
    _stopprocess = true;
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

/**
 * @brief ThreadMicrometer::initDevice
 * @return
 */
vector<double> ThreadMicrometer::initDevice()
{
    vector<double> paraVec;
    int type=0;
    for(int i=0; i<_di.parameterNo.size(); i++)
    {
        type = atoi((const char*)_di.parameterNo.at(i).data());
        switch(type)
        {
            case Mircrometer1:
            {
                if(_di.parameter.size() > i)
                {
                    if(!_di.parameter.at(i).empty())
                    {
                       paraVec.push_back(StringToDouble(_di.parameter.at(i).data()));
                    }
                    else
                    {
                       _log.LOG_ERROR("ThreadMicrometer 【%s】 获取加工参数 Mircrometer1【失败】",_di.Name.data());
                       m_db.Write_TagMValue(_di.devParaAlarm, "1");
                    }
                }
            }
            break;
            case Mircrometer2:
            {
                if(_di.parameter.size() > i)
                {
                    if(!_di.parameter.at(i).empty())
                    {
                       paraVec.push_back(StringToDouble(_di.parameter.at(i).data()));
                    }
                    else
                    {
                        _log.LOG_ERROR("ThreadMicrometer 【%s】 获取加工参数Mircrometer2【失败】",_di.Name.data());
                       m_db.Write_TagMValue(_di.devParaAlarm, "1");
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    return paraVec;
}

/**
 * @brief ThreadMicrometer::getParameterNoInfo
 * @return
 */
string ThreadMicrometer::getParameterNoInfo()
{
    string data = "";
    for(int i=0; i<_di.parameterNo.size(); i++)
    {
        if(atoi((const char*)_di.parameterNo.at(i).data()) == devPara)
        {
            if(_di.parameter.size() > i)
                data = _di.parameter.at(i);
            else
				_log.LOG_ERROR("ThreadMicrometer getParameterNoInfo 【%s】 获取参数失败",_di.Name.data());
        }
    }
    return data;
}

/**
 * @brief ThreadMicrometer::processSpectrumAnalyzerCheck
 * @param partSeqNo
 * @param partNoId
 * @param paraVec
 */
int ThreadMicrometer::processMicrometerCheck(string partSeqNo, string partNoId, vector<double> &paraVec)
{
    int index = 0;
    string value;
    if(!partNoId.empty() && !partSeqNo.empty())
    {
        index = getTestItemInfo();
        vector<double> result;       //result里面包含2个高度检测设备的值
#ifndef _Debug
        if(_di.type == "1")
            result = myTcpDevice->GetValue();
        else if(_di.type == "2")
            result = myComDevice->GetValue();
#else
        if((atoi(partNoId.data()) % 2) == 0)
        {
            if((atoi(partNoId.data()) % 3) == 0)
            {
                result.push_back(2.615);
                result.push_back(3.386);
            }
            else
            {
                result.push_back(3.000);
                result.push_back(3.28);
            }
        }
        else
        {
            if((atoi(partNoId.data()) % 3) == 0)
            {
                result.push_back(2.614);
                result.push_back(3.686);
            }
            else
            {
                result.push_back(2.900);
                result.push_back(3.28);
            }
        }
#endif
        value = judgeMicrometerCheck(index,result,paraVec);
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadMicrometer 【%s】 获取序列号、id为【空】，不进行测试，直接判定为【不良品】",_di.Name.data());
        string svalue;
        svalue += partNoId.empty()?("产品ID为空，"):("");
        svalue += partSeqNo.empty()?("序列号为空，"):("");
        svalue += "直接判定为【不良品】";
        m_db.Write_TagMValue(_di.iValue,svalue);
    }

    pi.testItemCollectValue = value;
    pi.testItemEigenValue = value;

    if(pi.testItemEigenValue.empty())
        pi.testItemEigenValue = "NG";
    _log.LOG_DEBUG("ThreadMicrometer 【%s】特征值为【%s】",_di.Name.data(),pi.testItemEigenValue.data());
    return index;
}

/**
 * @brief ThreadMicrometer::CommunicateTest
 */
void ThreadMicrometer::CommunicateTest()
{
    if(_di.type == "1")
        _connectstatus = myTcpDevice->CanAcess() ? "1" : "0";
    else if(_di.type == "2")
        _connectstatus = myComDevice->CanAcess() ? "1" : "0";
    m_db.Write_TagMValue(_di.Alarm, _connectstatus);
    if(!_connectstatus.compare("0"))
    {
        _log.LOG_ERROR("ThreadMicrometer 【%s】 通信检测【失败】",_di.Name.data());
        m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
    }
    else
    {
        m_db.Write_TagMValue(_num + "$" + "NT","");
//                            _log.LOG_DEBUG("ThreadMicrometer 【%s】 通信检测【成功】",_di.Name.data());
    }
}

string ThreadMicrometer::judgeMicrometerCheck(int index,vector<double>& result,vector<double> &paraVec)
{
    string value;
    if(_di.testItemCode.size() > 0 && paraVec.size() == 2)
    {
        if(!result.empty() && result.size() == 2 )
        {
            int j=0;
            if((_di.minValue.size() > index) && (_di.maxValue.size() > index))
            {
                value = getMicrometerValue(j,index,result,paraVec);
                updateJudgeResult(j,index,result);
            }
            else
            {
                _log.LOG_DEBUG("ThreadMicrometer 【%s】 minValue或者maxValue 【ERROR】",_di.Name.data());
                m_db.Write_TagMValue(_di.iValue, "Micrometer minValue或者maxValue【ERROR】");
                pi.testItemJudgeResult = 0;
                m_db.Write_TagMValue(_di.JudgeResult , "0");
            }
        }
        else if(result.empty())
        {
            _log.LOG_DEBUG("ThreadMicrometer 【%s】 高度检测结果 【为空】",_di.Name.data());
            m_db.Write_TagMValue(_di.iValue, "高度检测结果【为空】");
            pi.testItemJudgeResult = 0;
            m_db.Write_TagMValue(_di.JudgeResult , "0");
//                                pi.testItemEigenValue = "NG";
        }

        //display.cpp
        m_db.Write_TagMValue(_di.iValue, "高度检测结果为：【" + value + "】");
        _log.LOG_DEBUG("ThreadMicrometer 【%s】  高度检测结果 result = 【%s】",_di.Name.data(),value.data());

        //设备使能为0时，不进行判定，结果置为1
        noJudge();
    }
    else
    {
        judgeParameterCheck(paraVec);
        pi.testItemJudgeResult = 0;
        m_db.Write_TagMValue(_di.JudgeResult , "0");
//                            pi.testItemEigenValue = "NG";
    }
    return value;
}

/**
 * @brief judgeParameterCheck
 * @param paraVec
 */
void ThreadMicrometer::judgeParameterCheck(vector<double> &paraVec)
{
    if(paraVec.size() != 2)
    {
        m_db.Write_TagMValue(_di.devParaAlarm, "1");
        _log.LOG_ERROR("ThreadMicrometer 【%s】 获取【加工参数】失败",_di.Name.data());
        m_db.Write_TagMValue(_di.iValue, "获取【加工参数】失败");
    }
    else
    {
        m_db.Write_TagMValue(_di.testParaAlarm, "1");
        _log.LOG_ERROR("ThreadMicrometer 【%s】 获取【测试项参数】失败",_di.Name.data());
        m_db.Write_TagMValue(_di.iValue, "获取【测试项参数】失败");
    }
}

/**
 * @brief ThreadMicrometer::updateJudgeResult
 * @param oktimes
 * @param index
 * @param result
 */
void ThreadMicrometer::updateJudgeResult(int oktimes,int index,vector<double>& result)
{
    if(oktimes == 2)          //代表有2个高度检测设备的值，且都在规定范围以内
    {
        _log.LOG_DEBUG("ThreadMicrometer 【%s】 高度检测设备的值 【正确】 最大值为【%f】 最小值为【%f】 采集值1为【%f】 采集值2为【%f】"\
           ,_di.Name.data(),_di.maxValue.at(index),_di.minValue.at(index),result.at(0),result.at(1));
        pi.testItemJudgeResult = 1;
        m_db.Write_TagMValue(_di.JudgeResult , "1");
//                                    pi.testItemEigenValue = value;
    }
    else
    {
        _log.LOG_ERROR("ThreadMicrometer 【%s】 高度检测设备的值 【错误】 最大值为【%f】 最小值为【%f】 采集值1为【%f】 采集值2为【%f】"\
           ,_di.Name.data(),_di.maxValue.at(index),_di.minValue.at(index),result.at(0),result.at(1));
        pi.testItemJudgeResult = 0;
        m_db.Write_TagMValue(_di.JudgeResult , "0");
//                                    pi.testItemEigenValue = "NG";
    }
}

string ThreadMicrometer::getMicrometerValue(int &oktimes,int index,vector<double>& result,vector<double> &paraVec)
{
    string value;
    if((fabs(_di.minValue.at(index)) <= EPSINON || fabs(_di.maxValue.at(index)) <= EPSINON) ||
    (_di.minValue.at(index) >= _di.maxValue.at(index)))
    {
         m_db.Write_TagMValue(_di.testParaAlarm, "1");
         _log.LOG_ERROR("ThreadMicrometer 【%s】 获取测试项参数【失败】 minValue=0,maxValue=0",_di.Name.data());
    }
    for(int i = 0;i < result.size();i++)
    {
        if((result.at(i) - paraVec.at(i) > _di.minValue.at(index)) && (result.at(i) - paraVec.at(i)  <_di.maxValue.at(index)))
        {
            oktimes++;
        }
        value.append(DoubleToString(result.at(i) - paraVec.at(i),"%.2f")).append("/");
    }

    if(value.rfind("/")!=string::npos)
    {
        value = value.substr(0, value.rfind("/"));
    }
    return value;
}
