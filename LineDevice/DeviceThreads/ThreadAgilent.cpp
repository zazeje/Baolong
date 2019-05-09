#include "ThreadAgilent.h"
#include "main.h"


extern vector<string> PriorPartIdVector;        //ThreadKeyenceSR --> GetPartID()函数调用获得

/**
 * @brief ThreadAgilent::ThreadAgilent      构造函数
 * @param di                                设备信息
 */
ThreadAgilent::ThreadAgilent(DeviceInfo di) : DeviceThread(di)
{
    pcount = 0;
    _stopColl = true;
    _reinitflag = false;
    _counter = 0;   
    _stopprocess = false;
    repeatFlag = true;
    _workTimes = 0;
    devTest = lowPassrate;
    m_displayValue = "";
    //安捷伦默认端口9004
    if (di.Port.empty())
        _di.Port = "9004";
    if(_di.type == "1")
        myTcpDevice = new AgilentMultimeterTcp(atoi(_di.Port.data()),_di.Ip,_di.Name);
    else if(_di.type == "2")
        myComDevice = new AgilentMultimeterSerial(SerialPortParameters::GetSpp(_di),_di.Name);
}

/**
 * @brief ThreadAgilent::threadprocess      安捷伦驱动线程处理函数
 */
void ThreadAgilent::threadprocess()
{
    _log.LOG_INFO("【%s】线程驱动已启动.....",_di.Name.data());

    //得到设备编号
    getDeviceNum();
    //初始化结构体参数
    pi.init();
    //初始化设备
    Init();
#ifdef _TruckLine
    pthread_t _collectThread;
    //创建采集线程
    int ret = pthread_create(&_collectThread,NULL,ThreadAgilent::CollectData,this);
    if(ret != 0)
    {
        _log.LOG_ERROR("ThreadAgilent 【%s】 采集线程创建失败！",_di.Name.data());
        return;
    }
#else
    if((!workCenterNo.compare("3218")) || (!workCenterNo.compare("3309") || (!workCenterNo.compare("20035")) || (!workCenterNo.compare("3109"))))
    {
        pthread_t _collectThread;
        //创建采集线程
        int ret = pthread_create(&_collectThread,NULL,ThreadAgilent::CollectData,this);
        if(ret != 0)
        {
            _log.LOG_ERROR("ThreadAgilent 【%s】 采集线程创建失败！",_di.Name.data());
            return;
        }
    }
#endif

    while(!_stopprocess)
    {
        usleep(100 * 1000);
        //设备不使能时,默认设备的判定结果为良品
        if(!getDeviceEnable())
            continue;
        //处理结束信号点
        processEndFlag();

        //遍历设备层级下的采集单元
        for(map<string,UnitInfo>::iterator it = _di.Units.begin();it != _di.Units.end();it++)
        {
            //遍历采集单元下的采集点
            for (map<string,Tag>::iterator im = it->second.Tags.begin();im != it->second.Tags.end();im++)
            {
                Tag kvpt = im->second;
                string code = kvpt.TagCode;
                kvpt.MemortValue= m_db.Read_TagMValue(kvpt.TagName);

                /**************************************************************
                 *
                 *      D结构线第一、三、五次SC为静态电流开始（需要切换为uA）
                 *
                 *      D结构线第二、四、六次为发射电流开始（切换为mA）
                 *
                 * ***********************************************************/
                if (!code.compare("SC"))
                {
                    if(!kvpt.MemortValue.compare("1"))
                    {

                        clearData(kvpt.TagName);
                        _log.LOG_DEBUG("ThreadAgilent 【%s】 检测到电压或电流开始采集信号!",_di.Name.data());
#ifndef _TruckLine
                        if((!workCenterNo.compare("3218")) || (!workCenterNo.compare("3309") || (!workCenterNo.compare("20035")) || (!workCenterNo.compare("3109"))))
                        {
                            _stopColl = false;
                        }
                        else
                        {
                            _workTimes++;
                            CollectCurrent(_workTimes);                        //批量读取电流值
                            string partNoId = m_db.Read_TagMValue(_di.IdFlag);
                            string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                            _log.LOG_DEBUG("ThreadAgilent 【%s】设备 序列号为【%s】 产品ID为【%s】",_di.Name.data(),partSeqNo.data(),partNoId.data());
                            int index = processD3Line(partNoId, partSeqNo);
                            _log.LOG_DEBUG("ThreadAgilent 【%s】特征值为【%s】",_di.Name.data(),pi.testItemEigenValue.data());
                            saveToSql(partNoId, partSeqNo,index);
                            sendValueToTcpServer(pi.testItemEigenValue);
                        }
#endif

#ifdef _Debug
                        pi.testItemJudgeResult = 1;
#endif

#ifdef _TruckLine
                        _stopColl = false;
#endif
                    }
                }
#ifdef _TruckLine
                else if (!code.compare("EC"))
                {
                    if(!kvpt.MemortValue.compare("1"))
                    {
                        _stopColl = true;
                        usleep(500*1000);
                        m_db.Write_TagMValue(kvpt.TagName,"0");
                        string partNoId = m_db.Read_TagMValue(_di.IdFlag);
                        string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                        _log.LOG_DEBUG("ThreadAgilent 【%s】设备 序列号为【%s】 产品ID为【%s】",_di.Name.data(),partSeqNo.data(),partNoId.data());
                        processTruckLine(partNoId, partSeqNo);
                        _log.LOG_DEBUG("ThreadAgilent 【%s】特征值为【%s】",_di.Name.data(),pi.testItemEigenValue.data());
                        saveToSql(partNoId, partSeqNo,0);
                        sendValueToTcpServer(pi.testItemEigenValue);
                    }
                }
#else
                else if (!code.compare("EC"))
                {
                    if((!workCenterNo.compare("3218")) || (!workCenterNo.compare("3309") || (!workCenterNo.compare("20035")) || (!workCenterNo.compare("3109"))) )
                    {
                        if(!kvpt.MemortValue.compare("1"))
                        {
                            _stopColl = true;
                            usleep(500*1000);
                            m_db.Write_TagMValue(kvpt.TagName,"0");
                            string partNoId = m_db.Read_TagMValue(_di.IdFlag);
                            string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                            _log.LOG_DEBUG("ThreadAgilent 【%s】设备 序列号为【%s】 产品ID为【%s】",_di.Name.data(),partSeqNo.data(),partNoId.data());
                            processTruckLine(partNoId, partSeqNo);
                            _log.LOG_DEBUG("ThreadAgilent 【%s】特征值为【%s】",_di.Name.data(),pi.testItemEigenValue.data());
                            saveToSql(partNoId, partSeqNo,0);
                            sendValueToTcpServer(pi.testItemEigenValue);
                        }
                    }
                }
#endif
                //处理通信状态
                else if(!code.compare("CS"))
                {
                    //正在采集数据则通信状态点为"1"
                    if(!_stopColl)
                        _connectstatus = "1";
                    else
                    {
                        //两秒钟检测一次通信状态
                        if(_counter % 20 == 0)
                        {
                            CommunicateTest();
                            _counter = 1;
                        }
                    }
                }
            }
        }
        _counter++;
    }
    _stopColl = true;
    _log.LOG_INFO("ThreadAgilent 【%s】 线程驱动已停止....",_di.Name.data());
}

/**
 * @brief ThreadAgilent::Start_Thread           线程处理接口
 * @param arg
 * @return
 */
void *ThreadAgilent::Start_Thread(void* arg)
{
    ThreadAgilent *th = (ThreadAgilent*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief ThreadAgilent::Start          驱动线程启动
 * @return
 */
bool ThreadAgilent::Start()
{
    //将线程设置为detach模式，线程一关闭则自动释放线程资源
    pthread_attr_init(&pthattr);
    pthread_attr_setdetachstate(&pthattr,PTHREAD_CREATE_DETACHED);

    int ret = pthread_create(&pth, &pthattr, ThreadAgilent::Start_Thread, this);
    if(ret != 0)
    {
        _log.LOG_ERROR("ThreadAgilent 【%s】 驱动线程创建失败!",_di.Name.data());
        return false;
    }
    return true;
}

/**
 * @brief ThreadAgilent::InitAgilentDevice             初始化设备(初始化量程设置)
 * @return
 */
bool ThreadAgilent::InitAgilentDevice()
{
    string currentenablepara;
    string outputformatpara;
    string para;
    if(/*_di.type == "1"*/1)
    {
#ifdef _TruckLine
        //初始化皮安计电流参数
        para = getMachinePara();
        _log.LOG_DEBUG("ThreadAgilent 【%s】 皮安计电流使能参数 para=【%s】",_di.Name.data(),para.data());
        if(para.empty())
        {
            m_db.Write_TagMValue(_di.devParaAlarm,"1");
        }
        else
        {
            if(_di.testItemCode.size() > 0)
            {
                //获得测试项条目的下标
                int index = getTestItemCode();
            /*****************************************
             *
             *       lowPassrate = 1,        //静态电流/静态电压
             *       maxLimit,               //发射电流
             *       pressJudge,             //环境气压判定
             *       tpmsJudge,              //手柄气压判定
             *       minLimit                //发射电压判定
             *
             * ***************************************/
                switch(index)
                {
                    case staticElectricTest:
                        _coefficient = 1000*1000;
                    break;
                    case sendElectricTest:
                        _coefficient = 1000;
                    break;
                    case sendVoltageTest: case staticVoltageTest:
                        _coefficient = 1;
                    break;
                    default:
                    break;
                }
            }
        }
        //初始化电流参数

        myTcpDevice->InitPara(para,"");
#else
        if((!workCenterNo.compare("3218")) || (!workCenterNo.compare("3309")) || (!workCenterNo.compare("20035"))
                || (!workCenterNo.compare("3109")))
        {
            para = getMachinePara();
            _log.LOG_DEBUG("ThreadAgilent 【%s】 皮安计电流使能参数 para=【%s】",_di.Name.data(),para.data());
            if(para.empty())
            {
                m_db.Write_TagMValue(_di.devParaAlarm,"1");
            }
            else
            {
                if(_di.testItemCode.size() > 0)
                {
                    //获得测试项条目的下标
                    int index = getTestItemCode();
                /*****************************************
                 *
                 *       lowPassrate = 1,        //静态电流/静态电压
                 *       maxLimit,               //发射电流
                 *       pressJudge,             //环境气压判定
                 *       tpmsJudge,              //手柄气压判定
                 *       minLimit                //发射电压判定
                 *
                 * ***************************************/
                    switch(index)
                    {
                        case staticVoltageTest:
                            _coefficient = 1;
                        break;
                        default:
                        break;
                    }
                }
            }
            //初始化电流参数

            if(_di.type == "1")
                myTcpDevice->InitPara(para,"");
            else if(_di.type == "2"){
                _log.LOG_DEBUG("ThreadAgilent 【%s】para【%s】 ",_di.Name.data(),para.data());
                myComDevice->SendCommand(para);
            }
        }
        else
        {
            //初始化皮安计电流使能参数
            devPara = currentEnable;
            currentenablepara = getParameterNoInfo();
            _log.LOG_DEBUG("ThreadAgilent 【%s】 皮安计电流使能参数 para=【%s】",_di.Name.data(),currentenablepara.data());
            if(currentenablepara.empty())
                m_db.Write_TagMValue(_di.devParaAlarm,"1");

            //初始化皮安计输出参数
            devPara = outputFormat;
            outputformatpara = getParameterNoInfo();
            _log.LOG_DEBUG("ThreadAgilent 【%s】 皮安计输出参数 para=【%s】",_di.Name.data(),outputformatpara.data());
            if(outputformatpara.empty())
                m_db.Write_TagMValue(_di.devParaAlarm,"1");

            //初始化电流使能参数和输出参数
            myTcpDevice->InitPara(currentenablepara, outputformatpara);
            if(!myTcpDevice->InitDevice())
                return false;
            else
                return true;
        }
#endif
    }
//    else if(_di.type == "2")
//    {
//        devPara = currentEnable;
//        if(para.empty())
//            m_db.Write_TagMValue(_di.devParaAlarm,"1");
//        para = getParameterNoInfo();
//        myComDevice->SendCommand(para);
//        _log.LOG_DEBUG("ThreadAgilent 【%s】 皮安计电流使能参数 para=【%s】",_di.Name.data(),para.data());
//        devPara = outputFormat;
//        if(para.empty())
//            m_db.Write_TagMValue(_di.devParaAlarm,"1");
//        para = getParameterNoInfo();
//        _log.LOG_DEBUG("ThreadAgilent 【%s】 皮安计输出参数 para=【%s】",_di.Name.data(),para.data());
//        return myComDevice->SendCommand(para);
//    }
//    return false;
}

/**
 * @brief ThreadAgilent::    获得测试项参数下标索引
 * @return
 */
int ThreadAgilent::getTestItemInfo()
{
    int num=0;
    for(int i = 0;i < _di.testItemCode.size();i++)
    {
        if(devTest == atoi(_di.testItemCode.at(i).data()))
            num = i;
    }
    return num;
}


int ThreadAgilent::getTestItemCode()
{
    for(int i=0; i<_di.testItemCode.size();i++)
    {
        return atoi(_di.testItemCode.at(i).data());
    }
}

/**
 * @brief ThreadAgilent::getParameterNoInfo     获取加工参数(machine_parameter_info表)
 * @return
 */
string ThreadAgilent::getParameterNoInfo()
{
    string data = "";
    for(int i = 0;i < _di.parameterNo.size();i++)
    {
        int type = atoi((const char*)_di.parameterNo.at(i).data());
        if(type == devPara)
        {
            if(_di.parameter.size() > i)
                data = _di.parameter.at(i);
            else
                _log.LOG_ERROR("ThreadAgilent 【%s】 获取加工参数失败",_di.Name.data());
        }
    }
    if(data.empty())
        _log.LOG_ERROR("ThreadAgilent 【%s】 获取加工参数【失败】 参数数组大小为：【%d】",_di.Name.data(),_di.parameterNo.size());
    return data;
}

/**
 * @brief ThreadAgilent::getMachinePara                 获取加工参数
 * @return
 */
string ThreadAgilent::getMachinePara()
{
    string data = "";
    for(int i = 0;i < _di.parameter.size();i++)
    {
        data = _di.parameter.at(i);
    }
    return data;
}

/**
 * @brief ThreadAgilent::Stop           驱动线程关闭
 * @return
 */
bool ThreadAgilent::Stop()
{
    _stopprocess = true;
    _stopColl = true;
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


/**
 * @brief ThreadAgilent::CollectDataProcess     采集数据
 */
void ThreadAgilent::CollectDataProcess()
{
#ifndef _TruckLine
    //初始化，传给tcpServer采集值
    m_db.Write_TagMValue(_num + "$CV1", "--");
    m_db.Write_TagMValue(_num + "$CV2", "--");
#else
    int pos=0;
    pos = atoi(_di.testStartPos.data());
    string sCollectValue = _num + "$" + "CV" + IntToString(pos);
#endif
    /*************************************************************
     *
     *  D结构线第一、三、五次SC为静态电流开始（_coefficient = 1000*1000）
     *
     *  D结构线第二、四、六次SC为发射电流开始（_coefficient = 1000）
     *
     * ***********************************************************/


    while(!_stopprocess)
    {
        while(!_stopColl)
        {
            string value;
            //读取数据
            if(_di.type == "1")
                value = myTcpDevice->GetValue();
            else if(_di.type == "2")
                value = myComDevice->GetValue();

//                _log.LOG_DEBUG("【%s】 value = %s",_di.Name.data(),value.data());

            if(value.empty())
                continue;
            double tmp = atof(value.data());
            if(fabs(tmp) * _coefficient > 10000)
                continue;
            _iValue = DoubleToString( tmp*_coefficient, "%.2f");
            //将瞬时值写入内存数据库中瞬时值点位,显示界面调用
#ifdef _D2Line
            m_db.Write_TagMValue(_di.iValue,m_displayValue + _iValue);
#else
            m_db.Write_TagMValue(_di.iValue,_di.Name + "采集值为：【 " + _iValue + "】");
#endif
#ifndef _TruckLine

            if(_workTimes % 2 != 0)
            {
                string sCollectValue1 = _num + "$" + "CV1";
                m_db.Write_TagMValue(sCollectValue1, _iValue);
            }
            else
            {
                string sCollectValue2 = _num + "$" + "CV2";
                m_db.Write_TagMValue(sCollectValue2, _iValue);
            }
#else
            m_db.Write_TagMValue(sCollectValue, _iValue);
#endif
            if(!_stopColl)
                _cValue.append(_iValue).append("/");
            usleep(50*1000);
        }
        usleep(100*1000);
    }
}


/**
 * @brief ThreadAgilent::getTestPara                        设置测试、判定参数
 * @param workTimes
 */
void ThreadAgilent::setTestPara(int workTimes)
{
    if(workTimes % 2 == 0)
    {
        devPara = sendElectric;
        devTest = maxLimit;
        _coefficient = 1000;
        _log.LOG_DEBUG("ThreadAgilent 【%s】 切换为【mA】档位 测试发射电流 .....",_di.Name.data());
        m_db.Write_TagMValue(_di.iValue, "发射电流测试中.....");
    }
    else
    {
        devPara = staticElectric;
        devTest = lowPassrate;
        _coefficient = 1000*1000;
        _log.LOG_DEBUG("ThreadAgilent 【%s】 切换为【uA】档位 测试静态电流 .....",_di.Name.data());
        m_db.Write_TagMValue(_di.iValue, "静态电流测试中.....");
    }
}

/**
 * @brief ThreadAgilent::CollectCurrent             批量读取电流值
 * @param workTimes
 */
void ThreadAgilent::CollectCurrent(int workTimes)
{
    //设置测试、判定参数
    setTestPara(workTimes);
    //获取采集参数
    string para = getParameterNoInfo();
    if(_di.type == "1")
    {
        myTcpDevice->SendCommand(para);
        _cValue = myTcpDevice->GetValueByBatch(_coefficient);
    }
}


/**
 * @brief 保存产品序列号及ID
 * @param 小板序列号数组
 */
void ThreadAgilent::SaveProductSeqNoAndNoId(vector<string>& vecSeqNo)
{
    vecSeqNo = GetSmallBoardSeraialNo();

    int pos = ((_workTimes -1) / 2) * 4 + (_devicePosition % 4);
    if(vecSeqNo.size() > 0)
    {
        //从数据库查询得到小板序列号数组，根据测试次数以及设备编号，从数组找出对应小板序列号
        _log.LOG_DEBUG("ThreadAgilent 【%s】 小板序列号 PartSeqNo size = 【%d】 ",_di.Name.data(),vecSeqNo.size());
        if((vecSeqNo.size() - 1 ) >= pos)
            pi.partSeqNo = vecSeqNo.at(pos);
        else
            pi.partSeqNo = "";
    }
    if(PriorPartIdVector.size() > 0)
    {
        _log.LOG_DEBUG("ThreadAgilent 小板产品ID号 PriorPartIdVector size = 【%d】",PriorPartIdVector.size());
        if((PriorPartIdVector.size() - 1) >= pos)
            pi.partNoId = PriorPartIdVector.at(pos);
        else
            pi.partNoId = "";
    }
    _log.LOG_DEBUG("ThreadAgilent 【%s】设备 序列号为【%s】 产品ID为【%s】 _workTimes为【%d】 pos为【%d】",_di.Name.data(),pi.partSeqNo.data(),pi.partNoId.data(),_workTimes,pos);
}

/**
 * @brief ThreadAgilent::CollectData        采集线程接口
 * @param arg
 * @return
 */
void* ThreadAgilent::CollectData(void *arg)
{
    ThreadAgilent *th = (ThreadAgilent*)arg;
    th->CollectDataProcess();
    return NULL;
}

/**
 * @brief ThreadAgilent::processEndFlag         处理结束信号点
 */
void ThreadAgilent::processEndFlag()
{
    _endFlag = m_db.Read_TagMValue(_di.EndFlag);
    //开始信号为"1"时，清空遗留信息
    if(!_endFlag.empty() && !_endFlag.compare("1"))
    {
        usleep(500 * 1000);
        _log.LOG_DEBUG("ThreadAgilent 【%s】 检测到结束采集信号，清空瞬时值，特征值，复位结束采集信号点",_di.Name.data());
        m_db.Write_TagMValue(_di.EndFlag,"");
        //清空瞬时值和累积值
        _iValue.clear();
        _cValue.clear();
        m_displayValue.clear();
        pi.Clear();
        m_PartSeqNoVector.clear();
        m_PartIdVector.clear();
        m_db.Write_TagMValue(_di.iValue, "");
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _workTimes = 0;
        m_JR.clear();
    }
}

/**
 * @brief ThreadAgilent::processD3Line
 */
int ThreadAgilent::processD3Line(string partNoId, string partSeqNo)
{
    int index=0;
    if(!partSeqNo.empty() && !partNoId.empty())
    {
        if(_di.testItemCode.size() > 0)
        {
            //获得测试项条目的下标
            index = getTestItemInfo();
            //处理采集值,进行判定
            ProcessCValue(pi, index, devTest, _cValue);
            pi.testItemCode = _di.testItemCode.at(index);
            getJRVaule();
        }
        else
        {
            m_db.Write_TagMValue(_di.testParaAlarm, "1");
            _log.LOG_ERROR("ThreadAgilent 【%s】 获取【测试项判定参数】失败",_di.Name.data());
            m_db.Write_TagMValue(_di.iValue, "缺少【测试项判定参数】");
        }

        noJudge();
    }
    else
    {
        _log.LOG_DEBUG("ThreadAgilent 【%s】获取序列号为【空】，不参与测试，直接判定为【不良品】",_di.Name.data());
        m_db.Write_TagMValue(_di.JudgeResult,"0");          //不良品
        string svalue;
        svalue += partNoId.empty()?("产品ID为空，"):("");
        svalue += partSeqNo.empty()?("序列号为空，"):("");
        svalue += "直接判定为【不良品】";
        m_db.Write_TagMValue(_di.iValue,svalue);
    }

    startJudge();

    if(_workTimes > 2)
        _workTimes = 0;

    if(pi.testItemEigenValue.empty())
        pi.testItemEigenValue = "NG";

    m_db.Write_TagMValue(_di.iValue,"电流表采集值为：【 " + pi.testItemEigenValue + "】");
    return index;
}

/**
 * @brief ThreadAgilent::processTruckLine               卡车线判定方式
 * @param partNoId
 * @param partSeqNo
 * @return
 */
int ThreadAgilent::processTruckLine(string partNoId, string partSeqNo)
{
    int index=0;
    if(!partSeqNo.empty() && !partNoId.empty())
    {
        if(_di.testItemCode.size() > 0)
        {
            //获得测试项条目的下标
            index = getTestItemCode();
        /*****************************************
         *
         *       lowPassrate = 1,        //静态电流/静态电压
         *       maxLimit,               //发射电流
         *       pressJudge,             //环境气压判定
         *       tpmsJudge,              //手柄气压判定
         *       minLimit                //发射电压判定
         *
         * ***************************************/
            switch(index)
            {
                case staticElectricTest:
                    devTest = lowPassrate;
                break;
                case sendElectricTest:
                    devTest = maxLimit;
                break; 
                case sendVoltageTest: case staticVoltageTest:
                    devTest = minLimit;
                break;
                default:
                break;
            }
            //处理采集值,进行判定
            ProcessCValue(pi, 0, devTest, _cValue);
            pi.testItemCode = _di.testItemCode.at(0);
//            getJRVaule();
        }
        else
        {
            m_db.Write_TagMValue(_di.testParaAlarm, "1");
            _log.LOG_ERROR("ThreadAgilent 【%s】 获取【测试项判定参数】失败",_di.Name.data());
            m_db.Write_TagMValue(_di.iValue, "缺少【测试项判定参数】");
        }
        noJudge();
    }
    else
    {
        _log.LOG_DEBUG("ThreadAgilent 【%s】获取序列号为【空】，不参与测试，直接判定为【不良品】",_di.Name.data());
        m_db.Write_TagMValue(_di.JudgeResult,"0");          //不良品
        m_db.Write_TagMValue(_di.iValue, "获取序列号为【空】，不参与测试，直接判定为【不良品】");
    }

//    startJudge();
    if(pi.testItemJudgeResult == 1)
        m_db.Write_TagMValue(_di.JudgeResult, "1");
    else
        m_db.Write_TagMValue(_di.JudgeResult, "0");

    if(pi.testItemEigenValue.empty())
        pi.testItemEigenValue = "NG";

    m_db.Write_TagMValue(_di.iValue,_di.Name + "采集值为：【 " + pi.testItemEigenValue + "】");
    return index;
}

/**
 * @brief ThreadAgilent::processD2Line
 */
void ThreadAgilent::processD2Line()
{
    int index = 0;
    SaveProductSeqNoAndNoId(m_PartSeqNoVector);

    if(_di.testItemCode.size() > 0)
    {
        //获得测试项条目的下标
        index = getTestItemInfo();
        //处理采集值,进行判定
        ProcessCValue(pi, index, devTest, _cValue);
        pi.testItemCode = _di.testItemCode.at(index);
    }

    if(_workTimes > 6)
        _workTimes = 0;

    if(pi.testItemEigenValue.empty())
        pi.testItemEigenValue = "NG";

    m_displayValue.append(pi.testItemEigenValue).append(";");
    m_db.Write_TagMValue(_di.iValue, m_displayValue);
}

/**
 * @brief ThreadAgilent::startJudge
 */
void ThreadAgilent::startJudge()
{
    if(m_JR.size() == 2)
    {
        for(int i=0;i<m_JR.size();i++)
        {
            if(m_JR.at(i) == "0")
            {
                _log.LOG_DEBUG("ThreadAgilent 【%s】 判定为【不良品】",_di.Name.data());
                m_db.Write_TagMValue(_di.JudgeResult, "0");
                break;
            }
            if(i == m_JR.size() - 1)
            {
                _log.LOG_DEBUG("ThreadAgilent 【%s】 判定为【良品】",_di.Name.data());
                m_db.Write_TagMValue(_di.JudgeResult, "1");
            }
        }
    }
}

/**
 * @brief ThreadAgilent::getJRVaule
 */
void ThreadAgilent::getJRVaule()
{
    if(pi.testItemJudgeResult == 1)
    {
        _log.LOG_DEBUG("ThreadAgilent 【%s】 JR == 【1】",_di.Name.data());
        m_JR.push_back("1");
    }
    else
    {
        _log.LOG_DEBUG("ThreadAgilent 【%s】 JR == 【0】",_di.Name.data());
        m_JR.push_back("0");
    }
}

/**
 * @brief ThreadAgilent::sendValueToTcpServer
 * @param cValue
 */
void ThreadAgilent::sendValueToTcpServer(string cValue)
{
    //向上位机传值
#ifdef _TruckLine
    int pos=0;
    pos = atoi(_di.testStartPos.data());
    string sCollectValue = _num + "$" + "CV" + IntToString(pos);
    m_db.Write_TagMValue(sCollectValue, cValue);
#else
    if((!workCenterNo.compare("3218")) || (!workCenterNo.compare("3309")) || (!workCenterNo.compare("20035")) || (!workCenterNo.compare("3109")))
    {
        int pos=0;
        pos = atoi(_di.testStartPos.data());
        string sCollectValue = _num + "$" + "CV" + IntToString(pos);
        m_db.Write_TagMValue(sCollectValue, cValue);
    }
    else
    {
        if(_workTimes % 2 != 0)
        {
            string sCollectValue1 = _num + "$" + "CV1";
            m_db.Write_TagMValue(sCollectValue1, cValue);
        }
        else
        {
            string sCollectValue2 = _num + "$" + "CV2";
            m_db.Write_TagMValue(sCollectValue2, cValue);
        }
    }
#endif
}

/**
 * @brief ThreadAgilent::Init           初始化设备
 */
void ThreadAgilent::Init()
{
    //初始化设备
    if(!InitAgilentDevice())
    {
        //机台报警
        m_db.Write_TagMValue(_di.devInitAlarm, "1");
        //向上位机上传报警信息
        m_db.Write_TagMValue(gLine.Si.alarmStatus,"0");
        m_db.Write_TagMValue(_num + "$" + "NT", _di.Name + " 初始化【失败】!");
        m_db.Write_TagMValue(_di.iValue, "初始化【失败】");
        _log.LOG_ERROR("ThreadAgilent 【%s】 InitAgilentDevice 初始化【失败】",_di.Name.data());
    }
    else
    {
        _log.LOG_DEBUG("ThreadAgilent 【%s】 InitAgilentDevice 初始化【成功】",_di.Name.data());
        m_db.Write_TagMValue(_di.iValue, "初始化【成功】");
    }
}

/**
 * @brief ThreadAgilent::CommunicateTest            通信检测
 */
void ThreadAgilent::CommunicateTest()
{
    if(_di.type == "1")
        _connectstatus = myTcpDevice->CanAcess()? "1": "0";
    else if(_di.type == "2")
        _connectstatus = myComDevice->CanAcess() ? "1": "0";
    m_db.Write_TagMValue(_di.Alarm, _connectstatus);

    if(_connectstatus == "0")
    {
        _log.LOG_ERROR("ThreadAgilent 【%s】 通信检测【失败】",_di.Name.data());
        m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
    }
    else
    {
        m_db.Write_TagMValue(_num + "$" + "NT","");
//      _log.LOG_DEBUG("ThreadAgilent 【%s】 通信检测【成功】",_di.Name.data());
    }
}


/**
 * @brief ThreadAgilent::clearData                     清楚数据
 * @param tagName
 */
void ThreadAgilent::clearData(string tagName)
{
    _cValue.clear();
    _iValue.clear();
    pi.Clear();
    m_db.Write_TagMValue(tagName,"0");
    m_db.Write_TagMValue(_di.JudgeResult, "0");
}

/**
 * @brief ThreadAgilent::ThreadAgilent          无参构造
 */
ThreadAgilent::ThreadAgilent()
{
    _stopprocess = false;
    _reinitflag = false;
}


/**
 * @brief ThreadAgilent::~ThreadAgilent         析构函数
 */
ThreadAgilent::~ThreadAgilent()
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
