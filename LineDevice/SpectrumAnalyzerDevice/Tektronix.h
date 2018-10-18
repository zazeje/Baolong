#ifndef TEKTRONIX_H
#define TEKTRONIX_H

#include "SpectrumAnalyzerBase.h"
#include "LineModel/DeviceConfig.h"
#include "DataHelper/LogFile.h"
#include "DataHelper/MemoryDBHelper.h"


class Tektronix : public SpectrumAnalyzerBase
{

public:

    Tektronix();
    Tektronix(int port,string ip);
    Tektronix(int port ,string ip, string name);
    ~Tektronix();

    virtual bool CanAcess();
    virtual void Initialize(DeviceInfo di);
    virtual void SetCenterFreq(double freq, string unit);
    virtual void SetSpan(double span, string unit);
    virtual void SetRefl(int refl);
    virtual void SetDisplay(bool display);
    virtual void Reset();
    virtual void KeepMode();
    virtual void ClearMode();
    virtual void MarkTop();
    virtual void MarkNext();
    virtual double ReadPower();
    virtual double ReadFreq();
    virtual double ReadSetCenterFreq();
    virtual vector<string> AutoPowerJudge(DeviceInfo di);

    void SetPeakExc(int exc);
    void AddMark();
    void DeleteMark();
    vector<string> processValue(double power, double span, double centerFre, DeviceInfo di);

private:

    int delay;//延迟时间
    string SET_CENT;
    string SET_SPAN;
    string SET_PEAK_EXC;
    string ADD_Mark;
    string Delete_Mark;

};


#endif //TEKTRONIX_H
