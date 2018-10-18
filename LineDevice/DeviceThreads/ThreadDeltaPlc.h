#ifndef THREADDELTAPLC_H
#define THREADDELTAPLC_H

#include "./LineDevice/Device/DeviceThread.h"
#include "./LineDevice/PlcDevice/DeltaPlc.h"
#include "./LineDevice/Device/SerialPortParameters.h"

class ThreadDeltaPlc : public DeviceThread
{

public:

    ThreadDeltaPlc(DeviceInfo di);
    ~ThreadDeltaPlc();

    bool Start();
    bool Stop();

private:

    DeltaPlc* myDevice;

    void ProcessUnit(UnitInfo ui);
    void threadprocess();

    static void* Start_Thread(void* );

};

#endif // THREADDELTAPLC_H
