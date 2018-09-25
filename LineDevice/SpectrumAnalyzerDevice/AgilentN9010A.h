#ifndef AGILENTN9010A_H
#define AGILENTN9010A_H

#include "SpectrumAnalyzerBase.h"
#include "DataHelper/LogFile.h"


class AgilentN9010A : public SpectrumAnalyzerBase
{

public:

    AgilentN9010A();
    AgilentN9010A(int port ,string ip);
    AgilentN9010A(int port ,string ip, string name);
    ~AgilentN9010A();

    virtual void Initialize(DeviceInfo di);
    virtual bool CanAcess();
    virtual void SetDisplay(bool display);
    virtual void Reset();
    virtual void SetCenterFreq(double freq, string unit);
    virtual void SetSpan(double span, string unit);
    virtual void SetRefl(int refl);
    virtual void KeepMode();
    virtual void ClearMode();
    virtual void MarkTop();
    virtual void MarkNext();
    virtual double ReadPower();
    virtual double ReadFreq();
    virtual double ReadSetCenterFreq();
    virtual vector<string> AutoPowerJudge(DeviceInfo di);

    void SetPeakExcThrOff();
    void SetSingle();
    void SetTrigRefLev(int refl);
    vector<string> processValue(double power, double span, double centerFre, DeviceInfo di);


};

#endif //AGILENTN9010A_H
