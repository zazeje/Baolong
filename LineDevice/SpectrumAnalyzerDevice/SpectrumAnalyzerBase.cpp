#include "SpectrumAnalyzerBase.h"

SpectrumAnalyzerBase::SpectrumAnalyzerBase(int port,string ip) : TcpDevice(port,ip),GET_DEVICE_SPC("*IDN?")
{}

SpectrumAnalyzerBase::SpectrumAnalyzerBase(int port,string ip,string name) : TcpDevice(port,ip,name),GET_DEVICE_SPC("*IDN?")
{}

SpectrumAnalyzerBase::~SpectrumAnalyzerBase()
{}

