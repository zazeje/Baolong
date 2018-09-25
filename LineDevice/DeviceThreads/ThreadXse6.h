#ifndef THREADXSE6_H
#define THREADXSE6_H

#include <semaphore.h>
#include "LineDevice/DigitalmeterDevice/Xse6Tcp.h"
#include "LineDevice/DigitalmeterDevice/Xse6Serial.h"
#include "LineDevice/Device/DeviceThread.h"
#include "DataHelper/MemoryDBHelper.h"
#include "ThreadTpmsHandle.h"
#include "DataCommunication/TcpServer.h"
#include "DataHelper/LogFile.h"


enum Xse6Test
{
    Xse,
};


class ThreadXse6 : public DeviceThread
{
public:

    ThreadXse6();
    ThreadXse6(DeviceInfo di);
    ~ThreadXse6();

    bool Start();
    bool Stop();

private:

    Xse6Tcp *myTcpDevice;
    pthread_t _collectThread;
    string endFlag;
    string _iValue;
    string _cValue;
    bool _updateFlag;

    bool _stopColl;

    void CollectData();
    void ProcessStartThread();

    static void *CollectThread(void*);
    static void *StartThread(void*);
    void processXse6Check(string partSeqNo, string partNoId, Tag& tag);
    void clearData(string tagName);
    void CommunicateTest();
    void processEndFlag();
    void judgeXse6Check(int testItemJudgeResult);
    double GetAverageValue();


};


#endif //THREADXSE6_H
