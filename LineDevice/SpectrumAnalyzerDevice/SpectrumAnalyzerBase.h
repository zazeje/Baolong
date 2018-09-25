#ifndef SPECTRUMANALYZERBASE_H
#define SPECTRUMANALYZERBASE_H

#include "./LineDevice/Device/TcpDevice.h"
#include "./LineModel/DeviceConfig.h"


enum SpectrumAnalyzerPara       //设备加工参数枚举
{
    CenterFreq=1,               //中心频率
    Span,                       //带宽
    Refl,                       //参考准位
    PeakExc,
};

enum SpectrumAnalyzerTest       //设备测试项参数枚举
{
    CenterFreqTest=1,           //中心频率
    SpanTest,                   //带宽
    PowTest,                    //功率
};



class SpectrumAnalyzerBase : public TcpDevice
{

protected:

    SpectrumAnalyzerBase(int port,string ip);
    SpectrumAnalyzerBase(int port,string ip,string name);
    const string GET_DEVICE_SPC;
    string OPEN_DISPLAY;
    string CLOSE_DISPLAY;
    string RST;
    string SET_REF;//-25 dBm
    string SET_MAX_HOLD_MODE;
    string SET_CLEAR_WRITE_MODE;
    string SET_OPEN_MARK1;//打开mark
    string SET_MARK_TOP;
    string SET_MARK_NEXT_TOP;
    string GET_POWER;
    string GET_FREQ;
    string OPEN_PEAK_SEARCH_LIST_2;//打开peak search list,找最大两个点
    string SET_ORDER_LIST_BY_POWER;//按功率排序
    string GET_POWER_LIST;
    string GET_FREQ_LIST;
    string GET_FREQ_CENT;

    string SET_PEAK_EXC_STAT_OFF;
    string SET_PEAK_THR_OFF;
    string SET_SINGLE;

public:

    virtual ~SpectrumAnalyzerBase() = 0;
    virtual void Initialize(DeviceInfo di) = 0;
    virtual bool CanAcess() = 0;
    virtual void SetDisplay(bool display) = 0;
    virtual void Reset() = 0;
    virtual void SetCenterFreq(double freq, string unit) = 0;
    virtual void SetSpan(double span, string unit) = 0;
    virtual void SetRefl(int refl) = 0;
    virtual void KeepMode() = 0;
    virtual void ClearMode() = 0;
    virtual void MarkTop() = 0;
    virtual void MarkNext() = 0;
    virtual double ReadPower() = 0;
    virtual double ReadFreq() = 0;
    virtual double ReadSetCenterFreq() = 0;
    virtual vector<string> AutoPowerJudge(DeviceInfo di) = 0;

};

#endif
