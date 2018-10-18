#include "RohdeSchwarz.h"

/**
 * @brief 构造函数
 * @param port 端口号
 * @param ip IP地址
 */
RohdeSchwarz::RohdeSchwarz(int port, string ip) : SpectrumAnalyzerBase(port,ip)
{
    OPEN_DISPLAY = "SYST:DISP:UPD ON";
    CLOSE_DISPLAY = "SYST:DISP:UPD OFF";
    RST = "*RST";
    SET_REF = "DISP:WIND:TRAC:Y:RLEV ";//-25 dBm
    SET_MAX_HOLD_MODE = "DISP:WIND:TRAC:MODE MAXH";
    SET_CLEAR_WRITE_MODE = "DISP:WIND:TRAC:MODE WRIT";
    SET_OPEN_MARK1 = "CALC:MARK:TRAC 1";//打开mark
    SET_MARK_TOP = "CALC:MARK:MAX";
    SET_MARK_NEXT_TOP = "CALC:MARK:MAX:NEXT";
    GET_POWER = "CALC:MARK:Y?";
    GET_FREQ = "CALC:MARK:X?";
    OPEN_PEAK_SEARCH_LIST_2 = "CALC:MARK:FUNC:FPE 2;*WAI";//打开peak search list,找最大两个点
    SET_ORDER_LIST_BY_POWER = "CALC:MARK:FUNC:FPE:SORT Y";//按功率排序
    GET_POWER_LIST = "CALC:MARK:FUNC:FPE:Y?";
    GET_FREQ_LIST = "CALC:MARK:FUNC:FPE:X?";
    GET_FREQ_CENT = "FREQ:CENT?";
}

/**
 * @brief 构造函数
 * @param port 端口号
 * @param ip IP地址
 * @param name 设备名称
 */
RohdeSchwarz::RohdeSchwarz(int port, string ip,string name) : SpectrumAnalyzerBase(port,ip,name)
{
    OPEN_DISPLAY = "SYST:DISP:UPD ON";
    CLOSE_DISPLAY = "SYST:DISP:UPD OFF";
    RST = "*RST";
    SET_REF = "DISP:WIND:TRAC:Y:RLEV ";//-25 dBm
    SET_MAX_HOLD_MODE = "DISP:WIND:TRAC:MODE MAXH";
    SET_CLEAR_WRITE_MODE = "DISP:WIND:TRAC:MODE WRIT";
    SET_OPEN_MARK1 = "CALC:MARK:TRAC 1";//打开mark
    SET_MARK_TOP = "CALC:MARK:MAX";
    SET_MARK_NEXT_TOP = "CALC:MARK:MAX:NEXT";
    GET_POWER = "CALC:MARK:Y?";
    GET_FREQ = "CALC:MARK:X?";
    OPEN_PEAK_SEARCH_LIST_2 = "CALC:MARK:FUNC:FPE 2;*WAI";//打开peak search list,找最大两个点
    SET_ORDER_LIST_BY_POWER = "CALC:MARK:FUNC:FPE:SORT Y";//按功率排序
    GET_POWER_LIST = "CALC:MARK:FUNC:FPE:Y?";
    GET_FREQ_LIST = "CALC:MARK:FUNC:FPE:X?";
    GET_FREQ_CENT = "FREQ:CENT?";
}

/**
 * @brief 析构函数
 */
RohdeSchwarz::~RohdeSchwarz()
{

}

/**
 * @brief 初始化函数
 */
void RohdeSchwarz::Initialize(DeviceInfo di)
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
			_log.LOG_ERROR("RohdeSchwarz Initialize 获取频谱仪初始化参数【失败】");
    }
    SetCenterFreq(atof((const char*)centerFreq.data()),"mhz");
    SetSpan(atof((const char*)span.data()),"mhz");
    SetRefl(atoi((const char*)refl.data()));
}

/**
 * @brief 通信测试
 * @return 通信成功返回true，失败返回false
 */
bool RohdeSchwarz::CanAcess()
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
void RohdeSchwarz::SetDisplay(bool display)
{
    Write(display ? OPEN_DISPLAY : CLOSE_DISPLAY,"Ascii");
}

/**
 * @brief 重置
 */
void RohdeSchwarz::Reset()
{
    Write(RST,"Ascii");
}

/**
 * @brief 设定中心频率
 * @param freq
 * @param unit
 */
void RohdeSchwarz::SetCenterFreq(double freq, string unit)
{
    Write("FREQ:CENT " + DoubleToString(freq) + " " + unit,"Ascii");
}

/**
 * @brief 设定带宽
 * @param span
 * @param unit
 */
void RohdeSchwarz::SetSpan(double span, string unit)
{
    Write("FREQ:SPAN " + DoubleToString(span) + " " + unit,"Ascii");
}

/**
 * @brief 设置参考准位
 * @param refl
 */
void RohdeSchwarz::SetRefl(int refl)
{
    Write(SET_REF + IntToString(refl) + " dBm","Ascii");
}

/**
 * @brief 设置功率补偿
 * @param refl
 */
void RohdeSchwarz::SetTrigRefLev(int refl)
{
    Write("DISP:WIND:TRAC:Y:RLEV:OFFS " + IntToString(refl),"Ascii");
}

/**
 * @brief 设定通道A做峰值保持
 */
void RohdeSchwarz::KeepMode()
{
    Write(SET_MAX_HOLD_MODE,"Ascii");
}

/**
 * @brief 清除模式
 */
void RohdeSchwarz::ClearMode()
{
    Write(SET_CLEAR_WRITE_MODE,"Ascii");
}

/**
 * @brief 打开Mark1
 */
void RohdeSchwarz::OpenMark1()
{
    Write(SET_OPEN_MARK1,"Ascii");
}

/**
 * @brief 定Mark1在峰值上
 */
void RohdeSchwarz::MarkTop()
{
    Write(SET_MARK_TOP,"Ascii");
}

/**
 * @brief 定Mark1在次峰值上
 */
void RohdeSchwarz::MarkNext()
{
    Write(SET_MARK_NEXT_TOP,"Ascii");
}

/**
 * @brief 打开peak search list,找最大两个点
 */
void RohdeSchwarz::OpenPeakSearchList2()
{
    Write(OPEN_PEAK_SEARCH_LIST_2,"Ascii");
}

/**
 * @brief 按功率排序
 */
void RohdeSchwarz::SetOrderListByPower()
{
    Write(SET_ORDER_LIST_BY_POWER,"Ascii");
}

double RohdeSchwarz::ReadPower()
{
    return StringToDouble(WriteAndRead(GET_POWER,"Ascii"));
}

double RohdeSchwarz::ReadFreq()
{
    return StringToDouble(WriteAndRead(GET_FREQ,"Ascii"));
}

double RohdeSchwarz::ReadSetCenterFreq()
{
    return StringToDouble(WriteAndRead(GET_FREQ_CENT,"Ascii"));
}

string RohdeSchwarz::ReadPowerList()
{
    return WriteAndRead(GET_POWER_LIST,"Ascii");
}

string RohdeSchwarz::ReadFreqList()
{
    return WriteAndRead(GET_FREQ_LIST,"Ascii");
}

vector<string> RohdeSchwarz::processValue(double power, double span, double centerFre, DeviceInfo di)
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
vector<string> RohdeSchwarz::AutoPowerJudge(DeviceInfo di)
{
    double centerFre = -1;
    double span = -1;
    vector<string> judgeResult;

    Reset();
    Initialize(di);
    KeepMode();
    OpenPeakSearchList2();
    sleep(7);
    MarkTop();
    double power = ReadPower();
    string freOfPeak2 = ReadFreqList();
    if (!freOfPeak2.empty() && freOfPeak2.find(",") != string::npos)
    {
        vector<string> strFres;
        splittostring(freOfPeak2,",",strFres);
        vector<double> doubleFres;
        StringArrayToDoubleArray(strFres,doubleFres);
        if(!doubleFres.empty() && doubleFres.size() > 2)
        {
            centerFre = (doubleFres[0] + doubleFres[1]) / 2.0 / 1000000;
            span = fabs((doubleFres[0] - doubleFres[1]) / 1000);
        }
    }

    judgeResult = processValue(power, span, centerFre,di);

    return judgeResult;
}
