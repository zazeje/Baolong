#ifndef THREADFX5U_H
#define THREADFX5U_H

#include "./LineDevice/PlcDevice/Fx5U.h"
#include "./LineDevice/Device/DeviceThread.h"
#include "DataHelper/LogFile.h"

#if 1
class ThreadFx5U : public DeviceThread
{
public:

    ThreadFx5U(DeviceInfo di);
    ~ThreadFx5U();

    bool Start();
    bool Stop();
    void threadprocess();
    static void *Start_Thread(void*arg);
    void ProcessUnit(UnitInfo ui);


private:

    bool _stopprocess;
    bool _updateFlag;
    pthread_t pth;
    Fx5U *myDevice;

};
#endif

#endif // THREADFX5U_H
