#ifndef THREADITECHPOWER_H
#define THREADITECHPOWER_H

#include "LineDevice/Device/DeviceThread.h"
#include "LineDevice/PowerSupply/ItechPower.h"
#include "LineDriver/LogicalDriver.h"
#include "LineModel/DeviceConfig.h"
#include "DataHelper/MemoryDBHelper.h"
#include "DataHelper/LogFile.h"
#include "LineDevice/PowerSupply/ItechPowerTcp.h"

using namespace std;

class ThreadItechPower : public DeviceThread
{
    //稳压电源
    //作者 武汉分公司/李世龙
    //时间 2017-2-7
public:

    ThreadItechPower(DeviceInfo di);
    ThreadItechPower();
    ~ThreadItechPower();

    bool Start();
    void threadprocess();
    static void *Start_Thread(void*arg);
    bool Stop();

private:

    ItechPowerTcp *myTcpDevice;
    ItechPower *myComDevice;
    bool _stopprocess;
    pthread_t pth;

};



#endif // THREADITECHPOWER_H
