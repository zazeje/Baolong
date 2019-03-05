#ifndef MAIN_H
#define MAIN_H

#include <QApplication>
#include <signal.h>
#include <iomanip>
#include "PBOXUI/mainwidget.h"
#include "LineModel/ConfigLoader.h"
#include "LineDriver/DeviceDriver.h"
#include "LineDevice/Device/ModBus.h"
#include "LineDevice/PlcDevice/DagongPlc.h"
#include "LineDevice/ProgramDevice/CycloneProUdp.h"
#include "LineDevice/MarkerDevice/LaserMarker.h"
#include "LineDevice/ProgramDevice/CycloneProgram.h"
#include "LineDevice/MultimeterDevice/AgilentMultimeterTcp.h"
#include "LineDevice/DeviceThreads/ThreadAgilent.h"
#include "DataCommunication/TcpServer.h"
#include "DataCommunication/TcpWorkThread.h"
#include "LineDevice/ScannerDevice/KeyenceSR.h"
#include "LineDevice/ScannerDevice/DataManSR.h"
#include "LineDevice/PlcDevice/FxPlc.h"
#include "LineDevice/MultimeterDevice/AgilentMultimeterSerial.h"
#include "LineDriver/LogicalDriver.h"
#include "LineDevice/DigitalmeterDevice/XSE6.h"
#include "LineDevice/DeviceThreads/ThreadXse6.h"
#include "LineDevice/DeviceThreads/ThreadDgPlc.h"
#include "LineDevice/PlcDevice/DagongPlc.h"
#include "LineDevice/DeviceThreads/ThreadFxPlc.h"
#include "DataHelper/MemoryDBHelper.h"
#include "DataHelper/LogHelper.h"
#include "LineDevice/DigitalmeterDevice/SwpC80.h"
#include "LineDevice/DigitalmeterDevice/XSE6.h"
#include "LineDevice/MultimeterDevice/AgilentMultimeterSerial.h"
#include "LineDevice/MultimeterDevice/GwinstekMultimeter.h"
#include "LineDevice/PowerSupply/ItechPower.h"
#include "LineDevice/ViewCheckDevice/ViewCheck.h"
#include "LineDevice/TpmsHandleDevice/TpmsHandle.h"
#include "LineDevice/SpectrumAnalyzerDevice/Tektronix.h"
#include "TimeHandler/Timer.h"
#include "PBOXUI/PlcInfo/PlcControl.h"
#include "DataHelper/ParseCsv.h"
#include "PBOXUI/PlcInfo/PlcDisplay.h"
#include "PBOXUI/PlcInfo/PlcState.h"
#include "PBOXUI/UsrChoose.h"
#include "LineModel/LineInformation.h"
#include "LineModel/ParseLineInfo.h"
#include "LineDevice/DeviceThreads/ThreadTpmsHandle.h"
#include "LineDevice/PowerSupply/ItechPower.h"
#include "LineDevice/TpmsHandleDevice/TpmsHandle.h"
#include "LineDevice/DigitalmeterDevice/Xse6Serial.h"
#include "LineDevice/DigitalmeterDevice/Micrometer.h"
#include "LineDevice/ViewCheckDevice/ViewCheck.h"
#include "LineDevice/ViewCheckDevice/MarkViewCheck.h"
#include "LineDevice/PlcDevice/Fx5U.h"
#include "LineDevice/DeviceThreads/ThreadViewCheck.h"
#include "DataHelper/DatabaseInfo.h"
#include "DataHelper/LogFile.h"
#include "DataHelper/WatchDogHelper.h"
#include "LineDevice/DigitalmeterDevice/MicrocaliperTcp.h"
#include "LineDevice/DigitalmeterDevice/MicrometerTcp.h"
#include "LineDevice/PowerSupply/ItechPowerTcp.h"

//#define _D2Line
//#define _Debug
//#define _TruckLine
//#define _MyIP
#ifdef _TruckLine
#define LineName "-卡车线"
#elif _D2Line
#define LineName "-D2生产线"
#else
#define LineName "-D4生产线"
#endif
#define EPSINON 0.00001
#define ArrayBuffLen 60
extern string mainVersion;
extern string subVersion;
extern LineInfo gLine;
extern string dispatchNo;                           //派工单号
extern string partNo;                               //件号
extern string operationNo;                          //工序号
extern string user;                                 //用户名
extern string currentTime;                          //系统时间
extern string addressPC;                            //上位机IP地址
extern string productNumber;                        //产品个数
extern string projectNo;                            //产品型号
extern string workCenterNo;                         //工作中心編号
extern string currEditPartNo;                       //本地模式界面当前正在编辑的件号

extern string gFullPathSAP;           //Sap文件在文件服务器的文件路径
extern string gLocalPathSAP;           //Sap文件在PBOX上的文件路径

extern int lastSocket;                              //上位机最新连接的socket网口
extern int productnum;                              //当前机台能一次同时测试的产品数量
extern int sockAry[ArrayBuffLen];                             //上位机客户端conect的socket缓存数组
extern StartCmdInfo startCmdInfoBuff[ArrayBuffLen];           //start指令的启动参数结构体数组
extern int sockIndex;                               //上位机客户端conect的socket数组的最新socket索引值
extern int startCmdIndex;                           //start指令的启动参数结构体数组的最新索引值
extern int exitFlag;                                //通知daemonthread退出的标志
extern int onlydispatchChanged;                     //件号未切换（不需要重启设备驱动），但派工单号被切换，用于通知界面单独刷新派工单号。为0未改变，为1已改变
extern int WorkModeNotify;                          //WorkModeNotify:为0表示通知使用网络模式，使用上位机启动生产；
                                                    //为1表示通知使用本地模式，当前状态为停止生产状态。
                                                    //为2表示通知使用本地模式，当前状态为启动生产状态。
extern int WorkModeNotifyshadow;                    //WorkModeNotifyshadow:用于WorkModeNotify的值在线程间交换变量，0表示当前切换到了网络模式；
                                                    //为1表示当前切换到了本地模式，当前状态为停止生产状态。
                                                    //为2表示当前切换到了本地模式，当前状态为启动生产状态。
extern int EnableSampleMode;                        //EnableSampleMode:为0表示非样件模式；为1表示样件模式
extern pthread_rwlock_t rwlockPartNoDispatchNo;     //件号与派工单号读写锁，用于在本地生产启动界面线程与开启本地生产线程中同步件号与派工单号字符串；
extern pthread_rwlock_t rwlockpcStartCmdParameter;  //上位机start命令参数同步的读写锁。
extern pthread_mutex_t mutexPcConnectSocket;        //用于上位机连接TCP服务器的Socket同步的互斥锁。
extern pthread_mutex_t mutexDeviceDriverCond;       //用于设备驱动运行条件判断互斥锁。
extern pthread_cond_t cond;                         //用于同步设备驱动启停的线程条件。
extern TcpServer *server;
extern TcpWorkThread *workthread;

void *StartTcpServer(void *);
void *StartWatchDog(void* arg);
bool initSignal();
bool initCsv();
bool initTcpServer();
bool initSocketThread();
bool initWorkThread();
void initWatchDog();
void devTest();
void localTest();
string GetPBoxIp();
void initStartCmdBuffer();



#endif // MAIN_H
