#ifndef ALARMDRIVER_H
#define ALARMDRIVER_H

#include "DataCommunication/TcpServer.h"
#include "DataHelper/LogFile.h"
#include "DataHelper/ParseCsv.h"
#include "DataHelper/MySQLHelper.h"

using namespace std;

//class ThreadFxPlc;

class AlarmInfo
{
public:
    string Dispatch_No;                 //派工单号
    string Part_No;                     //件号
    string Production_Line;             //产品线
    string Work_Center_No;              //工作中心
    string Alarm_Level;                 //报警等级
    string Alarm_Content;               //报警内容
    string Alarm_Category;              //报警目录
    string Alarm_Processing_Method;     //报警方式
    string Enter_User;                  //用户名
    string MCH_Code;                    //设备编码
};

class AlarmDriver
{
public:
    AlarmDriver();
    ~AlarmDriver();

    bool Start();
    bool Stop();
    void threadprocess();
    static void *Start_Thread(void*arg);

    bool saveAlarmInfoToSql(AlarmInfo &ai);         //插入报警内容到数据库
    void initSqlPara(AlarmInfo &ai);                //初始化数据库参数
    void communicateAlarm(AlarmInfo &comAi);        //设备通信报警
    void plcPointAlarm(AlarmInfo &plcAi);           //PLC点位报警
    void tcpServerAlarm();                          //上位机报警
    void deviceParaAlarm(AlarmInfo &paraAi);        //设备参数报警
    void deviceInitAlarm(AlarmInfo &devAi);         //设备初始化报警

    void initPara();
    void initCommPara();                            //初始化通信报警参数
    void initPlcPara();                             //初始化PLC报警参数
    void initDevicePara();                          //初始化设备报警参数
    void initDeviceError();                              //初始化设备初始化报警参数

    void checkDevPara(AlarmInfo &paraAi, DeviceInfo &di);
    void checkDevTestPara(AlarmInfo &paraAi, DeviceInfo &di);

private:
    pthread_t pth;
    bool _stop;
    bool alarmFlag;    
    vector<string> m_preConnectStatus;      //上一次设备通信状态数组
    vector<int> m_prePLcAlarmValue;         //上一次PLC报警点数组
    int _counter;                           //设备数量
    string m_plcCommOK;                     //报警恢复点
    string m_plcCommERR;                    //报警点
    int m_startAddress;                     //起始地址
    vector<string> m_deviceAlarmName;       //设备报警名
    bool m_devFlag;
    bool m_testFlag;
    bool m_initFlag;

};

#endif // ALARMDRIVER_H
