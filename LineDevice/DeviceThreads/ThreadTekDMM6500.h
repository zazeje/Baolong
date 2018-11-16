#ifndef THREADTEKDMM6500_H
#define THREADTEKDMM6500_H

#include "./LineDevice/Device/TcpDevice.h"
#include "./LineDevice/MultimeterDevice/TekDMM6500Multimeter.h"
#include "./LineDevice/Device/DeviceThread.h"
#include "./DataConvert/DataType.h"
#include "./DataHelper/MemoryDBHelper.h"
#include <stdlib.h>
#include <time.h>
#include "./DataHelper/MySQLHelper.h"
#include "./PBOXUI/display.h"
#include "./LineDriver/LogicalDriver.h"
#include "./DataCommunication/TcpServer.h"
#include "DataHelper/LogFile.h"


enum TekDMM6500ParameterItem       //加工参数枚举
{
    TekNone=0,
    TekstaticElectric,                   //静态电流测量量程参数
    TeksendElectric,                     //发射电流测量量程参数
    TekoutputFormat,                     //皮安计输出参数
    TekcurrentEnable,                    //皮安计电流使能参数
    TekstaticVoltage,                    //静态电压测量量程参数
    TeksendVoltage,                      //发射电压测量量程参数
    TekcurrGear,                         //当前档位参数,测量电压或电流
};

enum TekDMM6500TestItem            //测试项参数枚举
{
    TekstaticElectricTest=1,       //静态电流
    TeksendElectricTest,           //发射电流
    TekstaticVoltageTest,          //静态电压
    TeksendVoltageTest             //发射电压
};

class ThreadTekDMM6500;

class ThreadTekDMM6500 : public DeviceThread
{
public:
    ThreadTekDMM6500();
    ThreadTekDMM6500(DeviceInfo di);
    ~ThreadTekDMM6500();

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
    CommunicationType type;
    TekDMM6500MultimeterTcp *myTcpDevice;
    string m_currentgear;//当前使用的档位，测试电流，或测试电压
    string m_currentrange;//当前使用的测量电流，或测量电压的量程
//    string m_range1;//加工项参数中的量程1
//    string m_range2;//加工项参数中的量程2
    bool m_bInitResult;

    bool _stopColl;
    static void* CollectData(void *);
    void Init();
    bool InitTekDMM6500Device();
    void CollectDataProcess();
    TekDMM6500ParameterItem devPara;//当前要查询的加工项参数的索引
//    TekDMM6500ParameterItem characteristic;
    JudgeMethod devTest;//当前要查询的测试项参数的索引
    int getTestItemInfo();//获取当前测试项参数的索引
    int getTestItemCode(int index);
    ServerProductInfo spi;
    string getDevParaStr();//获取加工项参数的内容
    void CollectCurrent(int workTimes);
    void CollectMeasData();


    void processEndFlag();
    vector<string> m_PartSeqNoVector;
    vector<string> m_PartIdVector;
    vector<string> m_JR;
    int processD3Line(string partNoId, string partSeqNo);//D线处理
    int processTruckLine(string partNoId, string partSeqNo);//卡车线处理
    void getJRVaule();
    void startJudge();

//    string getParameterNoInfo();
    void sendValueToTcpServer(string cValue);
    void CommunicateTest();
    void clearData(string tagName);
    void setTestPara(int workTimes);

};

#endif // THREADTEKDMM6500_H
