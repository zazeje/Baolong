#ifndef TCPWORKTHREAD_H
#define TCPWORKTHREAD_H
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "json/json.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <LineModel/ConfigLoader.h>
#include "LineDevice/Device/DeviceThread.h"
#include "DataHelper/LogHelper.h"
#include "DataHelper/MemoryDBHelper.h"
#include "LineModel/ParseLineInfo.h"
#include <vector>
#include "LineModel/DeviceConfig.h"
#include "LineDriver/DeviceDriver.h"
#include "LineDriver/LogicalDriver.h"
#include "LineDriver/AlarmDriver.h"
#include "DataHelper/LogFile.h"
#include "DataHelper/LocalLineInfo.h"

enum WorkMode
{
    NetworkMode=0,
    LocalMode,
};

enum DriverSource
{
    byNetwork,
    byLocal,
};

class TcpWorkThread
{
private:
    static LogicalDriver* m_logicalDriver;
    static AlarmDriver* m_alarmDriver;
    static DeviceDriver* m_deviceDriver;
    static int m_startFlag;
public:
    static string m_manualdispatchNoNotify;
    static string m_manualpartNoNotify;
    static string m_manualprojectNoNotify;
    static bool m_driverStartFlag;
    static bool m_driverStopOkFlag;
    static bool m_ShowDeviceListFlag;

    pthread_t m_workpth;
    StartCmdInfo m_currstartCmdinfo;
    StartCmdInfo m_prevstartCmdinfo;
    pthread_attr_t m_workpth_attr;
    bool m_WorkModeChangedFlag;           //工作模式改变标志，为true表示工作模式改变，网络模式与本地模式切换算工作模式改变，本地启停不算工作模式改变。
    bool m_manualStartNotifyFlag;         //本地模式手动启动或停止通知标志，通知手动启动为true，否则为false。
    int  m_lastWorkModeNotify;              //用来记录上一个循环周期的PBOX的工作模式。
    int  m_startCmdIndexshadow;
    enum WorkMode m_WorkModeFlag;                    //WorkModeFlag:为0表示使用网络模式，使用上位机启动生产；为1表示使用本地模式，手动启动生产。
    string m_WorkCenterNoshadow ;

    TcpWorkThread();
    bool Start();
    void WorkThreadInit();
    void checkWorkModeState();
    void saveLineInfotoFile();
    void setLineInfoField(string projectno,string dispatchno,string partno,string username,string operationno);
    void StartDeviceDriver(enum DriverSource startsourceindex,int workmodenotifyshadow,string partno,string dispatchno,string projectno,string user,string operationno);
    void StopDeviceDriver(enum DriverSource stopsourceindex,int workmodenotifyshadow,string partno);
    void DealWithNetworkStartStop(enum DriverSource startsourceindex);
    void DealWithLocalStartStop(enum DriverSource startsourceindex);
    static void *Work_Thread(void *arg);
    void threadprocess();
    void fetchSapFile();
    int GetSapFilePath(LineInfo li,string &path);
    static bool GetStartFlag();

};

#endif // TCPWORKTHREAD_H
