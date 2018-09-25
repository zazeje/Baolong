#ifndef THREADTPMSHANDLE_H
#define THREADTPMSHANDLE_H

#include "LineDevice/TpmsHandleDevice/TpmsHandle.h"
#include "LineDevice/Device/DeviceThread.h"
#include "DataHelper/MemoryDBHelper.h"
#include "ThreadXse6.h"
#include "DataCommunication/TcpServer.h"
#include "DataHelper/LogFile.h"
#include "DataHelper/ArithmeticHelper.h"
#include "LineDevice/TpmsHandleDevice/TpmsHandleTcp.h"

#define TestItemNoExist -1

enum TpmsHandlePara     //加工参数
{
    PressFormula = 1,       //气压计算公式
    FrameLength,             //数据帧长度
    VoltageFormula          //电压数据帧长度
};

enum CollectMode
{
    Press, Power
};

enum PressFormulaMode
{
    NoFormula, _3_5Bar, _5Bar, _6Bar, _7Bar, _14Bar, _3_5BarAdd0_5
};

class ThreadTpmsHandle : public DeviceThread
{
public:

    ThreadTpmsHandle();
    ThreadTpmsHandle(DeviceInfo di);
    ~ThreadTpmsHandle();

    bool Start();
    bool Stop();

    double GetAverageValue(string _cValue);
    void UpdateFormula();
    double CalcuateData(double nativedata,string formula);
    void InitTestItemInfo();

private:

    TpmsHandleTcp *myTcpDevice;
    TpmsHandle *myComDevice;
    bool _stopprocess;
    int _tpmsDataLength;
    string _pressFormula;
    string _voltageFormula;
    string _posfixpressFormula;
    string _posfixvoltageFormula;
    string _tpmsDatas;
    string _iValuePress;        //气压瞬时值
    string _iValueVoltage;        //电压瞬时值
    string _iCollectPress;          //气压收集值
    string _iCollectVoltage;        //电压收集值
    string _id;
    pthread_t pth;
    pthread_t _collectThread;
    StationInfo m_stationInfo;
    static bool updateFlag;
    bool _stopColl;
    CollectMode m_mode;
    PressFormulaMode m_formula;
    ArithmeticHelper m_calculateHelper;
    vector<string> m_JR;
    string _collectValue;   //气压收集值
    int m_nCurrTestItemIndex;
    int m_nPressTestItemIndex;
    int m_nVoltageTestItemIndex;
    string m_CurrPartNo;
    double m_VoltageMaxValue;
    double m_VoltageMinValue;

    string LowFrequencyWakeup(int wakeTimeLen);
    string AccelerationWakeup(string id, int times);
    void UpdateAccelerationInfo(string ids,string result);
    void checkCalcuateParameter();
    void CheckDataAndCalcuatePress();
    void CheckDataAndGetID();
    void processStart();
    void collectData();
    void getDeviceParameter();
    string sendID(string id);
    void clearData(string tagName);
    void trimTpmsData();
    string extractTpmsData();
    void judgeTpmsCheck(int testItemJudgeResult);
    void CommunicateTest();
    void processTpmsCheck(string partSeqNo, string partNoId);
    string getData();
    void checkTpmsAwakenedId(string id);
    void ParseScannerID(string _id,string partSeqNo);
    void getJRVaule();
    void startJudge();
    void jumpJudge();
    void CheckJudge();
    void sendValueToTcpServer(string cValue);
    void ProcessTpmsCValue(string partSeqNo, string partNoId,int currTestItemIndex,JudgeMethod judgeMethod,string cValue);

    static void* StartThread(void*);
    static void* CollectThread(void*);

    void initDevice();
    void checkTestPara(int index);
    bool m_simulationDataFlag;
};

#endif //THREADTPMSHANDLE_H
