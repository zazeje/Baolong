#include "UdpDevice.h"
#include "DataHelper/LogFile.h"



UdpDevice::UdpDevice()
{}

/**
 * @brief 构造函数
 * @param port
 * @param ip
 */
UdpDevice::UdpDevice(int port, string ip)
{
    isNeedConnect = true;
    isConnected = false;
    SocketInit(port,ip);
    tryConnect = true;
    Port = port;
    IPaddr = ip;
    Open();
}

/**
 * @brief 构造函数
 * @param port
 * @param ip
 * @param name
 */
UdpDevice::UdpDevice(int port, string ip, string name)
{
    isNeedConnect = true;
    isConnected = false;
    SocketInit(port,ip);
    tryConnect = true;
    Name = name;
    Port = port;
    IPaddr = ip;
    Open();
}

/**
 * @brief 析构函数
 */
UdpDevice::~UdpDevice()
{
    close(Socket_fd);
}

/**
 * @brief 初始化socket
 * @param port
 * @param ip
 * @return
 */
bool UdpDevice::SocketInit(int port, string ip)
{
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr (ip.data());;
    address.sin_port = htons(port);

    //创建一个 UDP socket
    //IPV4  SOCK_DGRAM 数据报套接字（UDP协议）
    if((Socket_fd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
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
 * @brief 判断socket套接字状态
 */
void UdpDevice::Open()
{
    if(Socket_fd != -1)
    {
        isConnected = true;
//        printf("%s端口 %s:%d 已连接，不需要重新连接！\n", Name.data(), IPaddr.data(), Port);
        _log.LOG_DEBUG("【%s】 端口 【%s】:【%d】 已连接，不需要重新连接！",Name.data(), IPaddr.data(), Port);
    }
}

/**
 * @brief 关闭socket套接字
 */
void UdpDevice::Close()
{
    isNeedConnect = false;
    isConnected = false;
    if (Socket_fd != -1)
    {
        close(Socket_fd);
        printf("%s端口 %s:%d 关闭成功\n", Name.data(), IPaddr.data(), Port);
    }
}

/**
 * @brief 以16进制方式读取数据
 * @return 16进制字符串
 */
string UdpDevice::ReadHex()
{
    int sin_size;
    unsigned char tmp_buf[1024]={0};
    char m_buff[1024]={0};
    string buff;
    int len,i;
    int offset = 0;

    sin_size = sizeof(struct sockaddr_in)*2;
    /*接收客户端的数据并将其发送给客户端--recvfrom是无连接的*/
    len = recvfrom(Socket_fd,tmp_buf,sizeof(tmp_buf),0,(struct sockaddr *)&address,(socklen_t *)&sin_size);
    if(len < 0)
    {
        printf("ReadHex recvfrom error \n");
        return "";
    }
    //将读取到的每个字节转换为2位16进制数据
    for(i = 0;i < len;i++)
    {
        offset += sprintf(m_buff + offset,"%02X",tmp_buf[i]);
    }
    buff = m_buff;
    return buff;
}

/**
 * @brief 以16进制方式写数据
 * @param 写入的数据(16进制字符串)
 * @return
 */
bool UdpDevice::WriteHex(string buf)
{
    int res;
    unsigned char Hex[1024]={0};

    stringtodec(buf,Hex);

    if((res = sendto(Socket_fd,Hex,buf.length()/2,0,(struct sockaddr *)&address,sizeof(struct sockaddr)))<0)
    {
        printf("WriteHex Sendto Error \n");
        return false;
    }
    return true;
}

/**
 * @brief 从端口缓冲区读取所有数据(1024字节)
 * @return 读取的数据字符串
 */
string UdpDevice::ReadBytes()
{
    char buffer[_bufferSize] = {0};
    int res = 1;
    if ((res = recv(Socket_fd,buffer,sizeof(buffer),0)) < 0)
    {
        printf("ReadBytes Recv Error \n");
        return "";
    }
    return buffer;
}

/**
 * @brief 以ASCII码方式写数据
 * @param 写入的数据
 * @return
 */
bool UdpDevice::WriteAscii(string buf)
{
    int res;
    if((res = sendto(Socket_fd,buf.data(),buf.length(),0,(struct sockaddr *)&address,sizeof(struct sockaddr)))<0)
    {
        printf("WriteAscii Sendto Error \n");
        return false;
    }
    return true;
}

/**
 * @brief 以ASCII码的方式读写
 * @param command 写入的指令
 * @param delay 延时时间
 * @return
 */
string UdpDevice::WriteAndReadAscii(string command, int delay)
{    
    string result;
    WriteHex(command);
    usleep(delay);
    result = ReadBytes();
    if (!result.empty() && result.length() > 0)
        return result;
    return "";
}

/**
 * @brief 以16进制方式读写
 * @param command 写入的指令
 * @param delay 延时时间
 * @return
 */
string UdpDevice::WriteAndReadHex(string command, int delay)
{
    string result;
    WriteHex(command);
    usleep(delay);
    result = ReadHex();
    return result;
}

bool UdpDevice::Write(string command, string CharType)
{
    if(!command.empty())
    {
        if(CharType == "Ascii")
            WriteAscii(command);
        else if(CharType == "Hex")
            WriteHex(command);
    }
    return true;
}

/**
 * @brief 读取数据
 * @param 读取数据的方式
 * @return
 */
string UdpDevice::Read(string CharType)
{
    string result;
    if(CharType == "Ascii")
        result = ReadBytes();
    else if(CharType == "Hex")
        result = ReadHex();
    return result;
}

/**
 * @brief 向端口写入数据然后间隔一段时间读取端口数据
 * @param command 写入的指令
 * @param CharType 读写的方式
 * @param delay 延时时间
 * @return
 */
string UdpDevice::WriteAndRead(string command, string CharType, int delay)
{
    string result;
    if((!command.empty())&&(command.length()>0))
    {
        if(CharType == "Ascii")
            result = WriteAndReadAscii(command,delay);
        else if(CharType == "Hex")
            result = WriteAndReadHex(command,delay);
    }
    return result;
}

/**
 * @brief 以ASCII码写入数据，以16进制方式读取数据
 * @param command 写入的数据指令
 * @param delay 延时时间
 * @return
 */
string UdpDevice::WriteAsciiAndReadHex(string command, int delay)
{    
    string result;
    if((!command.empty())&&(command.length()>0))
    {
        WriteAscii(command);
        usleep(delay);
        result = ReadHex();
    }
    return result;
}

/**
 * @brief 以16进制方式写入数据，以ASCII码方式读取数据
 * @param command 写入的数据
 * @param delay 延时时间
 * @return
 */
string UdpDevice::WriteHexAndReadAscii(string command, int delay)
{
    if((!command.empty())&&(command.length()>0))
    {
        WriteHex(command);
        usleep(delay);        
        string resultBytes = ReadBytes();
        if (!resultBytes.empty() && resultBytes.length()>0)
            return resultBytes;
    }
    return "";
}
