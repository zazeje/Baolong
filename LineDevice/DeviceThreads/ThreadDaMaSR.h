#ifndef THREADDAMASR_H
#define THREADDAMASR_H

#include "../Device/DeviceThread.h"
#include "./LineDevice/ScannerDevice/DataManSR.h"
#include "./LineDriver/LogicalDriver.h"
#include "./LineDevice/DeviceThreads/ThreadLaserMarker.h"

using namespace std;

class ThreadDaMaSR : public DeviceThread
{
public:

    ThreadDaMaSR(DeviceInfo di);
    ThreadDaMaSR();
    ~ThreadDaMaSR();

    bool Start();
    void threadprocess();
    static void *Start_Thread(void*arg);
    bool Stop();

private:

    DataManSR *myDevice;

    bool _stopprocess;
    bool _updateFlag;
    string _iValue;
    string _cValue;
    string _lastProductModel;
    string _productModel;
    pthread_t pth;

};




#endif // THREADDAMASR_H










