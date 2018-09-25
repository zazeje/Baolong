#include "./AgilentN9010A.h"

/**
 * @brief 构造函数
 * @param port 端口号
 * @param ip IP地址
 */
AgilentN9010A::AgilentN9010A(int port ,string ip) : SpectrumAnalyzerBase(port,ip)
{
    OPEN_DISPLAY = ":DISP:BACK ON";
    CLOSE_DISPLAY = ":DISP:BACK OFF";
    RST = ":SYST:PRES";//*RST
    SET_REF = "DISP:WIND:TRAC:Y:RLEV ";//-25 dBm
    SET_MAX_HOLD_MODE = ":TRAC1:MODE MAXH";//波形峰值保持
    SET_CLEAR_WRITE_MODE = ":TRAC1:MODE WRIT";//波形取消峰值保持
    SET_MARK_TOP = "CALC:MARK1:MAX";//找一次峰值
    SET_MARK_NEXT_TOP = "CALC:MARK1:MAX:NEXT";//找一次次峰值,间隔是0.01db
    SET_PEAK_EXC_STAT_OFF = ":CALC:MARK:PEAK:EXC:STAT OFF";
    SET_PEAK_THR_OFF = ":CALC:MARK:PEAK:THR:STAT OFF";
    SET_SINGLE = ":INIT:CONT OFF";
    GET_POWER = "CALC:MARK:Y?";
    GET_FREQ = "CALC:MARK:X?";
    GET_FREQ_CENT = "FREQ:CENT?";
}

/**
 * @brief 构造函数
 * @param port 端口号
 * @param ip IP地址
 * @param name 设备名称
 */
AgilentN9010A::AgilentN9010A(int port ,string ip, string name) : SpectrumAnalyzerBase(port,ip,name)
{
    OPEN_DISPLAY = ":DISP:BACK ON";
    CLOSE_DISPLAY = ":DISP:BACK OFF";
    RST = ":SYST:PRES";//*RST
    SET_REF = "DISP:WIND:TRAC:Y:RLEV ";//-25 dBm
    SET_MAX_HOLD_MODE = ":TRAC1:MODE MAXH";//波形峰值保持
    SET_CLEAR_WRITE_MODE = ":TRAC1:MODE WRIT";//波形取消峰值保持
    SET_MARK_TOP = "CALC:MARK1:MAX";//找一次峰值
    SET_MARK_NEXT_TOP = "CALC:MARK1:MAX:NEXT";//找一次次峰值,间隔是0.01db
    SET_PEAK_EXC_STAT_OFF = ":CALC:MARK:PEAK:EXC:STAT OFF";
    SET_PEAK_THR_OFF = ":CALC:MARK:PEAK:THR:STAT OFF";
    SET_SINGLE = ":INIT:CONT OFF";
    GET_POWER = "CALC:MARK:Y?";
    GET_FREQ = "CALC:MARK:X?";
    GET_FREQ_CENT = "FREQ:CENT?";
}

/**
 * @brief 析构函数
 */
AgilentN9010A::~AgilentN9010A()
{

}

/**
 * @brief 初始化函数
 */
void AgilentN9010A::Initialize(DeviceInfo di)
{
    string centerFreq;
    string span;        //带宽
    string refl;        //参考准位
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
                default:
                break;
            }
        }
        else
			_log.LOG_ERROR("AgilentN9010A Initialize 获取频谱仪初始化参数票【失败】");
    }
    SetCenterFreq(atof((const char*)centerFreq.data()),"mhz");
    SetSpan(atof((const char*)span.data()),"mhz");
    SetRefl(atoi((const char*)refl.data()));
}

/**
 * @brief 通信测试
 * @return 通信成功返回true，失败返回false
 */
bool AgilentN9010A::CanAcess()
{
    string result = WriteAndRead(GET_DEVICE_SPC,"Ascii");
    if(!result.empty())
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}

/**
 * @brief 切换屏幕显示
 * @param display
 */
void AgilentN9010A::SetDisplay(bool display)
{
    Write(display ? OPEN_DISPLAY : CLOSE_DISPLAY,"Ascii");
}

/**
 * @brief 重置
 */
void AgilentN9010A::Reset()
{
    Write(RST,"Ascii");
}

/**
 * @brief 设定中心频率
 * @param freq
 * @param unit
 */
void AgilentN9010A::SetCenterFreq(double freq, string unit)
{
    Write("FREQ:CENT " + DoubleToString(freq) + " " + unit,"Ascii");
}

/**
 * @brief 设定带宽
 * @param span
 * @param unit
 */
void AgilentN9010A::SetSpan(double span, string unit)
{
    Write("FREQ:SPAN " + DoubleToString(span) + " " + unit,"Ascii");
}

/**
 * @brief 设置参考准位
 * @param refl
 */
void AgilentN9010A::SetRefl(int refl)
{
    Write(SET_REF + IntToString(refl) + " dBm","Ascii");
}

/**
 * @brief 设置功率补偿
 * @param refl
 */
void AgilentN9010A::SetTrigRefLev(int refl)
{
    Write("DISP:WIND:TRAC:Y:RLEV:OFFS " + IntToString(refl),"Ascii");
}

/**
 * @brief 设定通道A做峰值保持
 */
void AgilentN9010A::KeepMode()
{
    Write(SET_MAX_HOLD_MODE,"Ascii");
}

/**
 * @brief 清除模式
 */
void AgilentN9010A::ClearMode()
{
    Write(SET_CLEAR_WRITE_MODE,"Ascii");
}

/**
 * @brief 定Mark1在峰值上
 */
void AgilentN9010A::MarkTop()
{
    Write(SET_MARK_TOP,"Ascii");
}

/**
 * @brief 定Mark1在次峰值上
 */
void AgilentN9010A::MarkNext()
{
    Write(SET_MARK_NEXT_TOP,"Ascii");
}

/**
 * @brief 关闭两个通道
 */
void AgilentN9010A::SetPeakExcThrOff()
{
    Write(SET_PEAK_EXC_STAT_OFF,"Ascii");
    Write(SET_PEAK_THR_OFF,"Ascii");
}

/**
 * @brief 设置single模式
 */
void AgilentN9010A::SetSingle()
{
    Write(SET_SINGLE,"Ascii");
}

double AgilentN9010A::ReadPower()
{
    return StringToDouble(WriteAndRead(GET_POWER,"Ascii"));
}

double AgilentN9010A::ReadFreq()
{
    return StringToDouble(WriteAndRead(GET_FREQ,"Ascii"));
}

double AgilentN9010A::ReadSetCenterFreq()
{
    return StringToDouble(WriteAndRead(GET_FREQ_CENT,"Ascii"));
}

vector<string> AgilentN9010A::processValue(double power, double span, double centerFre, DeviceInfo di)
{
    vector<string> judgeResult;
    int type=0;
    double powerMin=0.0, powerMax=0.0;
    double spanMin=0.0, spanMax=0.0;
    double centerFreStandard=0.0 , centerFredeviation = 0.0;
    bool poweResult, spanResult, centerFreResult;

    for(int i=0; i < di.testItemCode.size(); i++)
    {
        type = atoi((const char *)di.testItemCode.at(i).data());
        switch(type)
        {
            case CenterFreqTest://
                centerFreStandard = di.standardValue.at(i);
                centerFredeviation = di.deviation.at(i);
            break;
            case SpanTest:
                spanMax = di.maxValue.at(i);
                spanMin = di.minValue.at(i);
            break;
            case PowTest:
                powerMax = di.maxValue.at(i);
                powerMin = di.minValue.at(i);
            break;
        default:
            break;
        }
    }
    poweResult = ( power >= powerMin && power <= powerMax) ? 1 : 0;

    spanResult = (span >= spanMin && span <= spanMax) ? 1 : 0;

    centerFreResult = (fabs(centerFre - centerFreStandard) <= centerFredeviation) ? 1 : 0;
    if ((poweResult == 1) && (spanResult == 1) && (centerFreResult == 1))
    {
        judgeResult.push_back("1");
    }
    else
    {
        judgeResult.push_back("0");
    }
    judgeResult.push_back(DoubleToString(power));
    judgeResult.push_back(DoubleToString(span));
    judgeResult.push_back(DoubleToString(centerFre));
    return judgeResult;
}


/**
 * @brief 功率测试、判定
 * @param min 判定参数：最小值
 * @param max 判定参数：最大值
 * @return 返回值包括判定结果、功率、频率、带宽，用“/”分割
 */
vector<string> AgilentN9010A::AutoPowerJudge(DeviceInfo di)
{
    double centerFre = -1.0;
    double span = -1.0;
    double freq1=0.0;
    double power2=0.0, freq2=0.0;
    vector<string> judgeResult;
    double power = -1;

    Reset();
    Initialize(di);
    KeepMode();
    SetPeakExcThrOff();
    sleep(9);
    MarkTop();

    power = ReadPower();
    freq1 = ReadFreq();
    MarkNext();
    power2 = ReadPower();
    freq2 = ReadFreq();

    centerFre = (freq1 + freq2) / 2.0 / 1000000;
    span = fabs((freq1 - freq2) / 1000);

    judgeResult = processValue(power, span, centerFre, di);

    return judgeResult;
}
