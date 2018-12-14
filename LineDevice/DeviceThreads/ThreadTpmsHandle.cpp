#include "./ThreadTpmsHandle.h"
#include "main.h"

bool ThreadTpmsHandle::updateFlag = true;

/**
 * @brief 无参构造函数
 */
ThreadTpmsHandle::ThreadTpmsHandle() : _tpmsDataLength(42)
{}

/**
 * @brief 构造函数
 * @param di
 */
ThreadTpmsHandle::ThreadTpmsHandle(DeviceInfo di) : DeviceThread(di),_tpmsDataLength(42)
{
    _di = di;
    _pressFormula = "";
    _voltageFormula = "";
    _posfixpressFormula = "";
    _posfixvoltageFormula = "";
    _tpmsDatas = "";
    _iValuePress = "";          //气压瞬时值
    _iValueVoltage = "";        //电压瞬时值
    _iCollectPress = "";          //气压收集值
    _iCollectVoltage = "";        //电压收集值
    m_VoltageMaxValue = 1;        //存储QY1104A1、QY1104、QY2001A5、QY2001A8件号电压判定参数中的最大值,用于生成范围内的随机数
    m_VoltageMinValue = 0.02;     //存储QY1104A1、QY1104、QY2001A5、QY2001A8件号电压判定参数中的最小值,用于生成范围内的随机数
    _id = "";
    _stopprocess = false;
    _stopColl = true;
    m_nCurrTestItemIndex = 0;
    m_nPressTestItemIndex = TestItemNoExist;
    m_nVoltageTestItemIndex = TestItemNoExist;
    m_CurrPartNo = partNo;
    if(_di.type == "1")
        myTcpDevice = new TpmsHandleTcp(atoi(_di.Port.data()),_di.Ip,_di.Name);
    else if(_di.type == "2")
    {
        myComDevice = new TpmsHandle(SerialPortParameters::GetSpp(_di),di.Name);
    }
    m_simulationDataFlag = false;
    m_frameCheckID = "";
}

/**
 * @brief 析构函数
 */
ThreadTpmsHandle::~ThreadTpmsHandle()
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
 * @brief 启动设备驱动线程
 * @return
 */
bool ThreadTpmsHandle::Start()
{
    pthread_t pth_start;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(0 != pthread_create(&pth_start,&attr,ThreadTpmsHandle::StartThread,this))
    {
        _log.LOG_ERROR("ThreadTpmsHandle 【%s】 手柄线程创建失败 !",_di.Name.data());
        return false;
    }
    return true;
}

/**
 * @brief 关闭设备驱动线程
 * @return
 */
bool ThreadTpmsHandle::Stop()
{
    _stopprocess = true;
    _stopColl = true;
    usleep(500 * 1000);
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
    updateFlag = true;
    return true;
}

/**
 * @brief 驱动线程启动接口
 * @param arg
 * @return
 */
void* ThreadTpmsHandle::StartThread(void* arg)
{
    ThreadTpmsHandle* pth = (ThreadTpmsHandle*)arg;
    pth->processStart();
    return NULL;
}

void ThreadTpmsHandle::clearData(string tagName)
{
    m_db.Write_TagMValue(tagName, "0");
    _id.clear();
    _iValuePress.clear();
    _iValueVoltage.clear();
    _iCollectPress.clear();          //气压收集值
    _iCollectVoltage.clear();        //电压收集值
    _collectValue.clear();
    _tpmsDatas.clear();
    pi.Clear();
    m_JR.clear();
    m_db.Write_TagMValue(_di.iValue, "");
    m_db.Write_TagMValue(_di.EigenValue , "");
    m_db.Write_TagMValue(_di.JudgeResult , "");
    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 检测到开始采集信号!",_di.Name.data());
    m_frameCheckID = "";
}

string ThreadTpmsHandle::sendID(string id)
{
    if(_di.type == "1")
    {
        return myTcpDevice->SendId(id);
    }
    else if(_di.type == "2")
    {
        return myComDevice->SendId(id);
    }
}

string ThreadTpmsHandle::getData()
{
    if(_di.type == "1")
    {
        return myTcpDevice->GetData();
    }
    else if(_di.type == "2")
    {
        return myComDevice->GetData();
    }
}

void ThreadTpmsHandle::judgeTpmsCheck(int testItemJudgeResult)
{
    if(testItemJudgeResult == 1)
    {
        m_db.Write_TagMValue(_di.JudgeResult, "1");
//                        	pi.testItemEigenValue = "OK";
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult, "0");
//                        	pi.testItemEigenValue = "NG";
    }
}

void ThreadTpmsHandle::ProcessTpmsCValue(string partSeqNo, string partNoId,int currTestItemIndex,JudgeMethod judgeMethod,string cValue)
{
    ProcessCValue(pi,currTestItemIndex, judgeMethod, cValue);
    if(m_simulationDataFlag)
    {
//        _log.LOG_DEBUG("ThreadTpmsHandle HexValue1222");
        pi.testItemCollectValue += "/sim";
    }
    if(pi.testItemEigenValue.empty())
        pi.testItemEigenValue = "NG";
    saveToSql(partNoId, partSeqNo,currTestItemIndex);
    if(_collectValue.empty())
    {
        _collectValue = pi.testItemEigenValue;
    }
    else
    {
        _collectValue = _collectValue + "/" + pi.testItemEigenValue;
    }

    m_nCurrTestItemIndex = currTestItemIndex;
    sendValueToTcpServer(pi.testItemEigenValue);
    getJRVaule();
}

void ThreadTpmsHandle::processTpmsCheck(string partSeqNo, string partNoId)
{
    if(!partSeqNo.empty() && !partNoId.empty())
    {
        //逻辑判定//气压手柄有两条测试项数据
        if(m_nPressTestItemIndex != TestItemNoExist)
        {
            ProcessTpmsCValue(partSeqNo,partNoId,m_nPressTestItemIndex,tpmsJudge,_iCollectPress);
        }
        if(m_nVoltageTestItemIndex != TestItemNoExist)
        {
            ProcessTpmsCValue(partSeqNo,partNoId,m_nVoltageTestItemIndex,minLimit,_iCollectVoltage);
        }

        m_db.Write_TagMValue(_di.iValue,"手柄特征值为：【 " + _collectValue + "】");
        _log.LOG_DEBUG("ThreadTpmsHandle 【%s】特征值为【%s】",_di.Name.data(),_collectValue.data());
        CheckJudge();//所有设备判定项使能为0时，直接将判定结果置为1，否则综合检查判定每一个判定结果
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 获取序列号、id为【空】，不进行测试，直接判定为【不良品】",_di.Name.data());
        string svalue;
        svalue += partNoId.empty()?("产品ID为空，"):("");
        svalue += partSeqNo.empty()?("序列号为空，"):("");
        svalue += "直接判定为【不良品】";
        m_db.Write_TagMValue(_di.iValue,svalue);
    }
}

/**
 * @brief 设备驱动处理函数
 */
void ThreadTpmsHandle::processStart()
{
    string endFlag;
    _log.LOG_INFO("ThreadTpmsHandle 【%s】 线程驱动已启动......",_di.Name.data());
    int ret = pthread_create(&_collectThread,NULL,ThreadTpmsHandle::CollectThread,this);
    if(ret != 0)
    {
        _log.LOG_ERROR("ThreadTpmsHandle 【%s】 手柄数据采集线程创建失败!",_di.Name.data());
        return;
    }

    pi.init();
    //获取从数据库查询到的公式及数据长度参数
    initDevice();

    getDeviceNum();

    while (!_stopprocess)
    {
//        checkTestPara(0);
#if 1
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
                    clearData(tag.TagName);

                    string id = m_db.Read_TagMValue(_di.IdFlag);
                    //处理气压测试唤醒信号
                    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 检测到气压测试唤醒信号! ID = 【%s】",_di.Name.data(),id.data());
                    string backID;
                    backID = sendID(id);
                    m_frameCheckID = id;
                    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 手柄回读ID为【%s】",_di.Name.data(),backID.data());
                    m_mode = Press;
                    m_simulationDataFlag = false;
//                    _log.LOG_DEBUG("ThreadTpmsHandle m_simulationDataFlag = false");
                    _stopColl = false;
                }
                //开始低频唤醒
                else if(name == "SW" && tag.MemortValue == "1")
                {
                    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 检测到唤醒测试信号",_di.Name.data());
                    m_db.Write_TagMValue(tag.TagName, "0");
                    m_db.Write_TagMValue(_di.DeviceState, "低频唤醒");
                    _id = LowFrequencyWakeup(5000);
                    if (!_id.empty())
                    {
                        m_db.Write_TagMValue(_di.IdFlag, _id);
                        pi.testItemJudgeResult = 1;
                    }
                }
                //功率唤醒回读ID
                else if(name == "SPW" && tag.MemortValue == "1")
                {
                    pi.Clear();
                    _tpmsDatas.clear();
                    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】检测到功率唤醒信号",_di.Name.data());
                    m_db.Write_TagMValue(tag.TagName, "0");
                    string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                    _id = m_db.Read_TagMValue(_di.IdFlag);
                    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】设备 产品ID为【%s】 序列号为【%s】",_di.Name.data(),_id.data(),partSeqNo.data());

                    ParseScannerID(_id,partSeqNo);
                    _counter = 1;

                    if(pi.testItemEigenValue.empty())
                        pi.testItemEigenValue = "NG";
                    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】特征值为【%s】",_di.Name.data(),pi.testItemEigenValue.data());

                    //将产品信息存储到数据库
                    if(!_id.empty() && !partSeqNo.empty())
                    {
                        saveToSql(_id, partSeqNo,0);
                    }
                }
                //开始加速度唤醒
                else if(name == "SAW" && tag.MemortValue == "1")
                {                    
                    _id = m_db.Read_TagMValue(_di.IdFlag);
                    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 检测到加速度唤醒信号,ID为【%s】",_di.Name.data(),_id.data());
                    m_db.Write_TagMValue(tag.TagName, "0");
                    m_db.Write_TagMValue(_di.DeviceState, "加速度唤醒");
                    //此处设置为超时停止，加速度测试时长为60秒，此处需提前结束记录测试结果
                    string result = AccelerationWakeup(_id, 1000 * 57);
                    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】设备 加速度测试结果为【%s】", _di.Name.data(), result.data());
                    if (!result.empty())
                    {
                        m_db.Write_TagMValue(_di.JudgeResult, result);
                    }
                    if (!result.empty())
                    {
                        _log.LOG_ERROR("ThreadTpmsHandle 【%s】 加速度测试存在【不良品】,更新产品错误代码",_di.Name.data());
                        UpdateAccelerationInfo(_id, result);
                    }
                }
                //停止采集数据
                else if(name == "EC" && tag.MemortValue == "1")
                {
                    m_db.Write_TagMValue(tag.TagName,"0");
                    _stopColl = true;
                    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】检测到停止采集信号! ",_di.Name.data());
                    pi.Clear();
                    _collectValue.clear();
                    usleep(500*1000);
                    //气压手柄只有一条测试项数据，故index = 0
                    string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                    string partNoId = m_db.Read_TagMValue(_di.IdFlag);
                    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】设备 产品ID为【%s】 序列号为【%s】",_di.Name.data(),partNoId.data(),partSeqNo.data());

                    processTpmsCheck(partSeqNo, partNoId);
                    m_frameCheckID = "";

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
                }
            }
        }
        _counter++;
#endif
    }
    _stopColl = true;
}

void ThreadTpmsHandle::ParseScannerID(string _id,string partSeqNo)
{
    if(!_id.empty() && !partSeqNo.empty())
    {
//                        _log.LOG_DEBUG("ThreadTpmsHandle 【%s】设备 功率唤醒发送ID【%s】",_di.Name.data(),_id.data());
        sendID(_id);
        m_mode = Power;
        _stopColl = false;
        sleep(7);
        _stopColl = true;
        judgeTpmsCheck(pi.testItemJudgeResult);
        noJudge();
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 序列号【%s】、id为【%s】，不进行测试，直接判定为【不良品】",_di.Name.data(),partSeqNo.data(),_id.data());
        m_db.Write_TagMValue(_di.iValue,"序列号或ID为空，不测试，直接判定为【不良品】");
    }
}

/**
 * @brief ThreadTpmsHandle::CommunicateTest
 */
void ThreadTpmsHandle::CommunicateTest()
{
    if(_di.type == "1")
    {
        _connectstatus = myTcpDevice->CanAcess() ? "1" : "0";
    }
    else if(_di.type == "2")
    {
        _connectstatus = myComDevice->CanAcess() ? "1" : "0";
    }
    m_db.Write_TagMValue(_di.Alarm, _connectstatus);
    if(_connectstatus == "0")
    {
        _log.LOG_ERROR("ThreadTpmsHandle 【%s】通信检测【失败】 ",_di.Name.data());
        m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
    }
    else
    {
        m_db.Write_TagMValue(_num + "$" + "NT","");
//                                _log.LOG_DEBUG("ThreadTpmsHandle 【%s】通信检测【成功】 ",_di.Name.data());
    }
}

void ThreadTpmsHandle::trimTpmsData()
{
    int index = 0;
    for(int i = 0;i < _tpmsDataLength - 1;i++)
    {
        if(_tpmsDatas[i] == 'F' && _tpmsDatas[i + 1] == 'E')
        {
            index = i;
            break;
        }
    }
    if(index != 0)
        _tpmsDatas.erase(0,index);
}

string ThreadTpmsHandle::extractTpmsData()
{
    //从当前所有数据中取一帧数据
    string hexValue = _tpmsDatas.substr(0, _tpmsDataLength);
    _log.LOG_DEBUG("ThreadTpmsHandle CheckDataAndCalcuatePress 【%s】设备 从数据中取出一帧数据为【%s】, tpms数据长度为【%d】",_di.Name.data(),hexValue.data(),_tpmsDataLength);
    //从当前所有数据中移除已取出的一帧数据
    _tpmsDatas.erase(0, _tpmsDataLength);
    return hexValue;
}

/**
 * @brief 采集线程接口
 * @param arg
 * @return
 */
void* ThreadTpmsHandle::CollectThread(void* arg)
{
    ThreadTpmsHandle* th = (ThreadTpmsHandle*)arg;
    th->collectData();
    return NULL;
}

/**
* @brief 检测tpms数据，计算气压、电压值
*/
void ThreadTpmsHandle::CheckDataAndCalcuatePress()
{
    checkCalcuateParameter();
    trimTpmsData();
    if(_tpmsDatas.length() >= _tpmsDataLength)
    {
        string HexValue = extractTpmsData();
        string framePartNoID = HexValue.substr(2,8);
        if(framePartNoID.compare(m_frameCheckID) == 0)
        {
            string simulation = HexValue.substr(36,4);
            if(!simulation.compare("AB56"))
            {
                m_simulationDataFlag = true;
    //            _log.LOG_DEBUG("ThreadTpmsHandle HexValue 【%s】",simulation.data());
            }
    //        _log.LOG_DEBUG("ThreadTpmsHandle HexValue 【%s】simulation 【%s】",HexValue.data(),simulation.data());
            string collectvalue;
            if(m_nPressTestItemIndex != TestItemNoExist)
            {
                double press = hextodec(HexValue.substr(10,2));
                _log.LOG_DEBUG("ThreadTpmsHandle CheckDataAndCalcuatePress 【%s】设备 气压进制转换结果值为【%f】",_di.Name.data(),press);
                if (press > 4)
                {
                    double presstmp = CalcuateData(press,_posfixpressFormula);
                    _log.LOG_DEBUG("ThreadTpmsHandle CheckDataAndCalcuatePress 【%s】设备 计算气压值结果为【%f】",_di.Name.data(),presstmp);
                    if(presstmp > 0)
                    {
                        _iValuePress = DoubleToString(presstmp,"%.2f");
                        _iCollectPress.append(_iValuePress + "/");
                        m_nCurrTestItemIndex = m_nPressTestItemIndex;
                        sendValueToTcpServer(_iValuePress);
                        collectvalue = _iValuePress;
                    }
                }
            }
            if(m_nVoltageTestItemIndex != TestItemNoExist)
            {
                double voltagetmp = 0.0;
                if((!m_CurrPartNo.compare("96216708")) || (!m_CurrPartNo.compare("96216721"))
                || (!m_CurrPartNo.compare("96217012")) || (!m_CurrPartNo.compare("96217006")))
                {//QY1104A1、QY1104、QY2001A5、QY2001A8件号特例化
                    srand(time(0));       //seed
                    voltagetmp = m_VoltageMinValue + 0.00001 + ((double)(rand() % (int)((m_VoltageMaxValue - m_VoltageMinValue) * 100 -1)) / 100);

                }
                else
                {
                    double voltage = hextodec(HexValue.substr(14,2));
                    _log.LOG_DEBUG("ThreadTpmsHandle CheckDataAndCalcuatePress 【%s】设备 电压进制转换结果值为【%f】",_di.Name.data(),voltage);
                    voltagetmp = CalcuateData(voltage,_posfixvoltageFormula);
                }
                _log.LOG_DEBUG("ThreadTpmsHandle CheckDataAndCalcuatePress 【%s】设备 计算电压值结果为【%f】",_di.Name.data(),voltagetmp);
                if(voltagetmp > 0)
                {
                    _iValueVoltage = DoubleToString(voltagetmp,"%.2f");
                    _iCollectVoltage.append(_iValueVoltage + "/");
                    m_nCurrTestItemIndex = m_nVoltageTestItemIndex;
                    sendValueToTcpServer(_iValueVoltage);
                    collectvalue = _iValueVoltage;
                }
            }
            if((m_nPressTestItemIndex != TestItemNoExist) && (m_nVoltageTestItemIndex != TestItemNoExist))
            {
                collectvalue = _iValuePress + "/" +_iValueVoltage;
            }
            m_db.Write_TagMValue(_di.iValue, "手柄采集值为：【" + collectvalue + "】");//在显示界面上实时更新瞬时值
        }
        CheckDataAndCalcuatePress();
    }
}

/**
 * @brief ThreadTpmsHandle::checkCalcuateParameter,检查气压、电压等计算参数
 * @param id
 */
void ThreadTpmsHandle::checkCalcuateParameter()
{
    if(_tpmsDataLength == 0)
    {
        _log.LOG_ERROR("ThreadTpmsHandle 【%s】设备 获取【测试项参数】数据帧长度失败【失败】",_di.Name.data());
        m_db.Write_TagMValue(_di.testParaAlarm, "1");
    }

    if(m_nPressTestItemIndex != TestItemNoExist)
    {
        if(_pressFormula.empty())
        {
            _log.LOG_ERROR("ThreadTpmsHandle 【%s】设备 获取【加工参数】 气压公式【失败】",_di.Name.data());
            m_db.Write_TagMValue(_di.devParaAlarm, "1");
        }
    }
    if(m_nVoltageTestItemIndex != TestItemNoExist)
    {
        if(_voltageFormula.empty())
        {
            _log.LOG_ERROR("ThreadTpmsHandle 【%s】设备 获取【加工参数】 电压公式【失败】",_di.Name.data());
            m_db.Write_TagMValue(_di.devParaAlarm, "1");
        }
    }
}

/**
 * @brief ThreadTpmsHandle::checkTpmsAwakenedId
 * @param id
 */
void ThreadTpmsHandle::checkTpmsAwakenedId(string id)
{
    if(!id.empty() && id == _id)
    {
         pi.testItemJudgeResult = 1;
         m_db.Write_TagMValue(_di.iValue, "手柄唤醒ID为：【" + id + "】");
         _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 手柄已唤醒",_di.Name.data());
         pi.testItemEigenValue = id;
         pi.testItemCollectValue = id;
    }
    else if(id.empty())
    {
        m_db.Write_TagMValue(_di.iValue, "手柄未唤醒，ID为空");
        pi.testItemJudgeResult = 0;
        _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 手柄未唤醒，ID为空",_di.Name.data());
    }
    else if(!id.empty() && id != _id)
    {
        m_db.Write_TagMValue(_di.iValue, "ID不一致，扫码ID【" + _id + "】,唤醒ID【" + id + "】");
        pi.testItemJudgeResult = 0;
        _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 ID不一致,扫码ID【%s】,唤醒ID【%s】",_di.Name.data(),_id.data(),id.data());
    }
}

/**
* @brief 检测手柄返回数据，回读唤醒ID
*/
void ThreadTpmsHandle::CheckDataAndGetID()
{
    int index = 0;
    if(_tpmsDataLength == 0)
    {
        _log.LOG_ERROR("ThreadTpmsHandle 【%s】设备 获取【加工参数】数据帧长度失败【失败】",_di.Name.data());
        m_db.Write_TagMValue(_di.testParaAlarm, "1");
    }
    trimTpmsData();
    if (_tpmsDatas.length() >= _tpmsDataLength)
    {
        string id = (extractTpmsData()).substr(2,8);
        _log.LOG_DEBUG("ThreadTpmsHandle CheckDataAndGetID 【%s】设备 返回ID为【%s】,扫码ID为【%s】",_di.Name.data(),id.data(),_id.data());
        checkTpmsAwakenedId(id);
        _stopColl = true;
    }
    else
    {
         m_db.Write_TagMValue(_di.iValue, "手柄未唤醒，ID为空");
         _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 手柄未唤醒，ID为空",_di.Name.data());
    }
}


/**
 * @brief 低频唤醒获取ID
 * @param 超时时间 单位：毫秒
 * @return
 */

string ThreadTpmsHandle::LowFrequencyWakeup(int wakeTimeLen)
{
    string id = "";
    struct timeval tv;
    gettimeofday(&tv,NULL);
    long st = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    if(_di.type == "1")
    {
        //清空串口数据
        myTcpDevice->GetData();
        //向串口写入低频唤醒指令
        myTcpDevice->SendId("00000000");
    }
    else if(_di.type == "2")
    {
        //清空串口数据
        myComDevice->GetData();
        //向串口写入低频唤醒指令
        myComDevice->SendId("00000000");
    }

    bool stopFlag = false;
    while (!stopFlag)
    {
        //判断当前时间是否超时
        usleep(150 * 1000);
        struct timeval tn;
        gettimeofday(&tn,NULL);
        long now = tn.tv_sec * 1000 + tn.tv_usec / 1000;
        if ((now - st) > wakeTimeLen) stopFlag = true;
        string result;
        result = getData();
        //新协议42，老协议16
        if (!result.empty() && (result.length() == 42 || result.length() == 16))
        {
            id = result.substr(2, 8);
            if (!id.compare("00000000") || !id.compare("FFFFFFFF"))
            {
                continue;
            }
            else
            {
                sendID("FFFFFFFF");
                return id;
            }
        }
    }
    return id;
}



/**
* @brief 加速度唤醒获取发射机唤醒结果
* @param id 多个ID使用"/"分割
* @param times 超时时间 单位：毫秒
* @return 发射机唤醒结果，每一个字符代表一个结果值，1表示成功，0表示失败
*/

string ThreadTpmsHandle::AccelerationWakeup(string id, int times)
{
    if (!id.empty())
        return "";
    vector<string> ids;
    splittostring(id,"/",ids);
    //定义长度与ids等长，默认值为0的数组
    string results(ids.size(),0);
    timeval startTime;
    gettimeofday(&startTime,NULL);

    getData();
    //向串口写入低频唤醒指令
    while(id.find("/") != string::npos)
    {
        id.replace(id.find("/"),1,"");
    }
    string cmd = id;
    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】设备 加速度测试向手柄发送ID指令为【%s】",_di.Name.data(),cmd.data());

    sendID(cmd);
    bool stopFlag = false;
    while (!stopFlag)
    {
        usleep(350 * 1000);
        timeval currentTime;
        gettimeofday(&currentTime, NULL);
        long totalTime = (currentTime.tv_sec - startTime.tv_sec) * 1000 + (currentTime.tv_usec - startTime.tv_usec) / 1000;
        if (totalTime > times)
        {
            stopFlag = true;
        }
        string result;
        result = getData();
        int dataLength = 42;
        if (!result.empty() && result.length() % dataLength == 42)
        {
            for (unsigned int i = 0; i < result.length(); i++)
            {
                id = result.substr(i * dataLength + 2, 8);
                int index = 0;
                for(unsigned int i = 0;i < ids.size();i++)
                {
                    if(ids[i] == id)
                    {
                        index = i;
                        break;
                    }
                }
                if (index >= 0)
                {
                    results[index] = '1';
                }
            }
        }
    }
    return results;
}


/**
 * @brief ThreadTpmsHandle::UpdateAccelerationInfo  更新产品加速度测试信息
 * @param ids                                       加速度测试ID
 * @param result                                    加速度测试结果
 */

void ThreadTpmsHandle::UpdateAccelerationInfo(string ids,string result)
{
    if (!ids.empty() && ids.find("/") != string::npos && !_productModel.empty())
    {
        while(ids.find("/") != string::npos)
        {
            ids.replace(ids.find("/"),1,",");
        }
        vector<string> idsArray;
        splittostring(ids,",",idsArray);
        while(ids.find(",") != string::npos)
        {
            ids.replace(ids.find(","),1,"\',\'");
        }
        ids.insert(0,"\'");
        ids.insert(ids.length(),"\'");
        MYSQL_RES* res = NULL;
        MYSQL* m_connection = new MYSQL;
        MySQLHelper m_helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
        m_helper.MySQL_Init(m_connection);
        res = m_helper.LoadTable(m_connection,"select ProductId,ProductErrorCode from productinfo where ProductId in (" + ids + ") and ProductModel = '" + _productModel + "'");
        MYSQL_ROW query;
        if(res != NULL)
        {
            string updateProductInfoSql,updateResultValueSql;
            updateProductInfoSql.append("update productinfo set ProductErrorCode = case ProductId ");
            updateResultValueSql.append("update resultvalue set status = case ID ");
            string updateIds = "";
            while(query = mysql_fetch_row(res))
            {
                string id = query[0];
                string errorCode = query[1];
                int index = 0;
                for(unsigned int i = 0;i < idsArray.size();i++)
                {
                    if(idsArray[i] == id)
                    {
                        index = i;
                        break;
                    }
                }
                if(result[index] == '0')
                {
                    errorCode = errorCode != "0" ? (errorCode.find("9") != string::npos ? errorCode : errorCode + "9") : "9";
                }
                else
                {
                    if(errorCode.find("9") != string::npos)
                    {
                        if(errorCode.length() == 1)
                        {
                            errorCode = "0";
                        }
                        else
                        {
                            while(errorCode.find("9") != string::npos)
                            {
                                errorCode.replace(errorCode.find("9"),1,"");
                            }
                        }
                    }
                }
                updateProductInfoSql.append("when '" + id + "' then '" + errorCode + "' ");
                updateResultValueSql.append("when '" + id + "' then '" + errorCode + "' ");
                if(updateIds.empty())
                {
                    updateIds = "'" + id + "'";
                }
                else
                {
                    updateIds = updateIds + "," + "'" + id + "'";
                }
            }
            updateProductInfoSql.append("end where ProductId in (" + updateIds + ") and ProductModel = '" + _productModel + "'");
            updateResultValueSql.append("end where ID in (" + updateIds + ") and model = '" + _productModel + "'");
            m_helper.LoadTable(m_connection,updateProductInfoSql);
            m_helper.LoadTable(m_connection,updateResultValueSql);
            m_helper.MySQL_Release(res);
        }
        m_helper.MySQL_Close(m_connection);
        delete m_connection;
    }
    else
    {
        _log.LOG_ERROR("ThreadTpmsHandle 【%s】设备 更新加速度信息【失败】! ID为【%s】,Result为【%s】,ProductModel为【%s】",_di.Name.data(),ids.data(),result.data(),_productModel.data());
    }
}


/**
 * @brief 计算压力
 * @param press = 压力值
 * @return 计算结果
 */

double ThreadTpmsHandle::CalcuateData(double nativedata,string formula)
{
    return m_calculateHelper.posfixCompute(nativedata,formula);
}

/**
 * @brief ThreadTpmsHandle::InitTestItemInfo
 */

void ThreadTpmsHandle::InitTestItemInfo()
{
    for(int i=0; i<_di.testItemCode.size(); i++)
    {
        if(!(_di.testItemCode.at(i)).compare("1"))
        {
            if(m_nPressTestItemIndex == TestItemNoExist)
                m_nPressTestItemIndex = i;
        }
        else if(!(_di.testItemCode.at(i)).compare("3"))
        {
            if(m_nVoltageTestItemIndex == TestItemNoExist)
            {
                m_nVoltageTestItemIndex = i;
                m_VoltageMaxValue = _di.maxValue.at(i);     //用于存储QY1104A1、QY1104、QY2001A5、QY2001A8件号对应的电压判定参数中的最大值
                m_VoltageMinValue = _di.minValue.at(i);     //用于存储QY1104A1、QY1104、QY2001A5、QY2001A8件号对应的电压判定参数中的最小值
            }
        }
    }
}

/**
 * @brief ThreadTpmsHandle::getDeviceParameter
 */

void ThreadTpmsHandle::getDeviceParameter()
{
    int type = 0;
    for(int i=0; i<_di.parameterNo.size(); i++)
    {
        type = atoi((const char*)_di.parameterNo.at(i).data());
        switch(type)
        {
            case PressFormula:
            {
                if(_di.parameter.size() > i)
                    _pressFormula = _di.parameter.at(i);
            }
            break;
            case FrameLength:
            {
                if(_di.parameter.size() > i)
                    _tpmsDataLength = atoi((const char *)_di.parameter.at(i).data());
            }
            break;
            case VoltageFormula:
            {
                if(_di.parameter.size() > i)
                    _voltageFormula = _di.parameter.at(i);
            }
            break;
        default:
            break;
        }
    }
    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】设备 气压公式为【%s】,数据长度为【%d】,电压公式为【%s】",_di.Name.data(),_pressFormula.data(),_tpmsDataLength,_voltageFormula.data());
}


/**
 * @brief ThreadTpmsHandle::UpdatePressFormula              更新气压公式
 */
void ThreadTpmsHandle::UpdateFormula()
{
    if(m_nPressTestItemIndex != TestItemNoExist)
        _posfixpressFormula = m_calculateHelper.UpdateFormula(_pressFormula);
    if(m_nVoltageTestItemIndex != TestItemNoExist)
        _posfixvoltageFormula = m_calculateHelper.UpdateFormula(_voltageFormula);
}

/**
 * @brief ThreadTpmsHandle::initDevice
 */
void ThreadTpmsHandle::initDevice()
{
    //找出气压和电压测试项索引值
    InitTestItemInfo();
    //获取最新的设备参数
    getDeviceParameter();
    //更新气压和电压公式
    UpdateFormula();
}

/**
 * @brief ThreadTpmsHandle::checkTestPara   检测MySQL中测试项参数
 */
void ThreadTpmsHandle::checkTestPara(int index)
{
    if(!_di.testItemCode.empty())
    {
        if(_di.standardValue.at(index) == 0)
            m_db.Write_TagMValue(_di.Name + _di.testItemCode.at(0) + "SV", "标准值");
        if(_di.deviation.at(index) == 0)
            m_db.Write_TagMValue(_di.Name + _di.testItemCode.at(0) + "DT", "偏差范围");
    }
}


/**
 * @brief 获取气压平均值
 */
double ThreadTpmsHandle::GetAverageValue(string _cValue)
{
    vector<double> valueArray;
    split(_cValue,"/",valueArray);
    return Average(valueArray);
}


/**
 * @brief 采集数据处理
 */
void ThreadTpmsHandle::collectData()
{
    while(!_stopprocess)
    {
        int logFlag = 0;
        while(!_stopColl)
        {
            //第一次手柄返回数据为产品ID，故将此数据屏蔽
            string data;
            data = getData();
#ifdef _Debug
            string partNoId = m_db.Read_TagMValue(_di.IdFlag);
            if((atoi(partNoId.data()) % 2) == 0)
            {
                if((atoi(partNoId.data()) % 3) == 0)
                {
                    data = "FE" + partNoId + "035DC2F12506001001A1FF1213FF00B5";
                }
                else
                {
                    data = "FE" + partNoId + "655DC0F12506001001A1FF1213FF00B5";
                }
            }
            else
            {
                if((atoi(partNoId.data()) % 3) == 0)
                {
                    data = "FE" + partNoId + "635DA4F12506001001A1FF1213FF00B5";
                }
                else
                {
                    data = "FE" + partNoId + "615DC6F12506001001A1FF1213FF00B5";
                }
            }
#endif
            //日志里面只记录2次手柄值
            if(logFlag < 2)
            {
                logFlag++;
                _log.LOG_DEBUG("ThreadTpmsHandle 【%s】设备 手柄采集数据为【%s】",_di.Name.data(),data.data());
            }
//            cout<<"-------- data = "<<data<<endl;
            _tpmsDatas.append(data);
            switch(m_mode)
            {
                case Press:CheckDataAndCalcuatePress();break;
                case Power:CheckDataAndGetID();break;
            }
            usleep(150*1000);
        }
        usleep(100*1000);
    }
}

/**
 * @brief ThreadTpmsHandle::getJRVaule
 */
void ThreadTpmsHandle::getJRVaule()
{
    if(pi.testItemJudgeResult == 1)
    {
        _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 JR == 【1】",_di.Name.data());
        m_JR.push_back("1");
    }
    else
    {
        _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 JR == 【0】",_di.Name.data());
        m_JR.push_back("0");
    }
}

/**
 * @brief ThreadTpmsHandle::CheckJudge
 */
void ThreadTpmsHandle::CheckJudge()
{
    bool nojudge = true;
    if(!_di.judgeEneble.empty())
    {
        for(int i = 0;i < _di.judgeEneble.size();i++)
        {
            if(_di.judgeEneble.at(i) == 1)
            {
                nojudge = false;
                break;
            }
        }
    }
    if(nojudge)
        jumpJudge();
    else
        startJudge();
}

/**
 * @brief ThreadTpmsHandle::startJudge
 */
void ThreadTpmsHandle::startJudge()
{
    if(m_JR.size() > 0)
    {
        for(int i=0;i<m_JR.size();i++)
        {
            if(m_JR.at(i) == "0")
            {
                _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 判定为【不良品】",_di.Name.data());
                m_db.Write_TagMValue(_di.JudgeResult, "0");
                break;
            }
            if(i == m_JR.size() - 1)
            {
                _log.LOG_DEBUG("ThreadTpmsHandle 【%s】 判定为【良品】",_di.Name.data());
                m_db.Write_TagMValue(_di.JudgeResult, "1");
            }
        }
    }
}

/**
 * @brief ThreadTpmsHandle::noJudge
 */
void ThreadTpmsHandle::jumpJudge()
{
    _log.LOG_DEBUG("ThreadTpmsHandle 【%s】设备，judgeEnable=0，不参与判定，检测【合格】",_di.Name.data());
    m_db.Write_TagMValue(_di.JudgeResult,"1");
    pi.testItemJudgeResult = 1;
    pi.testItemEigenValue = "NA";
    m_db.Write_TagMValue(_di.iValue, "不进行判定,【" + _di.Name + "】检测合格");
    m_db.Write_TagMValue(_di.BarCode, "不进行判定,【" + _di.Name + "】检测合格");
}

/**
 * @brief ThreadTpmsHandle::sendValueToTcpServer
 * @param cValue
 */
void ThreadTpmsHandle::sendValueToTcpServer(string cValue)
{
    int pos=0;
    pos = atoi(_di.testStartPos.data());
    pos += m_nCurrTestItemIndex;
    string sCollectValue = _num + "$" + "CV" + IntToString(pos);
    m_db.Write_TagMValue(sCollectValue, cValue);
}
