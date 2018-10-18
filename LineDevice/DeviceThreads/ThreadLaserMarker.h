#ifndef THREADLASERMARKER_H
#define THREADLASERMARKER_H

#include "LineDevice/Device/DeviceThread.h"
#include "LineDevice/MarkerDevice/LaserMarker.h"
#include "LineModel/DeviceConfig.h"
#include "DataHelper/MemoryDBHelper.h"
#include "DataHelper/LogFile.h"
#include <vector>
#include <unistd.h>
#include <map>
#include <list>
#include <string.h>

using namespace std;

class ThreadLaserMarker : public DeviceThread
{
public:

    ThreadLaserMarker(DeviceInfo di);
    ~ThreadLaserMarker();

    bool Start();
    bool Stop();
    void threadprocess();
    static void *Start_Thread(void*arg);

private:

    bool _stopprocess;
    pthread_t pth;
    LaserMarker *myDevice;
    void getPlcStatePoint();
    string m_plcAlarm;
    string m_plcLR;
    void processLM(string id);
    void writeSeqAndID(Tag tag, string id);
    void CommunicateTest();

};


#endif // THREADLASERMARKER_H
