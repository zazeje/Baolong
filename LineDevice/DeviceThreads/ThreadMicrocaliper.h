#ifndef THREADMICROCALIPER_H
#define THREADMICROCALIPER_H

#include "LineDevice/DigitalmeterDevice/MicrocaliperTcp.h"
#include "LineDevice/DigitalmeterDevice/Microcaliper.h"
#include "LineDevice/Device/DeviceThread.h"
#include "DataHelper/LogFile.h"


enum MircocaliperTestItem
{
    Mircocaliper1 = 1,
    Mircocaliper2,
};

enum MircocaliperDevPara
{
    Offset = 1,          //补偿值（PCB点胶）
};

class ThreadMicrocaliper : public DeviceThread
{
public:

    ThreadMicrocaliper(DeviceInfo di);
    ~ThreadMicrocaliper();

    bool Start();
    bool Stop();
    void threadprocess();
    static void *Start_Thread(void*arg);
    int getTestItemInfo();
    double getDeviceParameter();

private:

    bool _stopprocess;
    bool _updateFlag;
    pthread_t pth;
    MircocaliperTestItem devTest;
    Microcaliper *myComDevice;
    MicrocaliperTcp *myTcpDevice;
    MircocaliperDevPara DevPara;

    string m_partNoId;
    string m_partSeqNo;
    double m_result1;
    double m_result2;
    double m_result3;
    double m_result;
    double m_offset;
    string m_collectValue;
    string m_EigenValue;
    void processLeakageSB1(string partNoID, string partSeqNo);
    void processLeakageSB2(string partNoID, string partSeqNo);
    void processLeakageSB3(string partNoID, string partSeqNo);
    double getLeakageResult();
    void getResult();
    void startToJudge(int index);
    void saveInfotoPi();
    void clearSB1Data(string tagName);
    void clearSB2Data(string tagName);
    void clearSB3Data(string tagName);
    double getOffsetVaule();
    void startToJudgeSB3(int index);
    void CommunicateTest();
    void processEndFlag();
};

#endif // THREADMICROCALIPER_H
