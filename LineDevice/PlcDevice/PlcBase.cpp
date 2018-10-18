#include "PlcBase.h"

PlcBase::PlcBase() : ComDevice()
{}

PlcBase::PlcBase(SerialPortParameters* spp, bool blockMode) : ComDevice(spp, blockMode)
{}

PlcBase::PlcBase(SerialPortParameters* spp, string deviceName, bool blockMode) : ComDevice(spp, deviceName, blockMode)
{}

PlcBase::~PlcBase()
{}
