#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "json/json.h"
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include "DataHelper/LogHelper.h"
#include "LineModel/ParseLineInfo.h"
#include "DataHelper/LogFile.h"

#define updateSoftShell "sh /home/root/App/shell/updatesoft.sh "
#define updateConfigShell "sh /home/root/App/shell/updatexml.sh "
#define exportLogShell "sh /home/root/App/shell/exportlog.sh "

#define updateSoftRespond "<Respond {\"Command\":\"UpdateSoft\",\"State\":\""
#define updateConfigRespond "<Respond {\"Command\":\"UpdateConfig\",\"State\":\""
#define exportLogRespond "<Respond {\"Command\":\"ExportLog\",\"State\":\""

#define exportSingleLogCmdOuttime   8
#define updateConfigCmdOuttime      25
#define updateSoftCmdOuttime        30

#define createShellThreadErr        -1
#define longTimeCmdBusy             -2
#define logFileNoExist              -3


enum longTimeCmd       //长时指令枚举变量
{
    NoLongTimeCmd = 0,
    UpdateSoft,                 //升级PBOX程序指令
    UpdateConfig,                 //更新xml配置文件指令
    ExportLog                     //导出日志指令
};

class DeviceDriver;
class AlarmDriver;
class LogicalDriver;

using namespace std;

class TcpServer
{
public:

    TcpServer(int port, string ip);
    TcpServer();

    struct sockaddr_in server_address;
    struct sockaddr_in client_addr;         /* 客户地址信息 */

    bool SocketInit(int port,string ip);
    bool SocketAccept();
    bool StartAccept();
    bool StartSocketRecvSend();

    int getTestItemCodeNum();

    static void *Accept_Thread(void *arg);
    static void *RecvSend_Thread(void *arg);
    static void *Shell_Thread(void *);
    void acceptProcess();
    void recvsendProcess();
    static void setPBOXSysTime(string);

private:
    int server;
    pthread_attr_t m_SocketAccept_attr;
    pthread_t m_pSocketAccept;
    pthread_attr_t m_SocketRecvSend_attr;
    pthread_t m_pSocketRecvSend;

    int m_pcSocket;
    int m_oldpcSocket;
    int m_out_time;
    bool m_sendPCStatus;                //是否想PC端发送生产测试状况的标志变量
    bool m_oldsendPCStatus;
    int m_ticks;                        //用于记录循环次数，每3次一个周期在向上发送生产测试状况信息
    int m_logstartticks;
    enum longTimeCmd m_currlongTimeCmd;
    string m_shellcmd;                  //sendthread线程使用用于命令日志打印
    int m_count;
    int m_sendErrTimes;
    int m_productnum;

    static bool sendToPC;

    string getWorkCenterNo();
    bool getNewSocketID();
    void getJsonCommand(string buff);
    void startShellThread(enum longTimeCmd currcmdindex,string shellcmd,int shellouttime,string &respondStr);

    struct timeval m_tv_start;
    struct timeval m_tv_current;
    pthread_t m_shellpth;
    pthread_attr_t m_shellpth_attr;
    static int m_shellout_time;
    static string m_shellcmdshadow;            //m_shellcmdshadow保持与m_shellcmd变量一致，shellthread线程使用用于命令执行。
    void parseJsonCommand(string command);
    string m_respond;
    void sendRegisterRespondToPC();
    void parseStartCommand(Json::Value root);
    void parseStopCommand();
    void parseRebootCommand();
    void parseUpdateSoftCommand();
    void parseUpdateConfigCommand();
    void parseExportLogCommand(int startpos,int copynum);
    void parseVersonMd5Command(int state);
    void parseChangeWorkMode(int modeIndex);
    void dealWithShellResult(int result);
    void checkShellResult();
    void sendMessagetoTcpServer();
    void sendMessage(int num);
    string getsCollectValue(int pos);
};




#endif // TCPSERVER_H
