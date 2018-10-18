#ifndef THREADDGPLC_H
#define THREADDGPLC_H

#include "LineDevice/Device/DeviceThread.h"
#include "LineDevice/PlcDevice/DagongPlc.h"
#include <vector>
#include <unistd.h>
#include <map>
#include <list>
#include <string.h>
#include <stdio.h>

using namespace std;

class ThreadDgPlc : public DeviceThread
{
public:

    ThreadDgPlc(DeviceInfo di);
    ~ThreadDgPlc();

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
    DagongPlc *myDevice;
    void ProcessUnit(UnitInfo);

};

#endif // THREADDGPLC_H
