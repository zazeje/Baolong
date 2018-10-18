#ifndef THREADAGILENT_H
#define THREADAGILENT_H

#include <stdlib.h>
#include <time.h>
#include "LineDevice/Device/TcpDevice.h"
#include "LineDevice/MultimeterDevice/AgilentMultimeterTcp.h"
#include "LineDevice/MultimeterDevice/AgilentMultimeterSerial.h"
#include "LineDevice/Device/SerialPortParameters.h"
#include "LineDevice/Device/DeviceThread.h"
#include "DataConvert/DataType.h"
#include "DataHelper/MemoryDBHelper.h"
#include "DataHelper/MySQLHelper.h"
#include "PBOXUI/display.h"
#include "LineDriver/LogicalDriver.h"
#include "DataCommunication/TcpServer.h"
#include "DataHelper/LogFile.h"


enum AgilentParameterItem       //加工参数枚举
{
    staticElectric=1,                 //静态电流测试
    sendElectric,                     //发射电流测试
    outputFormat,                     //皮安计输出参数
    currentEnable                     //皮安计电流使能参数
};

enum AgilentTestItem            //测试项参数枚举
{
    staticElectricTest=1,       //静态电流
    sendElectricTest,           //发射电流
    staticVoltageTest,          //静态电压
    sendVoltageTest             //发射电压
};


class ThreadAgilent;

class ThreadAgilent : public DeviceThread
{

public:

    ThreadAgilent();
    ThreadAgilent(DeviceInfo di);
    ~ThreadAgilent();

    bool Start();
    bool Stop();
    void threadprocess();
    static void *Start_Thread(void *arg);
    int _workTimes;
    int _coefficient;

private:

    bool _reinitflag;
    bool _stopprocess;
    bool _updateFlag;
    string _iValue;
    string _cValue;
    string m_displayValue;
    string _productModel;
    string _lastProductModel;
    string startFlag;
    AgilentMultimeterTcp *myTcpDevice;
    AgilentMultimeterSerial *myComDevice;
    bool _stopColl;
    static void* CollectData(void *);
    void Init();
    bool InitAgilentDevice();
    void CollectDataProcess();
    AgilentParameterItem devPara;
//    AgilentTestItem devTest;
    JudgeMethod devTest;
    int getTestItemInfo();
    int getTestItemCode();
    ServerProductInfo spi;
    string getParameterNoInfo();
    string getMachinePara();
    void CollectCurrent(int workTimes);
    void checkDevTestPara();
    void checkDevPara();
    void SaveProductSeqNoAndNoId(vector<string>& vecSeqNo);


    void processEndFlag();
    vector<string> m_PartSeqNoVector;
    vector<string> m_PartIdVector;
    vector<string> m_JR;
    int processD3Line(string partNoId, string partSeqNo);
    void processD2Line();
    int processTruckLine(string partNoId, string partSeqNo);
    void getJRVaule();
    void startJudge();

//    void noJudge();
//    void saveToSql(string partNoId, string partSeqNo);
    void sendValueToTcpServer(string cValue);
    void CommunicateTest();
    void clearData(string tagName);
    void setTestPara(int workTimes);
//    T checkDeviceType();

};

#endif // THREADAGILENT_H
