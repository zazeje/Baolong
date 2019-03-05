#ifndef THREADMICROMETER_H
#define THREADMICROMETER_H

#include "LineDevice/Device/DeviceThread.h"
#include "LineDevice/DigitalmeterDevice/Micrometer.h"
#include "DataHelper/LogFile.h"
#include "LineDevice/DigitalmeterDevice/MicrometerTcp.h"

enum MicrometerPara
{
    Mircrometer1 = 1,           //1号补偿值
    Mircrometer2 = 2,              //2号补偿值
    Mircrometer3 = 3,              //3号补偿值
    Mircrometer4 = 4              //4号补偿值
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
    //集线器，高度测试就只有一个测试项，索引为0。没有必要写这个函数
    /*int getTestItemInfo();*/

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

    string m_PointOk;                                        //PLC扫码良品点
    string m_PointNG;                                        //PLC扫码不良品点

    string D3GetPLCPointOK();                                //D3线获取PLC“扫码不良”点位
    string D3GetPLCPointNG();

};

#endif // THREADMICROMETER_H
