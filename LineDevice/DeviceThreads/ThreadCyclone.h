#ifndef THREADCYCLONE_H
#define THREADCYCLONE_H

#include <vector>
#include <unistd.h>
#include <map>
#include <list>
#include <string>
#include "LineModel/LineInformation.h"
#include "LineDevice/Device/DeviceThread.h"
#include "LineModel/DeviceConfig.h"
#include "DataHelper/MemoryDBHelper.h"
#include "LineDriver/LogicalDriver.h"
#include "LineDevice/ProgramDevice/CycloneProUdp.h"
#include "DataCommunication/TcpServer.h"
#include "DataHelper/LogFile.h"

using namespace std;

enum CycloneParameterItem
{
    CyclonePath = 1,        //烧成器SAP路径SAP/QY1014(SAP/QY1014)
    CycloneAddress,         //烧成器地址(DFB2)
};

class ThreadCyclone : public DeviceThread
{
public:

    ThreadCyclone(DeviceInfo di);
    ~ThreadCyclone();

public:

    bool Start();
    void threadprocess();
    static void *Start_Thread(void*arg);
    bool Stop();
    string AssignProgramId();
    string GetProgramAddress();
    bool initDevice();
    string getPlcAlarmPoint();
    string getParameterNoInfo();

private:

    pthread_t pth;
    CycloneProUdp *myDevice;
    int _workTimes;
    string m_displayValue;
    CycloneParameterItem devPara;

    bool Init();
    void processEndFlag();
    string processCyclone(string partSeqNo);
    string getCycloneaddress();
    string getCycloneId();
    string startToCyclone(string address);
    void CommunicateTest();
    void SendCycloneInfoToPC(string partNoId,string partSeqNo);
};


#endif // THREADCYCLONE_H
