#ifndef LOGICALDRIVER_H
#define LOGICALDRIVER_H

#include <list>
#include "PBOXUI/display.h"
#include "LineDevice/Device/DeviceThread.h"
#include "DataHelper/MemoryDBHelper.h"
#include "DataCommunication/TcpServer.h"
#include "DataHelper/LogFile.h"


using namespace std;

class LogicalDriver
{

public:

    LogicalDriver();
    ~LogicalDriver();
    LogicalDriver(LineInfo li);
    bool Start();
    bool Stop();

    void threadprocess();

    static void *Start_Thread(void*arg);
    string getPlcJR();

private:

    pthread_t pth;
    bool judgeFlag;

    bool SaveProductInfo(PartTestItemInfo &pi);

};

#endif // LOGICALDRIVER_H
