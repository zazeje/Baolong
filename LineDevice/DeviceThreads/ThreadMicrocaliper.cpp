#include "ThreadMicrocaliper.h"
#include "main.h"


/**
 * @brief ThreadMicrocaliper::ThreadMicrocaliper            测微计设备线程类驱动
 * @param di
 */
ThreadMicrocaliper::ThreadMicrocaliper(DeviceInfo di): DeviceThread(di)
{
    if(_di.type == "1")
        myTcpDevice = new MicrocaliperTcp(atoi(_di.Port.data()),_di.Ip,_di.Name);
    else if(_di.type == "2")
        myComDevice = new Microcaliper(SerialPortParameters::GetSpp(_di),_di.Name);
    _stopprocess = false;
}

/**
 * @brief ThreadMicrocaliper::~ThreadMicrocaliper
 */
ThreadMicrocaliper::~ThreadMicrocaliper()
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
 * @brief ThreadMicrocaliper::getTestItemInfo
 * @return
 */
int ThreadMicrocaliper::getTestItemInfo()
{
    int num=0;
    for(int i=0; i<_di.testItemCode.size(); i++)
    {
        if(devTest == atoi(_di.testItemCode.at(i).data()))    //高度检测设备的测试项条目为1（machine_test_item_info）
        {
            num = i;
            return num;
        }
    }
}

/**
 * @brief ThreadMicrocaliper::getDeviceParameter
 * @return
 */
double ThreadMicrocaliper::getDeviceParameter()
{
    int type = 0;
    string value;
    for(int i=0; i<_di.parameterNo.size(); i++)
    {
        type = atoi((const char*)_di.parameterNo.at(i).data());
        cout<<"--- type = "<<type<<" --- DevPara = "<<DevPara<<endl;
        if(type == DevPara)
        {
            if(_di.parameter.size() > i)
                value = _di.parameter.at(i);
        }
    }
    _log.LOG_DEBUG("ThreadMicrocaliper 【%s】设备 获取加工参数 补偿值 = 【%s】",_di.Name.data(),value.data());
    return StringToDouble(value);
}

void ThreadMicrocaliper::processEndFlag()
{
    _endFlag = m_db.Read_TagMValue(_di.EndFlag);
    //开始信号为"1"时，清空遗留信息
    if((!_endFlag.empty()) && (!_endFlag.compare("1")))
    {
         usleep(500*1000);
         _log.LOG_DEBUG("ThreadMicrocaliper 【%s】检测到结束采集信号，清空序列号，ID，复位结束采集信号点",_di.Name.data());
         m_db.Write_TagMValue(_di.EndFlag,"0");
         m_db.Write_TagMValue(_di.IdFlag,"");
         m_db.Write_TagMValue(_di.SnFlag,"");
         m_db.Write_TagMValue(_di.JudgeResult, "");
         m_db.Write_TagMValue(_di.BarCode,"");
         m_db.Write_TagMValue(gLine.Si.JudgeResult, "");
         pi.Clear();
    }
}


/**
 * @brief ThreadMicrocaliper::threadprocess
 */
void ThreadMicrocaliper::threadprocess()
{
//    int index = 0;

    _log.LOG_INFO("ThreadMicrocaliper 【%s】线程驱动已启动......",_di.Name.data());
    pi.init();


    //得到设备编号
    getDeviceNum();


    while (!_stopprocess)
    {
        usleep(100 * 1000);
        //设备不使能时,默认设备的判定结果为良品
        if(!getDeviceEnable())
            continue;
        //处理信号结束点
        processEndFlag();

//        //设备不使能时,默认设备的判定结果为良品
//        if(!_di.Enable)
//        {
//            m_db.Write_TagMValue(_di.JudgeResult,"1");
//            _log.LOG_ERROR("ThreadMarkViewCheck 【%s】 设备未使能!!!",_di.Name.data());
//            continue;
//        }
//        //处理设备结束信号
//        endFlag = m_db.Read_TagMValue(_di.EndFlag);
//        if (!endFlag.empty() && !endFlag.compare("1"))
//        {
//            usleep(500 * 1000);
//            _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 检测到结束采集信号，清空判定结果，复位结束采集信号点",_di.Name.data());
//            pi.Clear();
//            m_db.Write_TagMValue(_di.EndFlag, "");
//            m_db.Write_TagMValue(_di.IdFlag, "");
//            m_db.Write_TagMValue(_di.SnFlag, "");
//            m_db.Write_TagMValue(_di.iValue, "");
//            m_db.Write_TagMValue(_di.JudgeResult , "");
//            value.clear();
//            result1=0.0;
//            partNoId.clear();
//            partSeqNo.clear();
//        }
        //遍历采集点
        for(map<string, UnitInfo>::iterator kvpu = _di.Units.begin();kvpu != _di.Units.end();kvpu++)
        {
            for(map<string, Tag>::iterator kvpt = kvpu->second.Tags.begin();kvpt != kvpu->second.Tags.end();kvpt++)
            {
                Tag& tag = kvpt->second;
                string name = tag.TagCode;
                tag.MemortValue = m_db.Read_TagMValue(tag.TagName);
                //侧漏工位SB1、SB2
                if(name == "SB1" && tag.MemortValue == "1")
                {
                    _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 检测到高度检测信号【%s】",_di.Name.data(),name.data());
                    clearSB1Data(tag.TagName);
                    m_partNoId = m_db.Read_TagMValue(_di.IdFlag);
                    m_partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                     _log.LOG_DEBUG("ThreadMicrometer 【%s】 侧漏工位SB1 id为【%s】 序列号为【%s】",_di.Name.data(),m_partNoId.data(),m_partSeqNo.data());
                    //处理侧漏工位第1次信号
                    processLeakageSB1(m_partNoId, m_partSeqNo);
                }
                //侧漏工位SB1、SB2
                else if(name == "SB2" && tag.MemortValue == "1")
                {
                    _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 检测到高度检测信号【%s】",_di.Name.data(),name.data());                    
                    clearSB2Data(tag.TagName);
                    m_partNoId = m_db.Read_TagMValue(_di.IdFlag);
                    m_partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                    _log.LOG_DEBUG("ThreadMicrometer 【%s】 侧漏工位SB2 id为【%s】 序列号为【%s】",_di.Name.data(),m_partNoId.data(),m_partSeqNo.data());
                    //处理侧漏工位第2、3次信号
                    processLeakageSB2(m_partNoId, m_partSeqNo);
                    //保存信息到pi结构体
                    saveInfotoPi();
                    //保存数据到sql数据库
                    saveToSql(m_partNoId, m_partSeqNo,0);
                    //上传上位机信息
                    sendValueToTcpServer(m_collectValue);
                }
                //PCB点胶工位SB3
                else if(name == "SB3" && tag.MemortValue == "1")
                {
                    _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 检测到高度检测信号【%s】",_di.Name.data(),name.data());
                    clearSB3Data(tag.TagName);
                    m_partNoId = m_db.Read_TagMValue(_di.IdFlag);
                    m_partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                    _log.LOG_DEBUG("ThreadMicrometer 【%s】 id为【%s】 序列号为【%s】",_di.Name.data(),m_partNoId.data(),m_partSeqNo.data());
                    //处理PCB点胶工位SB3
                    processLeakageSB3(m_partNoId, m_partSeqNo);
                    //保存信息到pi结构体
                    saveInfotoPi();
                    //保存数据到sql数据库
                    saveToSql(m_partNoId, m_partSeqNo,0);
                    //上传上位机信息
                    sendValueToTcpServer(m_collectValue);
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
    _log.LOG_INFO("ThreadMicrocaliper 【%s】线程驱动已停止......",_di.Name.data());
}

/**
 * @brief ThreadMicrocaliper::Start_Thread
 * @param arg
 * @return
 */
void *ThreadMicrocaliper::Start_Thread(void* arg)
{
    ThreadMicrocaliper* pth = (ThreadMicrocaliper*)arg;
    pth->threadprocess();
    return NULL;
}

/**
 * @brief ThreadMicrocaliper::Start
 * @return
 */
bool ThreadMicrocaliper::Start()
{
    pthread_attr_t pth_attr;
    pthread_attr_init(&pth_attr);
    pthread_attr_setdetachstate(&pth_attr,PTHREAD_CREATE_DETACHED);
    if(0 != pthread_create(&pth,&pth_attr,ThreadMicrocaliper::Start_Thread,this))
    {
        _log.LOG_ERROR("ThreadMicrocaliper 【%s】 高度检测设备采集线程创建失败",_di.Name.data());
        return false;
    }
    return true;
}

/**
 * @brief ThreadMicrocaliper::Stop
 * @return
 */
bool ThreadMicrocaliper::Stop()
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
 * @brief ThreadMicrocaliper::clearData
 * @param tagName
 */
void ThreadMicrocaliper::clearSB1Data(string tagName)
{
    m_db.Write_TagMValue(_di.iValue, "");
    m_result1 = 0.0;
    m_db.Write_TagMValue(tagName, "0");
}

/**
 * @brief ThreadMicrocaliper::processLeakageSB1
 */
void ThreadMicrocaliper::processLeakageSB1(string partNoID, string partSeqNo)
{
    if(!partNoID.empty() && !partSeqNo.empty())
    {
        m_result1 = getLeakageResult();
        _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 result1 = 【%.3f】",_di.Name.data(),m_result1);
        m_db.Write_TagMValue(_di.iValue, "高度检测结果 result1 = 【" + DoubleToString(m_result1,"%.3f") + "】");
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 获取序列号、id为【空】，不进行测试，直接判定为【不良品】",_di.Name.data());
        m_db.Write_TagMValue(_di.iValue, "扫码失败，不进行测试，直接判定为【不良品】");
    }
}

/**
 * @brief ThreadMicrocaliper::processLeakageSB2
 */
void ThreadMicrocaliper::processLeakageSB2(string partNoID, string partSeqNo)
{
    int index = 0;
    if(!partNoID.empty() && !partSeqNo.empty())
    {

        if(_di.testItemCode.size() > 0)
        {
            if(fabs(_di.minValue.at(index)) <= EPSINON || fabs(_di.maxValue.at(index)) <= EPSINON)
            {
                m_db.Write_TagMValue(_di.testParaAlarm, "1");
                _log.LOG_ERROR("ThreadMicrocaliper 【%s】 SB2 获得测试项参数失败",_di.Name.data());
            }
            //获取第二、三次采集值
            getResult();
            //开始判定
            startToJudge(index);
        }
        else
        {
            m_db.Write_TagMValue(_di.testParaAlarm, "1");
            _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 获取【测试项判定参数】失败",_di.Name.data());
            m_db.Write_TagMValue(_di.iValue, "获取【测试项判定参数】失败");
        }

        //设备使能为0时，不进行判定，结果置为1
        noJudge();
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 获取序列号、id为【空】，不进行测试，直接判定为【不良品】",_di.Name.data());
        string svalue;
        svalue += partNoID.empty()?("产品ID为空，"):("");
        svalue += partSeqNo.empty()?("序列号为空，"):("");
        svalue += "直接判定为【不良品】";
        m_db.Write_TagMValue(_di.iValue,svalue);
    }
}

/**
 * @brief ThreadMicrocaliper::getLeakageResult
 * @return
 */
double ThreadMicrocaliper::getLeakageResult()
{
    double result = 0.00;
    if(_di.type == "1")
        result = myTcpDevice->GetValue();
    else if(_di.type == "2")
        result = myComDevice->GetValue();
    return result;
}

/**
 * @brief ThreadMicrocaliper::startToJudge
 * @param index
 */
void ThreadMicrocaliper::startToJudge(int index)
{
    /******************************************************
     *
     *  第一次大漏判定：值2-值1≥0.015mm  判为合格，反则不合格；
     *
     *  第二次微漏判定：|值2-值3|≤0.03mm 判为合格，反则不合格；
     *
     * ****************************************************/
    if(((m_result2 - m_result1 - _di.minValue.at(index)) >= (-EPSINON))\
            && (fabs(m_result2 - m_result3) - _di.maxValue.at(index)) <= EPSINON)
    {
        pi.testItemJudgeResult = 1;
//                                pi.testItemEigenValue = "OK";
        m_db.Write_TagMValue(_di.JudgeResult, "1");
        _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 高度检测判定【成功】",_di.Name.data());
    }
    else
    {
        pi.testItemJudgeResult = 0;
//                                pi.testItemEigenValue = "NG";
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 高度检测判定【失败】",_di.Name.data());
    }
    m_EigenValue = DoubleToString(m_result2 - m_result1,"%.3f").append("/").append(DoubleToString(fabs(m_result2 - m_result3),"%.3f"));
}

/**
 * @brief ThreadMicrocaliper::getResult
 */
void ThreadMicrocaliper::getResult()
{
    /**************************************************
     *
     *  侧漏工位：
     *  第二次采集和第三次采集之间，需间隔80秒
     *
     * ***********************************************/
    m_result2 = getLeakageResult();
    _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 result2 = 【%.3f】",_di.Name.data(),m_result2);
    m_db.Write_TagMValue(_di.iValue, "高度检测结果 result2 = 【" + DoubleToString(m_result2,"%.3f") + "】");

    sleep(78);

    m_result3 = getLeakageResult();
    _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 result3 = 【%.3f】",_di.Name.data(),m_result3);
    m_db.Write_TagMValue(_di.iValue, "高度检测结果 result3 = 【" + DoubleToString(m_result3,"%.3f") + "】");

    m_collectValue = DoubleToString(m_result1,"%.3f").append("/").append(DoubleToString(m_result2,"%.3f")).append("/").append(DoubleToString(m_result3,"%.3f"));
    _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 高度检测结果 result = 【%s】",_di.Name.data(),m_collectValue.data());
    m_db.Write_TagMValue(_di.iValue, "全部高度检测结果 result = 【" + m_collectValue + "】");
}


void ThreadMicrocaliper::saveInfotoPi()
{
    pi.testItemCode = _di.testItemCode.at(0);
    pi.testItemCollectValue = m_collectValue;
    pi.testItemEigenValue = m_EigenValue;
    if(pi.testItemEigenValue.empty())
        pi.testItemEigenValue = "NG";
    _log.LOG_DEBUG("ThreadMicrocaliper 【%s】特征值为【%s】",_di.Name.data(),pi.testItemEigenValue.data());
}

void ThreadMicrocaliper::clearSB2Data(string tagName)
{
    m_result2 = 0.0;
    m_result3 = 0.0;
    m_collectValue.clear();
    m_EigenValue.clear();
    m_db.Write_TagMValue(tagName, "0");
}

void ThreadMicrocaliper::clearSB3Data(string tagName)
{
    m_db.Write_TagMValue(_di.iValue, "");
    m_result = 0.0;
    m_collectValue.clear();
    m_EigenValue.clear();
    m_db.Write_TagMValue(tagName, "0");
}

void ThreadMicrocaliper::processLeakageSB3(string partNoID, string partSeqNo)
{
    int index = 0;
    if(!partNoID.empty() && !partSeqNo.empty())
    {
        m_offset = getOffsetVaule();
#ifndef _Debug
        m_result = getLeakageResult();
#else
        if((atoi(partNoID.data()) % 2) == 0)
        {
            m_result = 5.8;
        }
        else
        {
            m_result = 6.1;
        }
//        if((atoi(partNoID.data()) % 2) == 0)
//        {
//            m_result = 5.8;
//        }
//        else
//        {
//            m_result = 6.1;
//        }
//        if((atoi(partNoID.data()) % 3) == 0)
//        {
//            m_result = 6.2;
//        }
#endif
        if(_di.testItemCode.size() > 0)
        {
            startToJudgeSB3(index);
        }
        else
        {
            m_db.Write_TagMValue(_di.testParaAlarm, "1");
            _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 获取【测试项判定参数】失败",_di.Name.data());
            m_db.Write_TagMValue(_di.iValue, "获取【测试项判定参数】失败");
        }
//        pi.testItemCode = _di.testItemCode.at(index);
        //设备使能为0时，不进行判定，结果置为1
        noJudge();
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 获取序列号、id为【空】，不进行测试，直接判定为【不良品】",_di.Name.data());
        m_db.Write_TagMValue(_di.iValue, "扫码失败，不进行测试，直接判定为【不良品】");
    }
}


double ThreadMicrocaliper::getOffsetVaule()
{
    double offset=0;
    DevPara = Offset;
    offset = getDeviceParameter();
//    if(fabs(offset) <= EPSINON)
//    {
//        m_db.Write_TagMValue(_di.devParaAlarm, "1");
//        _log.LOG_ERROR("ThreadMicrocaliper 【%s】 SB3 获得加工参数失败",_di.Name.data());
//    }
    return offset;
}

void ThreadMicrocaliper::startToJudgeSB3(int index)
{
    _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 result = 【%.3f】",_di.Name.data(),(m_result - m_offset));
    if((fabs(_di.minValue.at(index)) <= EPSINON || fabs(_di.maxValue.at(index)) <= EPSINON) ||
            (_di.minValue.at(index) >= _di.maxValue.at(index)))
    {
        m_db.Write_TagMValue(_di.testParaAlarm, "1");
        _log.LOG_ERROR("ThreadMicrocaliper 【%s】 SB3 获得测试项参数失败",_di.Name.data());
    }

    if((m_result - m_offset) > _di.minValue.at(index) && (m_result - m_offset) < _di.maxValue.at(index))
    {
        pi.testItemJudgeResult = 1;
        m_db.Write_TagMValue(_di.JudgeResult, "1");
        _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 高度检测判定【成功】",_di.Name.data());
    }
    else
    {
        pi.testItemJudgeResult = 0;
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 高度检测判定【失败】",_di.Name.data());
    }
    m_collectValue = DoubleToString((m_result - m_offset), "%.3f");
    m_EigenValue = m_collectValue;
    m_db.Write_TagMValue(_di.iValue, "高度检测结果 result = 【" + m_collectValue + "】");
}

/**
 * @brief ThreadMicrocaliper::CommunicateTest
 */
void ThreadMicrocaliper::CommunicateTest()
{
    if(_di.type == "1")
        _connectstatus = myTcpDevice->CanAcess() ? "1" : "0";
    else if(_di.type == "2")
        _connectstatus = myComDevice->CanAcess() ? "1" : "0";
    m_db.Write_TagMValue(_di.Alarm, _connectstatus);
    if(!_connectstatus.compare("0"))
    {
        _log.LOG_ERROR("ThreadMicrocaliper 【%s】 通信检测【失败】",_di.Name.data());
        m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
    }
    else
    {
        m_db.Write_TagMValue(_num + "$" + "NT","");
//                            _log.LOG_DEBUG("ThreadMicrocaliper 【%s】 通信检测【成功】",_di.Name.data());
    }
}
