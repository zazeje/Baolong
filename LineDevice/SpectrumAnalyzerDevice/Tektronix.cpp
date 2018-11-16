#include "Tektronix.h"
#include "main.h"

/**
 * @brief 构造函数
 * @param port 端口号
 * @param ip IP地址
 */
Tektronix::Tektronix(int port, string ip) : SpectrumAnalyzerBase(port,ip)
{
    OPEN_DISPLAY = "SYST:KLOC ON";
    CLOSE_DISPLAY = "SYST:KLOC OFF";
    RST = "*RST";
    SET_CENT = ":SPEC:FREQ:CENT ";
    SET_SPAN = ":SPEC:FREQ:SPAN ";
    SET_REF = "INPUT:RLEVEL ";//-10 dBm
    GET_POWER = "CALC:SPEC:MARK0:Y?";
    GET_FREQ = "CALC:SPEC:MARK0:X?";
    SET_MAX_HOLD_MODE = "TRAC:SPEC:FUNC MAXH";//波形峰值保持
    SET_CLEAR_WRITE_MODE = "TRAC:SPEC:FUNC NONE";//波形取消峰值保持
    SET_MARK_TOP = "CALC:SPEC:MARK0:MAX";//找一次峰值
    SET_MARK_NEXT_TOP = "CALC:SPEC:MARK0:PEAK:LOW";//找下一次峰值
    SET_PEAK_EXC = "CALC:MARK:PEAK:EXC ";
    ADD_Mark = "CALC:MARK:ADD";
    Delete_Mark = "CALC:MARK:AOFF";
    delay = 10;//延迟时间
}

/**
 * @brief 构造函数
 * @param port 端口号
 * @param ip IP地址
 * @param name 设备名称
 */
Tektronix::Tektronix(int port, string ip, string name) : SpectrumAnalyzerBase(port,ip,name)
{
    OPEN_DISPLAY = "SYST:KLOC ON";
    CLOSE_DISPLAY = "SYST:KLOC OFF";
    RST = "*RST";
    SET_CENT = ":SPEC:FREQ:CENT ";
    SET_SPAN = ":SPEC:FREQ:SPAN ";
    SET_REF = "INPUT:RLEVEL ";//-10 dBm
    GET_POWER = "CALC:SPEC:MARK0:Y?";
    GET_FREQ = "CALC:SPEC:MARK0:X?";
    SET_MAX_HOLD_MODE = "TRAC:SPEC:FUNC MAXH";//波形峰值保持
    SET_CLEAR_WRITE_MODE = "TRAC:SPEC:FUNC NONE";//波形取消峰值保持
    SET_MARK_TOP = "CALC:SPEC:MARK0:MAX";//找一次峰值
    SET_MARK_NEXT_TOP = "CALC:SPEC:MARK0:PEAK:LOW";//找下一次峰值
    SET_PEAK_EXC = "CALC:MARK:PEAK:EXC ";
    ADD_Mark = "CALC:MARK:ADD";
    Delete_Mark = "CALC:MARK:AOFF";
    delay = 10;//延迟时间
}

/**
 * @brief 析构函数
 */
Tektronix::~Tektronix()
{}

/**
 * @brief 通信测试
 * @return 通信成功返回true，失败返回false
 */
bool Tektronix::CanAcess()
{//*IDN?
//    cout<<"--- GET_DEVICE_SPC = "<<GET_DEVICE_SPC<<endl;
    string result = WriteAndRead(GET_DEVICE_SPC + endStr,"Ascii",200);
//    cout<<"Tektronix CanAcess --- result = "<<result<<endl;
    if(!result.empty() && result.length() > 0)
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}

/**
 * @brief 初始化函数
 */
void Tektronix::Initialize(DeviceInfo di)
{
    string centerFreq;
    string span;        //带宽
    string refl;        //参考准位
    string peakExc;     //分辨率
    int type;

    for(int i=0; i<di.parameterNo.size(); i++)
    {
        type = atoi((const char *)di.parameterNo.at(i).data());
        if(di.parameter.size() > i)
        {
            switch(type)
            {
                case CenterFreq:
                    centerFreq = di.parameter.at(i);
                break;
                case Span:
                    span = di.parameter.at(i);
                break;
                case Refl:
                    refl = di.parameter.at(i);
                break;
                case PeakExc:
                    peakExc = di.parameter.at(i);
                break;
            default:
                break;
            }
        }
//        else
//            _log.LOG_ERROR("Tektronix Initialize 获取频谱仪初始化参数【失败】");

    }
    if(centerFreq.empty() || span.empty() || refl.empty() || peakExc.empty())
    {
        m_db.Write_TagMValue(di.devParaAlarm, "1");
        _log.LOG_ERROR("Tektronix Initialize 获取频谱仪【加工参数】【失败】");
    }

    SetCenterFreq(atof((const char*)centerFreq.data()),"MHz");
    SetSpan(atof((const char*)span.data()),"MHz");
    SetRefl(atoi((const char*)refl.data()));
    SetPeakExc(atoi((const char*)peakExc.data()));
}

/**
 * @brief 设定中心频率
 * @param freq
 * @param unit
 */
void Tektronix::SetCenterFreq(double freq, string unit)
{
    Write(SET_CENT + DoubleToString(freq) + " " + unit + endStr,"Ascii");
    usleep(delay * 1000);
}

/**
 * @brief 设定带宽
 * @param span
 * @param unit
 */
void Tektronix::SetSpan(double span, string unit)
{
    Write(SET_SPAN + DoubleToString(span) + " " + unit + endStr,"Ascii");
    usleep(delay * 1000);
}

/**
 * @brief 设置参考准位
 * @param refl
 */
void Tektronix::SetRefl(int refl)
{
    Write(SET_REF + IntToString(refl) + " dBm" + endStr,"Ascii");
    usleep(delay * 1000);
}

/**
 * @brief Tektronix::SetPeakExc             设置分辨率
 * @param exc
 */
void Tektronix::SetPeakExc(int exc)
{
    Write(SET_PEAK_EXC + IntToString(exc) + endStr,"Ascii");
    usleep(delay * 1000);
}


/**
 * @brief 切换屏幕显示
 * @param display
 */
void Tektronix::SetDisplay(bool display)
{
    Write(display ? OPEN_DISPLAY : CLOSE_DISPLAY,"Ascii");
    usleep(delay * 1000);
}

/**
 * @brief 重置
 */
void Tektronix::Reset()
{
    Write(RST + endStr,"Ascii");
    usleep(delay * 1000);
}

/**
 * @brief 设定通道A做峰值保持
 */
void Tektronix::KeepMode()
{
    Write(SET_MAX_HOLD_MODE + endStr,"Ascii");
    usleep(delay * 1000);
}

/**
 * @brief 清除模式
 */
void Tektronix::ClearMode()
{
    Write(SET_CLEAR_WRITE_MODE + endStr,"Ascii");
    usleep(delay * 1000);
}

/**
 * @brief 定Mark1在峰值上
 */
void Tektronix::MarkTop()
{
    Write(SET_MARK_TOP + endStr,"Ascii");
    usleep(delay * 1000);
}

/**
 * @brief 定Mark1在次峰值上
 */
void Tektronix::MarkNext()
{
    Write(SET_MARK_NEXT_TOP + endStr,"Ascii");
    usleep(delay * 1000);
}

/**
 * @brief Tektronix::ReadPower
 * @return
 */
double Tektronix::ReadPower()
{
    string s = WriteAndRead(GET_POWER + endStr,"Ascii");
    _log.LOG_DEBUG("Tektronix ReadPower 获取数据power = 【%s】",s.data());
    return StringToDouble(s);
}

/**
 * @brief Tektronix::ReadFreq
 * @return
 */
double Tektronix::ReadFreq()
{
    string s = WriteAndRead(GET_FREQ + endStr,"Ascii");
   _log.LOG_DEBUG("Tektronix ReadFreq 获取数据freq = 【%s】",s.data());
    return StringToDouble(s);
}


/**
 * @brief Tektronix::AddMark
 */
void Tektronix::AddMark()
{
    Write(ADD_Mark + endStr,"Ascii");
    usleep(delay * 1000);
}

/**
 * @brief Tektronix::DeleteMark
 */
void Tektronix::DeleteMark()
{
    Write(Delete_Mark + endStr,"Ascii");
    usleep(delay * 1000);
}

/**
 * @brief 对频谱仪采集值进行判定
 * @param 功率采集值
 * @param 带宽采集值
 * @param 中心频率采集值
 * @param di
 * @return  判定结果集
 * 其中第一个元素为综合判定结果，第二个元素为中心频率判定结果，第三个元素为中心频率采集值
 * 第四个元素为带宽判定结果，第五个元素为带宽采集值
 * 第六个元素为功率判定结果，第七个元素为功率采集值
 */
vector<string> Tektronix::processValue(double power, double span, double centerFre, DeviceInfo di)
{
    vector<string> judgeResult;
    int type=0;
    double powerMin=0.0, powerMax=0.0;
    double spanMin=0.0, spanMax=0.0;
    double centerFreStandard=0.0 , centerFredeviation = 0.0;
    int powerEnable = 1, spanEnable = 1, centerFreEnable = 1;
    bool poweResult, spanResult, centerFreResult;

    judgeResult.clear();

    for(int i=0; i < di.testItemCode.size(); i++)
    {
        type = atoi((const char *)di.testItemCode.at(i).data());
        switch(type)
        {
            case CenterFreqTest:            //中心频率
                centerFreStandard = di.standardValue.at(i);
                centerFredeviation = di.deviation.at(i);
                centerFreEnable = di.judgeEneble.at(i);
                if(fabs(centerFreStandard) <= EPSINON || fabs(centerFredeviation) <= EPSINON)
                {
                    m_db.Write_TagMValue(di.testParaAlarm, "1");
                    _log.LOG_ERROR("Tektronix Initialize 获取频谱仪【测试项参数】中心频率【失败】");
                }
            break;
            case SpanTest:                  //带宽
                spanMax = di.maxValue.at(i);
                spanMin = di.minValue.at(i);
                spanEnable = di.judgeEneble.at(i);
                if(fabs(spanMax) <= EPSINON)
                {
                    m_db.Write_TagMValue(di.testParaAlarm, "1");
                    _log.LOG_ERROR("Tektronix Initialize 获取频谱仪【测试项参数】带宽【失败】");
                }
            break;
            case PowTest:                   //功率
                powerMax = di.maxValue.at(i);
                powerMin = di.minValue.at(i);
                powerEnable = di.judgeEneble.at(i);
                if(fabs(powerMax) <= EPSINON || fabs(powerMin) <= EPSINON)
                {
                    m_db.Write_TagMValue(di.testParaAlarm, "1");
                    _log.LOG_ERROR("Tektronix Initialize 获取频谱仪【测试项参数】功率【失败】");
                }
            break;
            default:
            break;
        }
    }
    if(centerFreEnable != 0)
        centerFreResult = (fabs(centerFre - centerFreStandard) <= centerFredeviation) ? 1 : 0;
    else
        centerFreResult = true;
    cout<<di.Name<<"--- centerFreResult = "<<centerFreResult<<" -- centerFre = "<<centerFre<<" -- centerFreStandard = "<<centerFreStandard<<" -- centerFredeviation = "<<centerFredeviation<<endl;
    if(spanEnable != 0)
        spanResult = (span >= spanMin && span <= spanMax) ? 1 : 0;
    else
        spanResult = true;
    cout<<di.Name<<"--- spanResult = "<<spanResult<<" -- span = "<<span<<" -- spanMin = "<<spanMin<<" -- spanMax = "<<spanMax<<endl;
    if(powerEnable != 0)
        poweResult = ( power >= powerMin && power <= powerMax) ? 1 : 0;
    else
        poweResult = true;
    cout<<di.Name<<"--- poweResult = "<<poweResult<<" -- power = "<<power<<" -- powerMin = "<<powerMin<<" -- powerMax = "<<powerMax<<endl;

    if ((poweResult == 1) && (spanResult == 1) && (centerFreResult == 1))
        judgeResult.push_back("1");
    else
        judgeResult.push_back("0");

    judgeResult.push_back(centerFreResult ? "1" : "0");
    judgeResult.push_back(DoubleToString(centerFre, "%.2f"));
    judgeResult.push_back(spanResult ? "1" : "0");
    judgeResult.push_back(DoubleToString(span, "%.2f"));
    judgeResult.push_back(poweResult ? "1" : "0");
    judgeResult.push_back(DoubleToString(power, "%.2f"));
    return judgeResult;
}




/**
 * @brief 功率测试、判定
 * @param min 判定参数：最小值
 * @param max 判定参数：最大值
 * @return 返回值包括判定结果、功率、频率、带宽，用“/”分割
 */
vector<string> Tektronix::AutoPowerJudge(DeviceInfo di)
{    
#if 1
    double centerFre = -1.0;
    double span = -1.0;
    double power1=0.0, freq1=0.0;
    double power2=0.0, freq2=0.0;
    vector<string> judgeResult;

#if 1
    Initialize(di);
    cout<<"Tektronix Initialize is OK .... "<<endl;
    ClearMode();
    cout<<"Tektronix ClearMode is Ok .... "<<endl;
    KeepMode();
    //测试中
    cout<<"Tektronix is Collecting Value .... "<<endl;
    sleep(6);
    cout<<"Tektronix is Collecting 时间8秒 .... "<<endl;
    AddMark();
    MarkTop();
    power1 = ReadPower();
    freq1 = ReadFreq();
#ifdef _Debug
    string partId = m_db.Read_TagMValue(di.IdFlag);
    if((atoi(partId.data()) % 2) == 0)
    {
        power1 = -40;
        freq1 = 433.878 * 1000000;
    }
    else
    {
        power1 = -21;
        freq1 = 433.962 * 1000000;
    }
    if((atoi(partId.data()) % 3) == 0)
    {
        power1 = -19;
        freq1 = 433.964 * 1000000;
    }
#endif
    cout<<"--- power1 = "<<power1<<" -- freq1 = "<<freq1<<endl;
    _log.LOG_DEBUG("Tektronix AutoPowerJudge 获取频谱仪power1=【%f】 freq1=【%f】",power1,freq1);

//    sleep(2);
    MarkNext();
    power2 = ReadPower();
    freq2 = ReadFreq();
#ifdef _Debug
    partId = m_db.Read_TagMValue(di.IdFlag);
    if((atoi(partId.data()) % 2) == 0)
    {
        freq2 = 433.958 * 1000000;
    }
    else
    {
        freq2 = 433.879 * 1000000;
    }
    if((atoi(partId.data()) % 3) == 0)
    {
        freq2 = 433.879 * 1000000;
    }
#endif
    cout<<"--- power2 = "<<power2<<" -- freq2 = "<<freq2<<endl;
    _log.LOG_DEBUG("Tektronix AutoPowerJudge 获取频谱仪power2=【%f】 freq2=【%f】",power2,freq2);
    DeleteMark();
    centerFre = (freq1 + freq2) / 2.0 / 1000000;
    cout<<"--- centerFre = "<<centerFre<<endl;
    span = fabs((freq1 - freq2) / 1000);
    cout<<" -- span = "<<span<<endl;
#endif

//    cout<<"--- centerFre = "<<centerFre<<" -- span = "<<span<<" -- power1 = "<<power1;
//    centerFre= 433.92 ,span = 50,power1 = -30;

    judgeResult = processValue(power1, span, centerFre, di);

    return judgeResult;
#endif
}

/**
 * @brief Tektronix::ReadSetCenterFreq
 * @return
 */
double Tektronix::ReadSetCenterFreq()
{
    return 0.00;
}
