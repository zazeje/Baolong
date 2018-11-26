#include "ThreadTekDMM6500.h"
#include "main.h"
extern vector<string> PriorPartIdVector;        //ThreadKeyenceSR --> GetPartID()函数调用获得

ThreadTekDMM6500::ThreadTekDMM6500()
{
    _stopprocess = false;
    _reinitflag = false;
    type = NONE;
}

ThreadTekDMM6500::ThreadTekDMM6500(DeviceInfo di) : DeviceThread(di)
{
    pcount = 0;
    _stopColl = true;
    _reinitflag = false;
    _counter = 0;
    _stopprocess = false;
    repeatFlag = true;
    _workTimes = 0;
    m_bInitResult = false;
    devTest = lowPassrate;
    m_displayValue = "";
    m_currentgear = "";
    m_currentrange = "";

    if(_di.type == "1")
        myTcpDevice = new TekDMM6500MultimeterTcp(atoi(_di.Port.data()),_di.Ip,_di.Name);
    else
        _log.LOG_DEBUG("ThreadTekDMM6500 通讯类型参数错误");

    //泰克默认端口5025
    if (di.Port.empty())
        _di.Port = "5025";
}

ThreadTekDMM6500::~ThreadTekDMM6500()
{
    if(_di.type == "1")
    {
        if(myTcpDevice != NULL)
        {
            delete myTcpDevice;
            myTcpDevice = NULL;
        }
    }
}

bool ThreadTekDMM6500::InitTekDMM6500Device()
{
    bool bRet = true;
    if(_di.type == "1")
    {
        size_t parameterNum = _di.parameterNo.size();
        size_t testItemNum = _di.testItemCode.size();
        devPara = TekcurrGear;
        m_currentgear = getDevParaStr();
        if(m_currentgear.empty())
        {
            m_db.Write_TagMValue(_di.devParaAlarm,"1");
            bRet = false;
            _log.LOG_DEBUG("ThreadTektronix 【%s】获取泰克万用表档位参数失败",_di.Name.data());
        }
        else
        {
            if((testItemNum == 1) && (parameterNum == 2))
            {//电池电压测试,或卡车线静态电流,或发射测试电流
                devPara = TekNone;
                m_currentrange = getDevParaStr();
                if(m_currentrange.empty())
                {
                    m_db.Write_TagMValue(_di.devParaAlarm,"1");
                    bRet = false;
                }
                //获得测试项条目的下标
                int index = getTestItemCode(0);
            /*****************************************
             *
             *       lowPassrate = 1,        //静态电流/静态电压
             *       maxLimit,               //发射电流
             *       pressJudge,             //环境气压判定
             *       tpmsJudge,              //手柄气压判定
             *       minLimit                //发射电压判定
             *
             * ***************************************/
                if((index == (int)TekstaticElectricTest) && devPara == TekstaticElectric)
                {
                     devTest = lowPassrate;
                     _coefficient = 1000*1000;
                }
                else if((index == (int)TeksendElectricTest) && devPara == TeksendElectric)
                {
                    devTest = maxLimit;
                    _coefficient = 1000;
                }
                else if((index == (int)TekstaticVoltageTest) && devPara == TekstaticVoltage)
                {
                    devTest = minLimit;
                    _coefficient = 1;
                }
                else if((index == (int)TeksendVoltageTest) && devPara == TeksendVoltage)
                {
                    devTest = minLimit;
                    _coefficient = 1;
                }
                else
                {
                    _coefficient = 1;
                    bRet = false;
                }
                bRet = true;
                _log.LOG_DEBUG("ThreadTektronix 【%s】当前档位【%s】,量程【%s】测试项编号【%d】返回值【%s】",_di.Name.data(),m_currentgear.data(),m_currentrange.data(),index,bRet?"true":"false");
            }
            else if((testItemNum == 2) && (parameterNum == 3))
            {//D线静态电流，动态电流在一个工位测试
                devPara = TekstaticElectric;
                m_currentrange = getDevParaStr();
                if(m_currentrange.empty())
                {
                    bRet = false;
                    m_db.Write_TagMValue(_di.devParaAlarm,"1");
                }
                else
                {
                    devTest = lowPassrate;
                    _coefficient = 1000*1000;
                }
                _log.LOG_DEBUG("ThreadTektronix 【%s】静态电流量程【%s】发射电流量程【待查】",_di.Name.data(),m_currentrange.data());
                bRet = true;
            }
            else
            {
                _log.LOG_DEBUG("ThreadTektronix 【%s】 设备测试项参数或测试项与对应加工项参数不匹配缺失",_di.Name.data());
                bRet = false;
            }
        }

        if(bRet)
        {
            myTcpDevice->InitPara(m_currentgear,m_currentrange);
            bRet = myTcpDevice->InitDevice();
        }
        return bRet;
    }
    else
    {
        _log.LOG_DEBUG("ThreadTektronix 【%s】 设备类型不正确",_di.Name.data());
        return false;
    }
}

void ThreadTekDMM6500::threadprocess()
{
    _log.LOG_INFO("【%s】线程驱动已启动.....",_di.Name.data());
    connectRetryCount = 0;
    //得到设备编号
    getDeviceNum();
    //初始化结构体参数
    pi.init();
    //初始化设备
    Init();

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

                if (!code.compare("SC"))
                {
                    if(!kvpt.MemortValue.compare("1"))
                    {
                        clearData(kvpt.TagName);
                        _log.LOG_DEBUG("ThreadTektronix 【%s】 检测到电压或电流开始采集信号!",_di.Name.data());
                        if(m_bInitResult)
                        {
#ifdef _TruckLine
                            CollectMeasData();                        //批量读取电流值
                            string partNoId = m_db.Read_TagMValue(_di.IdFlag);
                            string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                            _log.LOG_DEBUG("ThreadTektronix 【%s】设备 序列号为【%s】 产品ID为【%s】",_di.Name.data(),partSeqNo.data(),partNoId.data());
                            int index = processTruckLine(partNoId, partSeqNo);
                            _log.LOG_DEBUG("ThreadTektronix 【%s】特征值为【%s】",_di.Name.data(),pi.testItemEigenValue.data());
                            saveToSql(partNoId, partSeqNo,index);
                            sendValueToTcpServer(pi.testItemEigenValue);
#else
                            _workTimes++;
                            CollectCurrent(_workTimes);                        //批量读取电流值
                            string partNoId = m_db.Read_TagMValue(_di.IdFlag);
                            string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                            _log.LOG_DEBUG("ThreadTektronix 【%s】设备 序列号为【%s】 产品ID为【%s】",_di.Name.data(),partSeqNo.data(),partNoId.data());
                            int index = processD3Line(partNoId, partSeqNo);
                            _log.LOG_DEBUG("ThreadTektronix 【%s】特征值为【%s】",_di.Name.data(),pi.testItemEigenValue.data());
                            saveToSql(partNoId, partSeqNo,index);
                            sendValueToTcpServer(pi.testItemEigenValue);
#endif
                        }
                        else
                        {
                            m_db.Write_TagMValue(_di.iValue, "初始化【失败】，不测试");
                            _log.LOG_DEBUG("ThreadTektronix 【%s】 初始化失败，不测试!",_di.Name.data());
                        }
                    }
                }
                else if (!code.compare("EC"))
                {
                    if(!kvpt.MemortValue.compare("1"))
                    {
                        usleep(500*1000);
                        m_db.Write_TagMValue(kvpt.TagName,"0");
                    }
                }
                //处理通信状态
                else if(!code.compare("CS"))
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
        _counter++;
    }
    _log.LOG_INFO("ThreadTektronix 【%s】 线程驱动已停止....",_di.Name.data());
}

int ThreadTekDMM6500::getTestItemInfo()
{
    int num=0;
    for(int i = 0;i < _di.testItemCode.size();i++)
    {
        if(devTest == atoi(_di.testItemCode.at(i).data()))
            num = i;
    }
    return num;
}


int ThreadTekDMM6500::getTestItemCode(int index)
{
    for(int i=0; i<_di.testItemCode.size();i++)
    {
        if(i == index)
            return atoi(_di.testItemCode.at(i).data());
    }
    return 0;
}

void *ThreadTekDMM6500::Start_Thread(void* arg)
{
    ThreadTekDMM6500 *th = (ThreadTekDMM6500*)arg;
    th->threadprocess();
    return NULL;
}

bool ThreadTekDMM6500::Start()
{
    //将线程设置为detach模式，线程一关闭则自动释放线程资源
    pthread_attr_init(&pthattr);
    pthread_attr_setdetachstate(&pthattr,PTHREAD_CREATE_DETACHED);

    int ret = pthread_create(&pth, &pthattr, ThreadTekDMM6500::Start_Thread, this);
    if(ret != 0)
    {
        _log.LOG_ERROR("ThreadTektronix 【%s】 驱动线程创建失败!",_di.Name.data());
        return false;
    }
    return true;
}

bool ThreadTekDMM6500::Stop()
{
    _stopprocess = true;
    _stopColl = true;
    pthread_detach(pth);
    pthread_cancel(pth);
    if(_di.type == "1")
    {
        if(myTcpDevice)
        {
            myTcpDevice->Close();
            myTcpDevice->CloseReconThread();
        }
    }
    return true;
}

void ThreadTekDMM6500::CollectDataProcess()
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

    while(!_stopprocess)
    {
         while(!_stopColl)
        {
            string value;

              //读取数据
            if(type == Serial)
                ;
            else if(type == TCP)
                value = myTcpDevice->GetValue();
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

void* ThreadTekDMM6500::CollectData(void *arg)
{
    ThreadTekDMM6500 *th = (ThreadTekDMM6500*)arg;
    th->CollectDataProcess();
    return NULL;
}

void ThreadTekDMM6500::Init()
{
    //初始化设备
    m_bInitResult = InitTekDMM6500Device();
    if(!m_bInitResult)
    {
        //机台报警
        m_db.Write_TagMValue(_di.devInitAlarm, "1");
        //向上位机上传报警信息
        m_db.Write_TagMValue(gLine.Si.alarmStatus,"0");
        m_db.Write_TagMValue(_num + "$" + "NT", _di.Name + " 初始化【失败】!");
        m_db.Write_TagMValue(_di.iValue, "初始化【失败】");
        _log.LOG_ERROR("ThreadTektronix 【%s】 InitAgilentDevice 初始化【失败】",_di.Name.data());
    }
    else
    {
        _log.LOG_DEBUG("ThreadTektronix 【%s】 InitAgilentDevice 初始化【成功】",_di.Name.data());
        m_db.Write_TagMValue(_di.iValue, "初始化【成功】");
    }
}

string ThreadTekDMM6500::getDevParaStr()
{
    string data = "";
    for(int i = 0;i < _di.parameterNo.size();i++)
    {
        int type = atoi((const char*)_di.parameterNo.at(i).data());
        if(devPara == TekcurrGear && type == (int)devPara)
        {//获取当前档位参数
            data = _di.parameter.at(i);
            break;
        }else
        {//获取当前其中一个量程参数
            if((int)devPara == TekNone && type != (int)TekcurrGear){
                devPara = (TekDMM6500ParameterItem)type;
                data = _di.parameter.at(i);
                break;
            }else
            {//获取当前指定的一个量程参数
                if((int)devPara == (TekDMM6500ParameterItem)type){
                    data = _di.parameter.at(i);
                    break;
                }
                else
                    data = "";
            }
        }
    }
    if(data.empty())
        _log.LOG_ERROR("ThreadTektronix 【%s】 获取加工参数失败",_di.Name.data());
    else
        _log.LOG_DEBUG("ThreadTektronix 【%s】 获取加工参数 %s ",_di.Name.data(),data.data());
    return data;
}

void ThreadTekDMM6500::clearData(string tagName)
{
    _cValue.clear();
    _iValue.clear();
    pi.Clear();
    m_db.Write_TagMValue(tagName,"0");
    m_db.Write_TagMValue(_di.JudgeResult, "0");
}

void ThreadTekDMM6500::CommunicateTest()
{
    if(_di.type == "1")
        _connectstatus = myTcpDevice->CanAcess()? "1": "0";
    else
        _connectstatus = "0";
   m_db.Write_TagMValue(_di.Alarm, _connectstatus);

    if(_connectstatus == "0")
    {
        _log.LOG_ERROR("ThreadTektronix 【%s】 通信检测【失败】",_di.Name.data());
        m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
        connectRetryCount ++;
        if(connectRetryCount >= 5){
            connectRetryCount = 0;
            /*Use the dead socket termination port to manually disconnect a dead session on any open socket. All
            existing ethernet connections will be terminated and closed when the connection to the dead socket
            termination port is closed.*/
            //创建一个用来通讯的socket
            int sock = socket(AF_INET,SOCK_STREAM, 0);
            if(sock < 0)
            {
                _log.LOG_ERROR("ThreadTektronix 【%s】 创建DST SOCKET 失败",_di.Name.data());
            }

            //需要connect的是对端的地址，因此这里定义服务器端的地址结构体
            struct sockaddr_in server;
            server.sin_family = AF_INET;
            server.sin_port = htons(5030);
            server.sin_addr.s_addr = inet_addr(_di.Ip.data());
            socklen_t len = sizeof(struct sockaddr_in);
            if(connect(sock, (struct sockaddr*)&server, len) < 0 )
            {
                _log.LOG_ERROR("ThreadTektronix 【%s】 连接DST 5030失败",_di.Name.data());
            }

            usleep(500*1000);
            close(sock);
            usleep(500*1000);
            Init();
        }
    }
    else
    {
        m_db.Write_TagMValue(_num + "$" + "NT","");
        connectRetryCount = 0;
    }
}

void ThreadTekDMM6500::CollectCurrent(int workTimes)
{
    //设置加工项参数，测试项参数
    setTestPara(workTimes);
    //获取加工项参数内容
    string para = getDevParaStr();
    if(_di.type == "1")
    {
        myTcpDevice->SendCommand(para+"\n");
        _cValue = myTcpDevice->GetValueByBatch(_coefficient);
    }
}

void ThreadTekDMM6500::CollectMeasData()
{
    if(_di.type == "1")
    {
        _cValue = myTcpDevice->GetValueByBatch(_coefficient);
    }
}

int ThreadTekDMM6500::processD3Line(string partNoId, string partSeqNo)
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
            _log.LOG_ERROR("ThreadTektronix 【%s】 获取【测试项判定参数】失败",_di.Name.data());
            m_db.Write_TagMValue(_di.iValue, "缺少【测试项判定参数】");
        }

        noJudge();
    }
    else
    {
        _log.LOG_DEBUG("ThreadTektronix 【%s】获取序列号为【空】，不参与测试，直接判定为【不良品】",_di.Name.data());
        m_db.Write_TagMValue(_di.JudgeResult,"0");          //不良品
        m_db.Write_TagMValue(_di.iValue, "获取序列号为【空】，不参与测试，直接判定为【不良品】");
    }

    startJudge();

    if(_workTimes > 2)
        _workTimes = 0;

    if(pi.testItemEigenValue.empty())
        pi.testItemEigenValue = "NG";

    m_db.Write_TagMValue(_di.iValue,"电流表采集值为：【 " + pi.testItemEigenValue + "】");
    return index;
}

void ThreadTekDMM6500::startJudge()
{
    if(m_JR.size() == 2)
    {
        for(int i=0;i<m_JR.size();i++)
        {
            if(m_JR.at(i) == "0")
            {
                _log.LOG_DEBUG("ThreadTektronix 【%s】 判定为【不良品】",_di.Name.data());
                m_db.Write_TagMValue(_di.JudgeResult, "0");
                break;
            }
            if(i == m_JR.size() - 1)
            {
                _log.LOG_DEBUG("ThreadTektronix 【%s】 判定为【良品】",_di.Name.data());
                m_db.Write_TagMValue(_di.JudgeResult, "1");
            }
        }
    }
}

void ThreadTekDMM6500::getJRVaule()
{
    if(pi.testItemJudgeResult == 1)
    {
        _log.LOG_DEBUG("ThreadTektronix 【%s】 JR == 【1】",_di.Name.data());
        m_JR.push_back("1");
    }
    else
    {
        _log.LOG_DEBUG("ThreadTektronix 【%s】 JR == 【0】",_di.Name.data());
        m_JR.push_back("0");
    }
}

int ThreadTekDMM6500::processTruckLine(string partNoId, string partSeqNo)
{
    int index=0;
    if(!partSeqNo.empty() && !partNoId.empty())
    {
        if(_di.testItemCode.size() > 0)
        {
            //获得测试项条目的下标
            index = getTestItemCode(0);
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
                case TekstaticElectricTest:
                    devTest = lowPassrate;
                break;
                case TeksendElectricTest:
                    devTest = maxLimit;
                break;
                case TeksendVoltageTest: case TekstaticVoltageTest:
                    devTest = minLimit;
                break;
                default:
                break;
            }
            //处理采集值,进行判定
            ProcessCValue(pi, 0, devTest, _cValue);
            pi.testItemCode = _di.testItemCode.at(0);
        }
        else
        {
            m_db.Write_TagMValue(_di.testParaAlarm, "1");
            _log.LOG_ERROR("ThreadTektronix 【%s】 获取【测试项判定参数】失败",_di.Name.data());
            m_db.Write_TagMValue(_di.iValue, "缺少【测试项判定参数】");
        }
        noJudge();
    }
    else
    {
        _log.LOG_DEBUG("ThreadTektronix 【%s】获取序列号为【空】，不参与测试，直接判定为【不良品】",_di.Name.data());
        m_db.Write_TagMValue(_di.JudgeResult,"0");          //不良品
        string svalue;
        svalue += partNoId.empty()?("产品ID为空，"):("");
        svalue += partSeqNo.empty()?("序列号为空，"):("");
        svalue += "直接判定为【不良品】";
        m_db.Write_TagMValue(_di.iValue,svalue);
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

void ThreadTekDMM6500::setTestPara(int workTimes)
{
    if(workTimes % 2 == 0)
    {
        devPara = TeksendElectric;
        devTest = maxLimit;
        _coefficient = 1000;
        _log.LOG_DEBUG("ThreadTektronix 【%s】 切换为【mA】档位 测试发射电流 .....",_di.Name.data());
        m_db.Write_TagMValue(_di.iValue, "发射电流测试中.....");
    }
    else
    {
        devPara = TekstaticElectric;
        devTest = lowPassrate;
        _coefficient = 1000*1000;
        _log.LOG_DEBUG("ThreadTektronix 【%s】 切换为【uA】档位 测试静态电流 .....",_di.Name.data());
        m_db.Write_TagMValue(_di.iValue, "静态电流测试中.....");
    }
}

void ThreadTekDMM6500::processEndFlag()
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

void ThreadTekDMM6500::sendValueToTcpServer(string cValue)
{
    //向上位机传值
#ifdef _TruckLine
    int pos=0;
    pos = atoi(_di.testStartPos.data());
    string sCollectValue = _num + "$" + "CV" + IntToString(pos);
    m_db.Write_TagMValue(sCollectValue, cValue);
#else
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
#endif
}
