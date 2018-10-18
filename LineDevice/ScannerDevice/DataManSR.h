#ifndef DATAMANSR_H
#define DATAMANSR_H

#include "../Device/ComDevice.h"
#include "../Device/SerialPortParameters.h"
#include "./DataConvert/DataType.h"
#include <sys/time.h>

class DataManSR : public ComDevice
{

public:

    DataManSR();
    DataManSR(SerialPortParameters *spp, string name);

    bool CanAcess();
    void StartScan();
    void StopScan();

    string GetBarCode();
    string AutoScan(int times);

private:

    const string TestCmd;
    const string StartCmd;
    const string StopCmd;

    string cmd;
    bool _Stop;

};

#endif // DATAMANSR_H
