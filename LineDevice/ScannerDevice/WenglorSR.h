#ifndef WENGLORSR_H
#define WENGLORSR_H

#include "../Device/ComDevice.h"
#include "../Device/SerialPortParameters.h"
#include "./DataConvert/DataType.h"
#include <sys/time.h>

class WenglorSR : public ComDevice
{

public:

    WenglorSR();
    WenglorSR(SerialPortParameters *spp, string name);
    bool CanAcess();
    string AutoScan(int times);

private:

    bool _stop;
    const string Command;

};

#endif // WENGLORSR_H


