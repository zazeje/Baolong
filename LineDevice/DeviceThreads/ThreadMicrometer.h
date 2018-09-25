#ifndef THREADMICROMETER_H
#define THREADMICROMETER_H

#include "LineDevice/Device/DeviceThread.h"
#include "LineDevice/DigitalmeterDevice/Micrometer.h"
#include "DataHelper/LogFile.h"
#include "LineDevice/DigitalmeterDevice/MicrometerTcp.h"

enum MicrometerPara
{
    Mircrometer1 = 1,           //1号补偿值
    Mircrometer2                //2号补偿值
};


class ThreadMicrometer : public DeviceThread
{
public:

    ThreadMicrometer(DeviceInfo di);
    ~ThreadMicrometer();

    bool Start();
    bool Stop();
    void threadprocess();
    static void *Start_Thread(void*arg);
    int getTestItemInfo();

    //add by vincent 20170810
    string getParameterNoInfo();
    vector<double> initDevice();
    void CommunicateTest();
    int processMicrometerCheck(string partSeqNo, string partNoId, vector<double> &paraVec);
    string judgeMicrometerCheck(int index,vector<double>& result,vector<double> &paraVec);
    void judgeParameterCheck(vector<double> &paraVec);
    void updateJudgeResult(int oktimes,int index,vector<double>& result);
    string getMicrometerValue(int &oktimes,int index,vector<double>& result,vector<double> &paraVec);

private:

    MicrometerTcp *myTcpDevice;
    Micrometer *myComDevice;
    bool _stopprocess;
    bool _updateFlag;
    pthread_t pth;
    MicrometerPara devPara;

};

#endif // THREADMICROMETER_H
