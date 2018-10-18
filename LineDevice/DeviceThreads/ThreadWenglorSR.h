#ifndef THREADWENGLORSR_H
#define THREADWENGLORSR_H

#include "../Device/DeviceThread.h"
#include "./LineDevice/ScannerDevice/WenglorSR.h"
#include "./LineDriver/LogicalDriver.h"

using namespace std;

class ThreadWenglorSR : public DeviceThread
{
public:

    ThreadWenglorSR();
    ThreadWenglorSR(DeviceInfo di);
    ~ThreadWenglorSR();

    bool Start();
    bool Stop();

    void threadprocess();
    static void *Start_Thread(void*arg);

private:

    bool _stopprocess;
    bool _updateFlag;
    string _iValue;
    string _cValue;
    string _lastProductModel;
    string _productModel;
    pthread_t pth;

    WenglorSR *myDevice;

};

#endif // THREADWENGLORSR_H

