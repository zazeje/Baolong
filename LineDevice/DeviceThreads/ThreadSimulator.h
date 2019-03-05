#ifndef THREADSIMULATOR_H
#define THREADSIMULATOR_H

#include "LineDevice/Device/DeviceThread.h"
#include "DataHelper/LogFile.h"
#include "LineDevice/SimulatorDevice/SimulatorTcp.h"


class ThreadSimulator : public DeviceThread
{
public:

    ThreadSimulator(DeviceInfo di);
    ~ThreadSimulator();

    bool Start();
    bool Stop();
    void threadprocess();
    static void *Start_Thread(void*arg);
    int getTestItemInfo();

    vector<string> getParameterNoInfo();
    bool initDevice();
    void CommunicateTest();
    int processSimulateCheck(string partSeqNo, string partNoId, vector<double> &paraVec);
    string judgeMicrometerCheck(int index,vector<double>& result,vector<double> &paraVec);
    void judgeParameterCheck(vector<double> &paraVec);
    void updateJudgeResult(int oktimes,int index,vector<double>& result);
    string getMicrometerValue(int &oktimes,int index,vector<double>& result,vector<double> &paraVec);

private:

    SimulatorTcp *myTcpDevice;
    bool _stopprocess;
    bool _updateFlag;
    pthread_t pth;

};

#endif // THREADSIMULATOR_H
