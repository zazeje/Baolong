#include "SpectrumBaseFactory.h"

DeviceInfo SpectrumBaseFactory::m_di;

SpectrumBaseFactory::SpectrumBaseFactory()
{
    m_di.Init();
}

SpectrumAnalyzerBase*& SpectrumBaseFactory::GetSAOper()
{
    if(!m_di.machDriverNo.empty())
    {
        return GetSAOperByDeviceInfo(m_di);
    }
}

SpectrumAnalyzerBase*& SpectrumBaseFactory::GetSAOper(DeviceInfo& di)
{
    if(!di.machDriverNo.empty())
    {
        m_di = di;
        return GetSAOperByDeviceInfo(di);
    }
}
void SpectrumBaseFactory::SetDefaultDeviceInfo(DeviceInfo& di)
{
    m_di = di;
}

SpectrumAnalyzerBase*& SpectrumBaseFactory::GetSAOperByDeviceInfo(DeviceInfo& di)
{
    SpectrumAnalyzerBase* mydevice;

    if(di.machDriverNo == "13")
    {
        mydevice = new AgilentN9010A(atoi(di.Port.data()),di.Ip,di.Name);
    }
    else if(di.machDriverNo == "14")
    {
        mydevice = new Tektronix(atoi(di.Port.data()),di.Ip,di.Name);
    }
    else if(di.machDriverNo == "15")
    {
        mydevice = new RohdeSchwarz(atoi(di.Port.data()),di.Ip,di.Name);
    }
    return mydevice;
}

