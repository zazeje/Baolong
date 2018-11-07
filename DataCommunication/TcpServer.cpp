#include "TcpServer.h"
#include "../PBOXUI/PBoxConfig.h"
#include "main.h"

bool TcpServer::sendToPC = false;
int TcpServer::m_shellout_time = 0;
string TcpServer::m_shellcmdshadow = "";

/**
 * @brief 无参构造
 */
TcpServer::TcpServer()
{}

/**
 * @brief 含参构造
 */
TcpServer::TcpServer(int port ,string ip): m_pcSocket(0),m_oldpcSocket(0),m_out_time(0),m_sendPCStatus(false)
  ,m_oldsendPCStatus(false),m_ticks(0),m_logstartticks(0),m_currlongTimeCmd(NoLongTimeCmd),m_sendErrTimes(0),m_productnum(0)
{
    SocketInit(port, ip);
}

/**
 * @brief 初始化socket套接字
 * @param 端口号
 * @param ip地址
 * @return 成功返回true，否则返回false
 */
bool TcpServer::SocketInit(int port, string ip)
{
    bzero(&server_address,sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip.data());
    server_address.sin_port = htons(port);

    if((server=socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        _log.LOG_ERROR("TcpServer Creat Socket Error !");
        return false;
    }
    int on = 1;
    if((setsockopt(server,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
    {
        _log.LOG_ERROR("TcpServer Setsockopt Failed !");
        return false;
    }
    if (bind(server, (struct sockaddr *)&server_address, sizeof(struct sockaddr)) == -1)
    {
        _log.LOG_ERROR("TcpServer Bind Error !");
        return false;
    }
    if (listen(server, 10) == -1)
    {
        _log.LOG_ERROR("TcpServer Listen Error !");
        return false;
    }
    _log.LOG_INFO("TcpServer server 【Tcp服务器初始化成功】 ");
    return true;
}

/**
 * @brief TcpServer::StartSocketRecvSend 向上位机接收发送消息线程创建
 * @return  成功返回true,失败返回false.
 */
bool TcpServer::StartSocketRecvSend()
{
    pthread_attr_init(&m_SocketRecvSend_attr);
    //将线程设置为detach模式，线程一关闭则自动释放线程资源
    pthread_attr_setdetachstate(&m_SocketRecvSend_attr,PTHREAD_CREATE_DETACHED);
    if(pthread_create(&m_pSocketRecvSend, &m_SocketRecvSend_attr, TcpServer::RecvSend_Thread, (void*)this)!=0)       //创建子线程
    {
         _log.LOG_ERROR("TcpServer 向上位机接收发送消息线程创建失败!");
         return false;
    }
    return true;
}

/**
 * @brief TcpServer::StartAccept Accept上位机socket连接线程创建
 * @return 成功返回true,失败返回false.
 */
bool TcpServer::StartAccept()
{
    pthread_attr_init(&m_SocketAccept_attr);
    //将线程设置为detach模式，线程一关闭则自动释放线程资源
    pthread_attr_setdetachstate(&m_SocketAccept_attr,PTHREAD_CREATE_DETACHED);
    if(pthread_create(&m_pSocketAccept, &m_SocketAccept_attr, TcpServer::Accept_Thread, (void*)this)!=0)       //创建子线程
    {
         _log.LOG_ERROR("TcpServer Accept上位机socket连接线程创建失败!");
         return false;
    }
    return true;
}

/**
 * @brief TcpServer::acceptProcess Accept上位机socket连接线程处理函数
 */
void TcpServer::acceptProcess()
{
    SocketAccept();
}

/**
 * @brief 接收socket连接
 */
bool TcpServer::SocketAccept()
{
    int client_len;
    int acceptClient = 0;
    lastSocket = 0;
    sockIndex = 0;
    memset(sockAry,0,ArrayBuffLen);
    while(1)
    {
        client_len = sizeof(client_addr);
        acceptClient = accept(server,(struct sockaddr *)&client_addr, (socklen_t *)&client_len);
        if(acceptClient == -1)
        {
             continue;
        }
        _log.LOG_DEBUG("TcpServer Got connection from 【%s】，cport【%d】，acsocket【%d】",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port),acceptClient);

        int flags = fcntl(acceptClient,F_GETFL,0);
        fcntl(acceptClient,F_SETFL,flags | O_NONBLOCK);
        pthread_mutex_lock(&mutexPcConnectSocket);
        sockAry[sockIndex] = acceptClient;                //connect到PBOX的所有Socket缓存区，缓存区目前设定为60
        if(sockIndex == (ArrayBuffLen - 1))                             //达到缓存区顶端59位置，回环到0位置
        {
            sockIndex = 0;
        }
        else
        {
            sockIndex++;
        }
        lastSocket = acceptClient;                      //收到新的Socket后，将老的Socket推入数组缓存，使用新的Socket来替换lastSocket。
        pthread_mutex_unlock(&mutexPcConnectSocket);
        _log.LOG_INFO("收到最新的Socket为【%d】",lastSocket);
        usleep(200 * 1000);
    }
    return false;
}

/**
 * @brief 接收处理上位机指令，循环向上位机发送生产信息线程
 */
void TcpServer::recvsendProcess()
{
    char cbuff[1000] = {0};
    workCenterNo = getWorkCenterNo();
    gettimeofday(&m_tv_start,0);
    while(exitFlag == 0)
    {
        if(getNewSocketID())
        {//返回值为true，收到一个新的socket，主动上传PBOX版本号、PBOX的MD5码、xml文件的MD5码，每个新连接只主动上传一次
//            parseVersonMd5Command(1);
        }
        checkShellResult();
        if(m_pcSocket > 0)
        {
            memset(cbuff,0,sizeof(cbuff));
            if(recv(m_pcSocket,cbuff,sizeof(cbuff),0) > 0)
            {
                string buff;
                buff = cbuff;
                _log.LOG_DEBUG("TcpServer 接收上位机消息【%s】",buff.data());
                getJsonCommand(buff);
            }
            sendMessagetoTcpServer();
        }
        m_ticks++;
        if(m_ticks == 90000000)                           //90000000无特定含义，用于防止m_ticks++后过大，溢出而反转
        {
            m_ticks = 0;
            m_logstartticks = 0;
            _log.LOG_DEBUG("m_ticks 达到90000000，反转为0！");
        }
        usleep(200 * 1000);
    }
    _log.LOG_DEBUG("向上位机发送线程已退出!!!");
//    m_SendPcThreadStartupFlag = false;
}


/**
 * @brief RecvSend_Thread 启动接收处理上位机指令，回复指令并循环向上位机发送生产信息的线程回调函数
 * @param arg
 * @return
 */
void *TcpServer::RecvSend_Thread(void *arg)
{
    ((TcpServer *)arg)->recvsendProcess();
    return NULL;
}

/**
 * @brief TcpServer::Accept_Thread accept socket 连接的线程的回调函数
 * @param arg
 * @return
 */
void *TcpServer::Accept_Thread(void *arg)
{
    ((TcpServer *)arg)->acceptProcess();
    return NULL;
}

/**
 * @brief 设置系统时间
 * @param time
 */
void TcpServer::setPBOXSysTime(string time)
{
    string cmd;
    cmd.append("date -s \"").append(time).append("\"");
    system(cmd.data());
}

/**
 * @brief string TcpServer::getWorkCenterNo() 获取工作中心编号
 * @return string类型，工作中心编号字串
 */
string TcpServer::getWorkCenterNo()
{
    string workCenterNo;
//    string respond;
    workCenterNo = g_dbInfo.GetWorkCenterNo();
//    respond = "<Respond {\"WorkCenter\":\"" + workCenterNo + "\"}>";
    _log.LOG_INFO("TcpServer 向上位机发送消息线程启动,工作中心编号为【%s】",workCenterNo.data());
    return workCenterNo;
}

/**
 * @brief bool TcpServer::getNewSocketID() 响应并反馈导出日志指令
 * @return bool类型，标示是否获得一个新连接。为true，获得了一个新连接，并且新的socket赋值到了m_pcSocket。为false，没有获得了一个新连接，原有m_pcSocket不更改。
 */
bool TcpServer::getNewSocketID()
{
    bool newsocket = false;
    pthread_mutex_lock(&mutexPcConnectSocket);
    if(m_pcSocket != lastSocket)
    {//收到新socket口时，关闭旧socket，并使用最新的socket
        m_oldpcSocket = m_pcSocket;
        m_oldsendPCStatus = m_sendPCStatus;
        for(int i = 0; i < ArrayBuffLen;i++)
        {
            if(sockAry[i] != 0)
            {
                if(sockAry[i] != lastSocket)
                {
                    close(sockAry[i]);
                    sockAry[i] = 0;
                }
            }
        }
        m_pcSocket = lastSocket;
        m_sendPCStatus = false;
        newsocket = true;
    }
    pthread_mutex_unlock(&mutexPcConnectSocket);
    if(newsocket)
    {
        _log.LOG_INFO("关闭所有过期Socket，使用最新Socket【%d】，旧Socket【%d】，m_oldsendPCStatus【%d】，\
m_productnum【%d】，m_logstartticks【%d】，m_ticks【%d】",m_pcSocket,m_oldpcSocket,m_oldsendPCStatus?1:0,
        m_productnum,m_logstartticks,m_ticks);
        m_logstartticks = m_ticks;
    }
    return newsocket;
}

/**
 * @brief 获取上位机json格式的指令
 * @param buff 收到的网络缓冲区字符串
 */
void TcpServer::getJsonCommand(string buff)
{
    string command;
    //上位机发送过来的Json指令以<Request开头，以>结尾
    if(!buff.substr(0,8).compare("<Request"))
    {
       command = buff.substr(9,buff.length()-9);
       command = command.erase(command.find(">"));
       //root保存整个Json对象的value
       parseJsonCommand(command);
    }
}

/**
 * @brief 分解上位机json格式的指令，根据命令进行分类处理
 * @param command 从getJsonCommand函数提取出的指令字串
 */
void TcpServer::parseJsonCommand(string command)
{
    Json::Reader reader;
    Json::Value root;
    if(reader.parse(command,root))
    {
        string name;
        name = root["Command"].asString();
        if(!name.compare("Register"))
        {
            m_sendPCStatus = false;
            usleep(600 * 1000);
            sendRegisterRespondToPC();
        }
        else if(!name.compare("UnRegister"))
        {
            m_sendPCStatus = false;
        }
        else if(!name.compare("Start"))
        {
            m_sendPCStatus = true;
            parseStartCommand(root);

        }
        else if(!name.compare("Reboot"))
        {
            parseRebootCommand();
        }
        else if(!name.compare("GetVersion"))                          //有GetVersion命令被收到
        {
            parseVersonMd5Command(0);
        }
        else if(!name.compare("UpdateSoft"))
        {
            parseUpdateSoftCommand();
        }
        else if(!name.compare("UpdateConfig"))
        {
            parseUpdateConfigCommand();
        }
        else if(!name.compare("ExportLog"))                          //有ExportLog命令被收到
        {
            int startpos = atoi(root["StartPos"].asString().data());
            int copynum = atoi(root["FileNum"].asString().data());
            parseExportLogCommand(startpos,copynum);
        }
        else if(!name.compare("ChangeWorkMode"))
        {
            int modeIndex = atoi(root["WorkMode"].asString().data());
            if(PBoxConfig::m_setWorkMode == -1)
                PBoxConfig::m_setWorkMode = modeIndex;
        }
    }
}

/**
 * @brief TcpServer::parseStopCommand 停止设备驱动指令，目前停止指令不处理
 */
void TcpServer::parseStopCommand()
{
//    if((startCmdIndex == -1) || (startCmdIndex == (ArrayBuffLen - 1)))
//    {
//        startCmdInfoBuff[0].dispatchNo = "";
//        startCmdInfoBuff[0].partNo = "";
//        pthread_mutex_lock(&mutexDeviceDriverCond);
//        startCmdIndex = 0;
//        pthread_mutex_unlock(&mutexDeviceDriverCond);
//    }
//    else
//    {
//        startCmdInfoBuff[startCmdIndex + 1].dispatchNo = "";
//        startCmdInfoBuff[startCmdIndex + 1].partNo = "";
//        pthread_mutex_lock(&mutexDeviceDriverCond);
//        startCmdIndex++;
//        pthread_mutex_unlock(&mutexDeviceDriverCond);
//    }
//    if(WorkModeNotify == 0)
//    {
//        pthread_cond_signal(&cond);                     //通知Workthread,有新的Start命令被收到
//    }
}

/**
 * @brief TcpServer::parseStartCommand
 * @param root中含有start指令的附带相关参数
 */
void TcpServer::parseStartCommand(Json::Value root)
{
    if((startCmdIndex == -1) || (startCmdIndex == (ArrayBuffLen - 1)))
    {
        startCmdInfoBuff[0].projectNo = root["ProjectNo"].asString();
        startCmdInfoBuff[0].dispatchNo = root["DispatchNo"].asString();
        startCmdInfoBuff[0].partNo = root["PartNo"].asString();
        startCmdInfoBuff[0].user = root["User"].asString();
        startCmdInfoBuff[0].operationNo = root["OperationNo"].asString();
        _log.LOG_INFO("Start命令1，pcsocket【%d】，partNo【%s】，dispatchNo【%s】，projectNo【%s】，user【%s】，operationNo【%s】",
                m_pcSocket,startCmdInfoBuff[0].partNo.data(),startCmdInfoBuff[0].dispatchNo.data(),startCmdInfoBuff[0].projectNo.data(),
                startCmdInfoBuff[0].user.data(),startCmdInfoBuff[0].operationNo.data());
        pthread_mutex_lock(&mutexDeviceDriverCond);
        startCmdIndex = 0;
        pthread_mutex_unlock(&mutexDeviceDriverCond);
    }
    else
    {
        startCmdInfoBuff[startCmdIndex + 1].projectNo = root["ProjectNo"].asString();
        startCmdInfoBuff[startCmdIndex + 1].dispatchNo = root["DispatchNo"].asString();
        startCmdInfoBuff[startCmdIndex + 1].partNo = root["PartNo"].asString();
//                                startCmdInfoBuff[startCmdIndex + 1].productNum = atoi(root["ProductNumber"].asString().data());
        startCmdInfoBuff[startCmdIndex + 1].user = root["User"].asString();
        startCmdInfoBuff[startCmdIndex + 1].operationNo = root["OperationNo"].asString();
        _log.LOG_INFO("Start命令2，pcsocket【%d】，partNo【%s】，dispatchNo【%s】，projectNo【%s】，user【%s】，operationNo【%s】",
                m_pcSocket,startCmdInfoBuff[startCmdIndex + 1].partNo.data(),startCmdInfoBuff[startCmdIndex + 1].dispatchNo.data(),
                startCmdInfoBuff[startCmdIndex + 1].projectNo.data(),
                startCmdInfoBuff[startCmdIndex + 1].user.data(),startCmdInfoBuff[startCmdIndex + 1].operationNo.data());
        pthread_mutex_lock(&mutexDeviceDriverCond);
        startCmdIndex++;
        pthread_mutex_unlock(&mutexDeviceDriverCond);
    }
    if(WorkModeNotify == 0)
    {
        pthread_cond_signal(&cond);
    }
}

/**
 * @brief 响应并反馈UpdateSoft升级PBOX程序指令
 */
void TcpServer::parseUpdateSoftCommand()
{
    string strtmp = updateSoftRespond;
    if(m_out_time == 0)
    {
        string cmd = updateSoftShell;
#ifdef _MyIP
        cmd += "192.168.20.49";
#else
        cmd += g_dbInfo.GetHostIP();
#endif
        startShellThread(UpdateSoft,cmd,updateSoftCmdOuttime,strtmp);
    }
    else
    {
        strtmp += IntToString(longTimeCmdBusy) + "\"}>";
    }
    if(send(m_pcSocket,(const char *)strtmp.data(),strtmp.length(),0)==-1)
    {
        _log.LOG_ERROR("TcpServer 回应上位机 UpdateSoft指令消息失败!");
    }
    _log.LOG_DEBUG("TcpServer 回应上位机升级PBOX程序指令【%s】",strtmp.data());
    if(m_out_time > 0)
    {
        usleep(400 * 1000);
    }
}

/**
 * @brief 响应并反馈更新xml指令
 */
void TcpServer::parseUpdateConfigCommand()
{
    string strtmp = updateConfigRespond;
    if(m_out_time == 0)
    {
        string cmd = updateConfigShell;
#ifdef _MyIP
        cmd += "192.168.20.49 ";
#else
        cmd += g_dbInfo.GetHostIP() + " ";
#endif
        cmd += g_dbInfo.GetWorkCenterNo();
        startShellThread(UpdateConfig,cmd,updateConfigCmdOuttime,strtmp);
    }
    else
    {
        strtmp += IntToString(longTimeCmdBusy) + "\"}>";
    }
    if(send(m_pcSocket,(const char *)strtmp.data(),strtmp.length(),0)==-1)
    {
        _log.LOG_ERROR("TcpServer 回应上位机 UpdateConfig指令消息失败!");
    }
    _log.LOG_DEBUG("TcpServer 回应上位机升级PBOX程序指令【%s】",strtmp.data());
    if(m_out_time > 0)
    {
        usleep(400 * 1000);
    }
}

/**
 * @brief 响应并反馈导出日志指令
 * @param startpos 指示要导出的日志文件起始序号。有效范围【0，49】。如从log.3（包含log.3）开始导出日志文件，则本参数指定为3
 * @param copynum  指示要导出的日志文件数量。数量有效范围【1，5】，即使对应序号的文件不存在，依然计入文件数量中。如copynum指定为5，startpos指定为3，即为指定要导出log.3,log.4,log.5,log.6,log.7。
 */
void TcpServer::parseExportLogCommand(int startpos,int copynum)
{
    string strtmp = exportLogRespond;
    if(m_out_time == 0)
    {
        _log.LOG_DEBUG("TcpServer 接收上位机消息startpos【%d】copynum【%d】",startpos,copynum);
        string cmd = exportLogShell;
#ifdef _MyIP
        cmd += "192.168.20.49 ";
#else
        cmd += g_dbInfo.GetHostIP() + " ";
#endif
        cmd += g_dbInfo.GetWorkCenterNo();
        int existnum = 0;
        string filename = " ";
        string filepath = "/home/root/App/Log/";
        if(copynum > 5)
        {
            copynum = 5;
        }
        if(startpos < 0)
        {
            startpos = 0;
        }
        copynum = startpos + copynum;
        if(copynum > 50)
        {
            copynum = 50;
        }
        for(int i = startpos;i < copynum;i++)
        {
            filename = "log.";
            filename += IntToString(i);
            fstream file;
            file.open(filepath + filename,fstream::in);
            if(file.is_open())
            {
                file.close();
                cmd += " ";
                cmd += filename;
                existnum++;
            }
        }
        if(existnum > 0)
        {
            startShellThread(ExportLog,cmd,existnum * exportSingleLogCmdOuttime,strtmp);
        }
        else
        {
            strtmp += IntToString(logFileNoExist) + "\"}>";
        }
    }
    else
    {
        strtmp += IntToString(longTimeCmdBusy) + "\"}>";
    }
    if(send(m_pcSocket,(const char *)strtmp.data(),strtmp.length(),0)==-1)
    {
        _log.LOG_ERROR("TcpServer 回应上位机 ExportLog指令消息失败!");
    }
    _log.LOG_DEBUG("TcpServer 回应上位机导出日志指令【%s】",strtmp.data());
    if(m_out_time > 0)
    {
        usleep(400 * 1000);
    }
}

/**
 * @brief TcpServer::parseRebootCommand 处理PC软件下发的PBOX重启指令
 */
void TcpServer::parseRebootCommand()
{
    string strtmp = "<Respond {\"Command\":\"Reboot\",\"State\":\"0\"}>";
    if(send(m_pcSocket,(const char *)strtmp.data(),strtmp.length(),0)==-1)
    {
        _log.LOG_ERROR("TcpServer 回应上位机重启指令消息失败!");
    }
    _log.LOG_DEBUG("TcpServer 回应上位机重启指令【%s】",strtmp.data());
    system("reboot");
}

/**
 * @brief TcpServer::parseVersonMd5Command 获取PBOX程序版本、PBOX MD5码、Xml文件MD5码并上传
 * @param state 指示被上位机查询后上传(为0)，socket首次连接、执行PBOX升级、执行XML文件升级后主动上传（非0）
 */
void TcpServer::parseVersonMd5Command(int state)
{
    char buf[100] = {0};
    char md5buf[33] = {0};
    FILE *pfile = NULL;
    string pboxstr;
    string xmlstr;
    int lenth = 0;
    memset(buf,0,100);
    memset(md5buf,0,33);
    pfile = popen("md5sum -b /home/root/App/PBOX","r");
    if(pfile != NULL)
    {
        fread(buf, 100, 1, pfile);
        pclose(pfile);
        pfile = NULL;
    }
    pboxstr = md5buf;
    lenth = strlen(buf);
    if(lenth == 54)
    {
        memset(md5buf,0,33);
        for(int i = 32;i < 53;i++)
        {
            md5buf[i - 32] = buf[i];
        }
        pboxstr = md5buf;
        memset(md5buf,0,33);
        if(!pboxstr.compare(" */home/root/App/PBOX"))
        {
            for(int i = 0;i < 32;i++)
            {
                md5buf[i] = buf[i];
            }
        }
        else
        {
            md5buf[0] = ' ';
        }
    }
    else
    {
        md5buf[0] = ' ';
    }
    pboxstr = md5buf;
    memset(buf,0,100);
    memset(md5buf,0,33);
    pfile = popen("md5sum -b /home/root/App/PBOX.xml","r");
    if(pfile != NULL)
    {
        fread(buf, 100, 1, pfile);
        pclose(pfile);
    }
    xmlstr = md5buf;
    lenth = strlen(buf);
    if(lenth == 58)
    {
        memset(md5buf,0,33);
        for(int i = 32;i < 57;i++)
        {
            md5buf[i - 32] = buf[i];
        }
        xmlstr = md5buf;
        memset(md5buf,0,33);
        if(!xmlstr.compare(" */home/root/App/PBOX.xml"))
        {
            for(int i = 0;i < 32;i++)
            {
                md5buf[i] = buf[i];
            }
        }
        else
        {
            md5buf[0] = ' ';
        }
    }
    else
    {
        md5buf[0] = ' ';
    }
    xmlstr = md5buf;
    string strtmp = "<Respond {\"Version\":\"";
    strtmp += mainVersion + "\",\"pboxmd5\":\"" + pboxstr;
    strtmp += "\",\"xmlmd5\":\"" + xmlstr + "\",\"State\":\"" + IntToString(state) + "\"}>";
    if(send(m_pcSocket,(const char *)strtmp.data(),strtmp.length(),0)==-1)
    {
        _log.LOG_ERROR("TcpServer 回应上位机 GetVersion指令消息失败!");
    }
    _log.LOG_DEBUG("TcpServer 回应上位机获取版本指令【%s】",strtmp.data());
}

/**
 * @brief TcpServer::sendRegisterRespondToPC 回复注册指令
 */
void TcpServer::sendRegisterRespondToPC()
{
    string respond;
    respond = "<Respond {\"WorkCenter\":\"" + workCenterNo + "\"}>";
    if(send(m_pcSocket,(const char *)respond.data(),respond.length(),0)==-1)
    {
        _log.LOG_ERROR("TcpServer 向上位机发送消息失败!");
//        continue;
    }
    _log.LOG_DEBUG("TcpServer 回应上位机注册指令【%s】",respond.data());
}

/**
 * @brief TcpServer::sendMessage 发送生产信息
 * @param num：0传送空值，防止上位机以为断线而重起连接，非0收集对应位置的信息上传
 */
void TcpServer::sendMessage(int num)
{
    string cmd;
    if(num == 0)
    {
        cmd = cmd + "<Respond {\"SeqNo\":\"" + "" + "\",\"PartId\":\"" + "" + "\",\"Value\":\"" + "" +\
                "\",\"Message\":\"" + "" + "\",\"Alarm\":\"" + "" + "\",\"Communication\":\"" +\
                "1" + "\",\"Note\":\"" + ""+ "\",\"Position\":\"" + "" + "\"}>";
    }
    else
    {
        string tmp = IntToString(num);
        string sCollectValue = getsCollectValue(num);
        string sId = m_db.Read_TagMValue(tmp + "$" + "ID");
        string sSeqNo = m_db.Read_TagMValue(tmp + "$" + "SQ");
        string sMessage = m_db.Read_TagMValue(tmp + "$" + "MG");
        string sPosition = m_db.Read_TagMValue(tmp + "$" + "PS");
        string sNote = m_db.Read_TagMValue(tmp + "$" + "NT");
        string sAlarmStatus = m_db.Read_TagMValue(gLine.Si.alarmStatus);
        cmd = "<Respond {\"SeqNo\":\"" + sSeqNo + "\",\"PartId\":\"" + sId + "\",\"Value\":\"" + sCollectValue\
                 + "\",\"Message\":\"" + sMessage + "\",\"Alarm\":\""+ sAlarmStatus + "\",\"Communication\":\""\
                 + "1" + "\",\"Note\":\"" + sNote+ "\",\"Position\":\"" + sPosition + "\"}>";
    }
    char buff[1024]={0};
    u2g((char *)cmd.data(),cmd.length(),buff,sizeof(buff));
    int ret = send(m_pcSocket,buff,strlen(buff),0);
    if(ret <= 0)
    {
        _log.LOG_ERROR("TcpServer 向上位机发送消息失败 !");
        m_sendErrTimes++;
    }
    else
    {
        if(((m_ticks - m_logstartticks) < 1000) && ((m_ticks % 4) == 0))
        {
            //每(m_ticks % 3)才进本函数一次，本函数每(m_ticks % 4) == 0才输出，所以相当于每12个tick才输出一次。输出间隔最小2.4秒，最大4.2秒
            //(m_ticks - m_logstartticks) < 1000从接收到新socket开始计数，计数1000以内，输出发送OK信息，新socket1000个tick后停止输出OK信息。
            _log.LOG_INFO("send ok ,Socket为【%d】,m_productnum【%d】,num【%d】",m_pcSocket,m_productnum,num);
        }
        m_sendErrTimes = 0;
    }
    if(m_sendErrTimes >= 10)    //发送消息失败10次则认为与上位机连接断开，关闭该线程
    {
        m_sendPCStatus = false;
    }
}

/**
 * @brief TcpServer::getsCollectValue 获取测试项的收集值
 * @param buff
 * @param pos
 * @return
 */
string TcpServer::getsCollectValue(int pos)
{
    string buff;
    string sCollectValue;
    int index = 0;
    index = testCodeNum;//atoi((const char*)gLine.Si.testCodeNum.data());
    for(int j = 1;j <= index;j++)
    {
       string CollectValue = IntToString(pos) + "$" + "CV" + IntToString(j);
       buff.append(m_db.Read_TagMValue(CollectValue)).append("/");
       if(j == index)
       {
           sCollectValue = buff;
           buff.clear();
       }
    }

    if(sCollectValue.rfind("/")!=string::npos)
    {
        sCollectValue = sCollectValue.substr(0, sCollectValue.rfind("/"));
    }
    return sCollectValue;
}

/**
 * @brief TcpServer::dealWithShellResult 根据长时指令的shell脚本的执行结果，回应上位机并复位相关标志变量
 * @param result 为1没有因超时结束，>1因超时而结束长时指令，并复位相关标志
 */
void TcpServer::dealWithShellResult(int result)
{
//    switch (m_currlongTimeCmd)
//    {
//    case UpdateSoft:
//        {
//            parseVersonMd5Command(result);
//        }
//        break;
//    case UpdateConfig:
//        {
//            parseVersonMd5Command(result);
//        }
//        break;
//    case ExportLog:
//        {
//            string strtmp = "<Respond {\"Command\":\"ExportLog\",\"State\":\"0\"}>";
//            if(send(m_pcSocket,(const char *)strtmp.data(),strtmp.length(),0)==-1)
//            {
//                _log.LOG_ERROR("TcpServer 上传 ExportLog指令结束信号失败!");
//            }
//            _log.LOG_DEBUG("TcpServer 上传导出日志指令结束信号【%s】",strtmp.data());
//        }
//        break;
//    default:
//        break;
//    }
    m_currlongTimeCmd = NoLongTimeCmd;
    m_out_time = 0;
    m_shellout_time = 0;
    _log.LOG_DEBUG("TcpServer shell【%s】 result【%d】 tv_current【%d】，【%d】",m_shellcmd.data(),result,m_tv_current.tv_sec,m_tv_current.tv_usec);
}

/**
 * @brief void TcpServer::checkShellResult 判断长耗时指令的shell脚本执行是否超时指令，如果超时则复位m_shellout_time和m_out_time，从而放开对长耗时指令的锁定。日志记录下来shell脚本是因超时解锁，还是因执行完成解锁。
 */
void TcpServer::checkShellResult()
{
    int total_time = 0;
    if(m_ticks % 5 == 0)
    {
        m_out_time = m_shellout_time;
        if(m_out_time > 0)
        {
            gettimeofday(&m_tv_current,0);
            total_time = (m_tv_current.tv_sec - m_tv_start.tv_sec);
            total_time += (m_tv_current.tv_usec - m_tv_start.tv_usec) / 1000000;    //ms
            if(total_time > m_out_time)
            {//判定shell执行超时
                dealWithShellResult(2);//参数为shell结果码，2标示因执行超时而结束
            }
        }
        else
        {
            if(m_currlongTimeCmd != NoLongTimeCmd)
            {//判定shell属于正常结束,没有因超时结束
                dealWithShellResult(1);//参数为shell结果码，1标示正常结束
            }
        }
    }
}

/**
 * @brief TcpServer::sendMessagetoTcpServer 循环发送生产信息到上位机，其中每600ms发送一次
 */
void TcpServer::sendMessagetoTcpServer()
{
    if(m_sendPCStatus)
    {
        if((m_ticks % 3) == 2)
        {
            m_productnum = productnum;                                           //使用最新的单机台能同时测试的产品数量来作为上传生产测试数据的循环值
            if(m_productnum == 0)
            {
                sendMessage(0);
            }
            else
            {
                for(int i=1;i<=m_productnum; i++)
                {
                    sendMessage(i);
                    if(i != m_productnum)
                    {
                        usleep(50 * 1000);
                    }
                }
            }
        }
    }
    if(((m_ticks - m_logstartticks) < 1000) && ((m_ticks % 10) == 0))
    {
        //本函数每(m_ticks % 10) == 0才输出，所以相当于每10个tick才输出一次。输出间隔最小2.0秒，最大3.5秒
        //(m_ticks - m_logstartticks) < 1000从接收到新socket开始计数，计数1000（最小200秒，最大350秒）以内，输出条件变量信息，新socket1000个tick后停止输出条件变量信息。
        _log.LOG_DEBUG("TcpServer sendMestoPC m_sendPCStatus【%d】 m_productnum【%d】 m_logstartticks【%d】 m_ticks【%d】 m_pcSocket【%d】",m_sendPCStatus?1:0,m_productnum,m_logstartticks,m_ticks,m_pcSocket);
    }
}

/**
 * @brief 用于来执行长耗时操作的shell脚本的线程
 */
void *TcpServer::Shell_Thread(void *)
{
    system(TcpServer::m_shellcmdshadow.data());
    TcpServer::m_shellout_time = 0;
    return NULL;
}

/**
 * @brief TcpServer::startShellThread 创建shell执行线程，启动shell脚本,
 * @param currcmdindex
 * @param shellcmd
 * @param shellouttime
 * @return
 */
void TcpServer::startShellThread(enum longTimeCmd currcmdindex,string shellcmd,int shellouttime,string &respondStr)
{
    m_shellcmd = shellcmd;
    m_shellcmdshadow = shellcmd;
    pthread_attr_init(&m_shellpth_attr);
    pthread_attr_setdetachstate(&m_shellpth_attr,PTHREAD_CREATE_DETACHED);
    if(pthread_create(&m_shellpth,&m_shellpth_attr,TcpServer::Shell_Thread,(void*)server) == -1)
    {
        switch(currcmdindex)
        {
            case UpdateSoft:
            {
                _log.LOG_DEBUG("TcpServer::Shell_Thread UpdateSoft create error");
            }
            break;
            case UpdateConfig:
            {
                _log.LOG_DEBUG("TcpServer::Shell_Thread UpdateConfig create error");
            }
            break;
            case ExportLog:
            {
                _log.LOG_DEBUG("TcpServer::Shell_Thread ExportLog create error");
            }
            break;
            default:
            {
            }
            break;
        }
        respondStr += IntToString(createShellThreadErr) + "\"}>";
    }
    else
    {
        respondStr += IntToString(shellouttime) + "\"}>";
        gettimeofday(&m_tv_start,0);
        m_currlongTimeCmd = currcmdindex;
        m_out_time = shellouttime;
        m_shellout_time = shellouttime;
        _log.LOG_DEBUG("TcpServer shell 【%s】 executed tv_start【%d】，【%d】",shellcmd.data(),m_tv_start.tv_sec,m_tv_start.tv_usec);
    }
}

