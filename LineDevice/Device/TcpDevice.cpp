#include "TcpDevice.h"


/**
 * @brief 构造函数
 * @param port
 * @param ip
 */
TcpDevice::TcpDevice(int port ,string ip)
{
    isReConnect = false;
    stopRecon = false;
    Port = port;
    IPaddr = ip;
    SocketInit(port,ip);
    Connect();
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGPIPE, &sa, 0);
    flag = false;
    isStopflag = false;
    endStr = "\r\n";
    m_communicateStatus = true;
}

/**
 * @brief 构造函数
 * @param port
 * @param ip
 * @param name
 */
TcpDevice::TcpDevice(int port ,string ip ,string name)
{
    isReConnect = false;
    stopRecon = false;
    Name = name;    
    Port = port;
    IPaddr = ip;
    SocketInit(port,ip);
    Connect();
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGPIPE, &sa, 0);
    isStopflag = false;
    endStr = "\r\n";
    m_communicateStatus = true;
}

TcpDevice::~TcpDevice()
{
    close(Socket_fd);
    isStopflag = true;
}

/**
 * @brief 初始化socket套接字
 * @param port
 * @param ip
 * @return
 */
bool TcpDevice::SocketInit(int port,string ip)
{
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip.data());
    address.sin_port = htons(port);

    //创建一个 TCP socket
    if((Socket_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)  //IPV4  SOCK_STREAM 数据报套接字（TCP协议）
    {
        printf("Creat Socket Error \n");
        return false;
    }

    struct timeval timeout;
    timeout.tv_sec = 2;//2s
    timeout.tv_usec = 0;
    setsockopt(Socket_fd,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));
    setsockopt(Socket_fd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
    return true;
}

/**
 * @brief socket连接
 * @return
 */
bool TcpDevice::Connect()
{   
    if(Socket_fd != -1)
    {
        int res = -1;
        if((res = connect(Socket_fd,(const sockaddr*)(&address),sizeof(address))) == 0)
        {
            _log.LOG_DEBUG("TcpDevice 【%s】 端口 【%s】:【%d】 已连接，不需要重新连接！", Name.data(), IPaddr.data(), Port);
            return true;
        }
        else
        {
            _log.LOG_ERROR("TcpDevice 【%s】 端口 【%s】 : 【%d】 连接失败，请重新连接！", Name.data(), IPaddr.data(), Port);
            return false;
        }
    }
}

/**
 * @brief 关闭socket连接
 */
void TcpDevice::Close()
{
    TcpSocketClose();
}

/**
 * @brief 虚函数，默认初始化函数为空，不需要对下位机测试设备初始化，如果特例，如安捷伦表断线重连需要重新初始化设备，请在子类继承该函数
 */
bool TcpDevice::InitDevice()
{
//    _log.LOG_DEBUG("TcpDevice InitDevice");
}

/**
 * @brief 关闭socket连接
 */
void TcpDevice::TcpSocketClose()
{
    if (Socket_fd != -1)
    {
        close(Socket_fd);
        //_log.LOG_DEBUG("TcpDevice 设备【%s】 端口关闭成功！",Name.data());
    }
}

/**
 * @brief TcpDevice::CloseReconThread
 */
void TcpDevice::CloseReconThread()
{
    isStopflag = true;
    stopRecon = true;
}

/**
 * @brief 断线重连线程处理函数
 * @return
 */
bool TcpDevice::threadprocess()
{
    while(!stopRecon)
    {
//        _log.LOG_DEBUG("TcpDevice 设备【%s】 正在执行断线重连...", Name.data());
        TcpSocketClose();
        if(!isStopflag)
        {
            SocketInit(Port,IPaddr);
            if(Connect())
            {
                InitDevice();
                stopRecon = true;
    //            _log.LOG_DEBUG("TcpDevice 设备【%s】 Thread 断线重连操作成功！ ",Name.data());
                m_communicateStatus = true;
                if(isStopflag)
                {
                    TcpSocketClose();
                }
                break;
            }
            else
            {
                if(isStopflag)
                {
                    TcpSocketClose();
                    break;
                }
            }
            sleep(2);
        }
        else
        {
            break;
        }
    }
    isReConnect = false;
    stopRecon = false;
//    _log.LOG_DEBUG("TcpDevice 设备【%s】 重连线程已退出",Name.data());
    pthread_detach(pth);
    pthread_cancel(pth);
    return true;
}

/**
 * @brief 调用断线重连线程处理函数
 * @param arg
 * @return
 */
void *TcpDevice::Start_Thread(void* arg)
{
    TcpDevice *th = (TcpDevice*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief 启动断线重连线程
 * @return
 */
bool TcpDevice::Restart_Tcp()
{
    if(!isStopflag)
    {
        if(!isReConnect)
        {
    //        _log.LOG_DEBUG("TcpDevice 设备【%s】 已启动重连线程 .... ",Name.data());
            isReConnect = true;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
            int ret = pthread_create(&pth,&attr,TcpDevice::Start_Thread,this);
            if(ret != 0)
            {
                _log.LOG_ERROR("TcpDevice 设备【%s】 重连线程创建【失败】",Name.data());
                isReConnect = false;
                return false;
            }
            else
                return true;
        }
    }
    return true;
}

/**
 * @brief 关闭线程
 * @return
 */
bool TcpDevice::Stop()
{
    pthread_detach(pth);
    pthread_cancel(pth);
    return true;
}

/**
 * @brief 获取socket通信句柄
 * @return
 */
int TcpDevice::GetSocketFd()
{
    return Socket_fd;
}

/**
 * @brief 设置socket套接字
 * @param m_fd
 */
void TcpDevice::SetSocketFd(int m_fd)
{
    Socket_fd = m_fd;
}

/**
 * @brief 以16进制方式读取数据
 * @return
 */
string TcpDevice::ReadHex()
{
    unsigned char tmp_buf[_bufferSize] = {0};
    char m_buff[_bufferSize] = {0};
    string buff = "";
    int len = 0,i = 0;
    int offset=0;
    if(!isReConnect)
    {
        len = recv(Socket_fd,tmp_buf,sizeof(tmp_buf),0);
        //接收客户端的数据
        if(len < 0)
        {
            _log.LOG_ERROR("TcpDevice 设备【%s】 向TCP端口ReadHex【失败】",Name.data());
            if(!m_communicateStatus)
                Restart_Tcp();
            return "";
        }
        for(i=0;i < len;i++)
        {
            if(i*2<_bufferSize-1)
                offset += sprintf(m_buff + offset,"%02X",tmp_buf[i]);
        }
        buff = m_buff;
    }
    return buff;
}


/**
 * @brief 从端口缓冲区读取所有数据(1024字节)
 * @return
 */
char* TcpDevice::ReadBytes()
{
    char buffer[_bufferSize] = {0};
    int res = 0;
    if(!isReConnect)
    {
        res = recv(Socket_fd,buffer,sizeof(buffer),0);
        if (res < 0)
        {
            _log.LOG_ERROR("TcpDevice 设备【%s】 向TCP端口ReadBytes【失败】",Name.data());
            if(!m_communicateStatus)
                Restart_Tcp();
            return "";
        }
    }
    return buffer;
}

/**
 * @brief 向端口口写入ASCII码字符串格式命令
 * @param 指令
 * @return
 */
bool TcpDevice::WriteAscii(string command)
{
    int res = 0;
    if(!isReConnect)
    {
        res = send(Socket_fd,command.data(),command.length(),0);
        if(res < 0)
        {
            _log.LOG_ERROR("TcpDevice 设备【%s】 向TCP端口WriteAscii【失败】",Name.data());
            if(!m_communicateStatus)
                Restart_Tcp();
            return false;
        }
        return true;
    }
    return false;
}

/**
 * @brief 向端口写入十六进制格式字符串命令
 * @param command
 * @return
 */
bool TcpDevice::WriteHex(string command)
{
    unsigned char buffer[_bufferSize] = {0};
    stringtodec(command,buffer);
    int res;
    if(!isReConnect)
    {
        res = send(Socket_fd,buffer,command.length()/2,0);
        if(res < 0)
        {
            _log.LOG_ERROR("TcpDevice 设备【%s】 向TCP端口WriteHex【失败】",Name.data());
            if(!m_communicateStatus)
                Restart_Tcp();
            return false;
        }
        return true;
    }
    return false;
}

/**
 * @brief 向端口口写入ASCII码字符串格式命令，间隔一段时间后读取端口缓冲区的所有数据
 * @param command 指令
 * @param delay 延时时间 单位：微秒
 * @return
 */
string TcpDevice::WriteAndReadAscii(string command, unsigned long delay)
{
    WriteAscii(command);
    usleep(delay * 1000);
    string resultBytes = ReadBytes();
    if (!resultBytes.empty() && resultBytes.length() > 0)
    {
        return resultBytes;
    }
    return "";
}

/**
 * @brief 向端口写入16进制格式命令，间隔一段时间后读取端口缓冲区的所有数据
 * @param command 写入指令
 * @param delay 延时时间
 * @return
 */
string TcpDevice::WriteAndReadHex(string command, unsigned long delay)
{
    WriteHex(command);
    usleep(delay * 1000);
    return ReadHex();
}

/**
 * 受限方法
 * @brief 向端口写入指令
 * @param command 写入的指令
 * @param charType 写入的方式
 * @return
 */
bool TcpDevice::Write(string command, string charType)
{
    if((!command.empty()) && (command.length()>0))
    {
        if(charType == "Ascii")
        {
            return WriteAscii(command);
        }
        else if(charType == "Hex")
        {
            return WriteHex(command);
        }
    }
    return true;
}

/**
 * @brief 读端口缓冲区所有数据
 * @param charType 读取方式
 * @return
 */
string TcpDevice::Read(string charType)
{
    string result;
    if(charType == "Ascii")
    {
        string resultbuff = ReadBytes();
        if(!resultbuff.empty() && resultbuff.length() > 0)
        {
            result = resultbuff;
        }
    }
    else if(charType == "Hex")
    {
        string resultbuff = ReadHex();
        if(!resultbuff.empty() && resultbuff.length() > 0)
        {
            result = resultbuff;
        }
    }
    return result;
}

/**
 * @brief 向端口写入字符串命令后，间隔一段时间后读取端口缓冲区的所有数据
 * @param command 写入指令
 * @param charType 写入格式
 * @param delay 延时时间
 * @return
 */
string TcpDevice::WriteAndRead(string command, string charType, unsigned long delay)
{
    if((!command.empty()) && (command.length()>0))
     {
         if(charType == "Ascii")
         {
             string resultbuff = WriteAndReadAscii(command,delay);
             if(!resultbuff.empty() && resultbuff.length() > 0)
                return resultbuff;
             else
                return "";
         }
         else if(charType == "Hex")
         {
             string resultbuff = WriteAndReadHex(command,delay);
             if(!resultbuff.empty() && resultbuff.length() > 0)
               return resultbuff;
             else
                return "";
         }
     }
     return "";
}

/**
 * @brief 向端口写入字符串命令（ASCII字符串）后，间隔一段时间后读取端口缓冲区的所有数据（十六进制字符串格式）
 * @param command 写入的指令
 * @param delay 延时时间
 * @return
 */
string TcpDevice::WriteAsciiAndReadHex(string command, unsigned long delay)
{
    if((!command.empty()) && (command.length()>0))
    {
        WriteAscii(command);
        usleep(delay * 1000);
        return ReadHex();
    }
    return "";
}

/**
 * @brief 向端口写入字符串命令（十六进制字符串）后，间隔一段时间后读取端口缓冲区的所有数据（ASCII字符串格式）
 * @param command 写入的指令
 * @param delay 延时时间
 * @return
 */
string TcpDevice::WriteHexAndReadAscii(string command, unsigned long delay)
{
    string result = "";
    if((!command.empty()) && (command.length() > 0))
    {
        WriteHex(command);
        usleep(delay);
        string resultBytes = ReadBytes();
        if (!resultBytes.empty() && resultBytes.length() > 0)
            result = resultBytes;
    }
    return result;
}
