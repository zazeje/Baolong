#ifndef THREADVIEWCHECK_H
#define THREADVIEWCHECK_H

#include "LineDevice/ViewCheckDevice/ViewCheck.h"
#include "LineDevice/Device/DeviceThread.h"
#include "LineModel/DeviceConfig.h"
#include "DataHelper/MemoryDBHelper.h"
#include "LineDevice/DeviceThreads/ThreadLaserMarker.h"
#include "LineDriver/LogicalDriver.h"
#include "DataCommunication/TcpServer.h"

class ThreadViewCheck : public DeviceThread
{
    //视觉检测设备
    //作者 武汉分公司/李世龙
    //时间 2017-2-7
public:
    ThreadViewCheck();
    ThreadViewCheck(DeviceInfo di);
    ~ThreadViewCheck();

    bool Start();
    bool Stop();
    void threadprocess();
    static void *Start_Thread(void*arg);
    void judgeViewCheck(string result);
    void processViewCheck(string partSeqNo, string partId);
    void CommunicateTest();

private:

//    bool _stopprocess;
    pthread_t pth;
    ViewCheck *myDevice;

};

#endif // THREADVIEWCHECK_H
