#ifndef ROHDESCHWARZ_H
#define ROHDESCHWARZ_H

#include "SpectrumAnalyzerBase.h"
#include "DataHelper/LogFile.h"

class RohdeSchwarz : public SpectrumAnalyzerBase
{

public:

    RohdeSchwarz();
    RohdeSchwarz(int port ,string ip );
    RohdeSchwarz(int port ,string ip , string name);
    ~RohdeSchwarz();

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

    void OpenPeakSearchList2();
    void SetTrigRefLev(int refl);
    void SetOrderListByPower();
    void OpenMark1();
    string ReadPowerList();
    string ReadFreqList();

    vector<string> processValue(double power, double span, double centerFre, DeviceInfo di);

};

#endif // ROHDESCHWARZ_H
