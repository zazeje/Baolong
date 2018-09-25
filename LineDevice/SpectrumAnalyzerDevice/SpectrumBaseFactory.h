#ifndef SPECTRUMBASEFACTORY_H
#define SPECTRUMBASEFACTORY_H

#include "./LineModel/DeviceConfig.h"
#include "SpectrumAnalyzerBase.h"
#include "AgilentN9010A.h"
#include "RohdeSchwarz.h"
#include "Tektronix.h"

class SpectrumBaseFactory
{

public:

    SpectrumBaseFactory();
    static SpectrumAnalyzerBase*& GetSAOper();
    static SpectrumAnalyzerBase*& GetSAOper(DeviceInfo& di);
    static void SetDefaultDeviceInfo(DeviceInfo& di);

private:

    static DeviceInfo m_di;
    static SpectrumAnalyzerBase*& GetSAOperByDeviceInfo(DeviceInfo& di);

};

#endif //SPECTRUMBASEFACTORY_H
