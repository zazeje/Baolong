#include "ThreadSpectrumAnalyzer.h"



/**
 * @brief 无参构造
 */
ThreadSpectrumAnalyzer::ThreadSpectrumAnalyzer() : DeviceThread()       //频谱仪
{

}

/**
 * @brief 含参构造
 * @param di 包含设备信息的对象
 */
ThreadSpectrumAnalyzer::ThreadSpectrumAnalyzer(DeviceInfo di) : DeviceThread(di)
{
    _di = di;
    _minStandar = 0;
    _maxStandar = 0;
    _counter = 0;
    mydevice = SpectrumBaseFactory::GetSAOper(di);
    _stopprocess = false;
}

ThreadSpectrumAnalyzer::~ThreadSpectrumAnalyzer()
{
    if(mydevice != NULL)
    {
        delete mydevice;
        mydevice = NULL;
    }
}

/**
 * @brief 线程启动接口
 * @return
 */
bool ThreadSpectrumAnalyzer::Start()
{
    pthread_attr_t pth_attr;
    pthread_attr_init(&pth_attr);
    pthread_attr_setdetachstate(&pth_attr,PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&start_pth,&pth_attr,ThreadSpectrumAnalyzer::Start_Thread,this);
    if(ret != 0)
    {
        _log.LOG_ERROR("ThreadSpectrumAnalyzer 【%s】 频谱仪驱动线程创建失败！",_di.Name.data());
        return false;
    }
    return true;
}

/**
 * @brief 停止线程接口
 * @return
 */
bool ThreadSpectrumAnalyzer::Stop()
{
    _stopprocess = true;
    mydevice->Close();
    mydevice->CloseReconThread();
    return true;
}

/**
 * @brief pthread_create中传入的线程执行函数
 * @param arg
 * @return
 */
void* ThreadSpectrumAnalyzer::Start_Thread(void* arg)
{
    ThreadSpectrumAnalyzer* th = (ThreadSpectrumAnalyzer*)arg;
    th->threadprocess();
    return NULL;
}

void ThreadSpectrumAnalyzer::sendSpectrumAnalyzerValue(vector<string>& result)
{
    int pos=0;
    pos = atoi(_di.testStartPos.data());
//                        cout<<_di.Name<<"-- pos = "<<pos<<" ---- size = "<<_di.testItemCode.size()<<endl;
    //频谱仪序号从CV1开始(CV1/CV2/CV3),CV1对应result(1)
    for(int i = pos;i < _di.testItemCode.size() + pos;i++)
    {
//                            cout << "i = " << i << endl;
        string sCollectValue = _num + "$" + "CV" + IntToString(i);
        if(!result.empty())
        {
            int index = atoi(_di.testItemCode.at(i-1).data());
//                            cout<<"----- result.size() = "<<result.size()<<" --- index = "<<index<<endl;
            if(result.size() > index * 2)
            {
                m_db.Write_TagMValue(sCollectValue, result.at(index * 2));
            }
        }
        else
        {
             m_db.Write_TagMValue(sCollectValue, "");
        }
    }
}

/**
 * @brief 线程处理函数
 */
void ThreadSpectrumAnalyzer::threadprocess()
{
    string code = "";
    vector<string> result;

    _log.LOG_INFO("ThreadSpectrumAnalyzer 【%s】线程驱动已启动......",_di.Name.data());

    mydevice->Initialize(_di);

    getDeviceNum();

    pi.init();

    while (!_stopprocess)
    {
        usleep(100 * 1000);
        //设备不使能时,默认设备的判定结果为良品
        if(!getDeviceEnable())
            continue;

        processEndFlag();

        /*遍历采集点*/
        for(map<string,UnitInfo>::iterator iu = _di.Units.begin();iu != _di.Units.end();iu++)
        {
            UnitInfo unit = iu->second;
            for(map<string, Tag>::iterator it = unit.Tags.begin();it != unit.Tags.end();it++)
            {
                Tag m_tag = it->second;
                code = m_tag.TagCode;
                m_tag.MemortValue = m_db.Read_TagMValue(m_tag.TagName);
                if(code == "SW" && m_tag.MemortValue == "1")
                {
                    result.clear();
                    _log.LOG_DEBUG("ThreadSpectrumAnalyzer 【%s】 检测到功率测试SW信号! ",_di.Name.data());
                    m_db.Write_TagMValue(m_tag.TagName,"0");                    
                    string partNoId = m_db.Read_TagMValue(_di.IdFlag);
                    string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                    _log.LOG_DEBUG("ThreadSpectrumAnalyzer 【%s】设备，功率测试ID为【%s】，序列号为【%s】",_di.Name.data(),partNoId.data(),partSeqNo.data());

                    processSpectrumAnalyzerCheck(partSeqNo, partNoId, result);
                    //tcpServer
                    sendSpectrumAnalyzerValue(result);
                }
                //处理通信状态
                else if(code == "CS")
                {
                    if (_counter % 20 == 0)
                    {
                        CommunicateTest();
                        _counter = 1;
                    }
                }
                if(  (!m_tag.CTagNames.empty()) && (m_tag.MemortValue == "1")  ) //关联控制点,写给手柄测试结束
                {
                    for(list<string>::iterator it = m_tag.CTagNames.begin();it != m_tag.CTagNames.end();it++)
                    {
                        string key = *it;
                        m_db.Write_TagMValue(key,"1");
                        _log.LOG_DEBUG("ThreadSpectrumAnalyzer 【%s】设备，写关联控制点【%s】",_di.Name.data(),key.data());
                    }
                }
            }
         }
        _counter++;
    }
    _log.LOG_INFO("ThreadSpectrumAnalyzer 【%s】线程驱动已停止......",_di.Name.data());
}

/**
 * @brief ThreadSpectrumAnalyzer::noJudge
 */
void ThreadSpectrumAnalyzer::noJudge(string partNoId,string partSeqNo)
{
    if(!_di.judgeEneble.empty() && _di.judgeEneble.at(0) == 0)
    {
        _log.LOG_DEBUG("ThreadSpectrumAnalyzer 【%s】 判定使能为 0 ",_di.Name.data());
        m_db.Write_TagMValue(_di.JudgeResult,"1");
        pi.testItemJudgeResult = 1;
        pi.testItemEigenValue = "NA";
        m_db.Write_TagMValue(_di.iValue, "不进行测试，频谱仪检测合格");
        for(int i = 0;i < _di.testItemCode.size();i++)
        {
            saveToSql(partNoId, partSeqNo,i);
        }
    }
}

/**
 * @brief ThreadSpectrumAnalyzer::saveAllValueToSql
 * @param result
 */
void ThreadSpectrumAnalyzer::saveAllValueToSql(string partNoId,string partSeqNo,vector<string>& result)
{
    m_db.Write_TagMValue(_di.iValue,"频谱仪采集结果为：【" + result[0] + "/" + result[2] + "/" + result[4] + "/" + result[6] + "】");
    for(int i = 0;i < _di.testItemCode.size();i++)
    {
        pi.testItemCode = _di.testItemCode[i];
        //测试项索引值为1,2,3,由于结果集包含单个测试项的判定结果,所以采集值索引对应为2,4,6
        int index = atoi(pi.testItemCode.data());
        if(result.size() > index * 2)
        {
            pi.testItemCollectValue = result[index * 2];
            pi.testItemEigenValue = result[index * 2];
            pi.testItemJudgeResult = atoi(result[index * 2 - 1].data());
        }
        saveToSql(partNoId, partSeqNo,i);
    }
}

/**
 * @brief ThreadSpectrumAnalyzer::processSpectrumAnalyzerCheck
 * @param partSeqNo
 * @param partNoId
 * @param result
 */
void ThreadSpectrumAnalyzer::processSpectrumAnalyzerCheck(string partSeqNo, string partNoId, vector<string>& result)
{
    if(!partNoId.empty() && !partSeqNo.empty())
    {
        //进行功率测试，判定，返回测试结果
        result = mydevice->AutoPowerJudge(_di);
        if(result.size() > 0)
        {
            if(result.at(0) == "1")
            {
                _log.LOG_DEBUG("ThreadSpectrumAnalyzer 【%s】设备，功率测试ID为【%s】，序列号为【%s】，判定结果为【良品】",_di.Name.data(),partNoId.data(),partSeqNo.data());
                m_db.Write_TagMValue(_di.JudgeResult, "1");
            }
            else
            {
                _log.LOG_DEBUG("ThreadSpectrumAnalyzer 【%s】设备，功率测试ID为【%s】，序列号为【%s】，判定结果为【不良品】",_di.Name.data(),partNoId.data(),partSeqNo.data());
                m_db.Write_TagMValue(_di.JudgeResult, "0");
            }
        }
        for(int j = 0;j < result.size();j++)
        {
            _log.LOG_DEBUG("ThreadSpectrumAnalyzer 【%s】设备,功率测试结果 result=【%s】,索引 index=【%d】,大小 size=【%d】",_di.Name.data(),result.at(j).data(),j,result.size());
        }
        if(result.size() == 7)
        {
            saveAllValueToSql(partNoId,partSeqNo,result);
        }
        else
        {
            _log.LOG_ERROR("ThreadSpectrumAnalyzer 【%s】设备，结果集大小不等于7，size = 【%d】",_di.Name.data(),result.size());
        }
        //设备判定使能为0时，直接将判定结果置为1
        noJudge(partNoId,partSeqNo);
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadSpectrumAnalyzer 【%s】 序列号【%s】、id为【%s】，不进行测试，直接判定为【不良品】",_di.Name.data(),partSeqNo.data(),partNoId.data());
        string svalue;
        svalue += partNoId.empty()?("产品ID为空，"):("");
        svalue += partSeqNo.empty()?("序列号为空，"):("");
        svalue += "直接判定为【不良品】";
        m_db.Write_TagMValue(_di.iValue,svalue);
    }
}

/**
 * @brief ThreadSpectrumAnalyzer::CommunicateTest
 */
void ThreadSpectrumAnalyzer::CommunicateTest()
{
    _connectstatus = mydevice->CanAcess() ? "1" : "0";
    m_db.Write_TagMValue(_di.Alarm, _connectstatus);
    if(_connectstatus == "0")
    {
        _log.LOG_ERROR("ThreadSpectrumAnalyzer 【%s】 通信检测【失败】",_di.Name.data());
        m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
    }
    else
        m_db.Write_TagMValue(_num + "$" + "NT","");
}
