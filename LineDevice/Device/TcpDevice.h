#ifndef TCPDEVICE_H
#define TCPDEVICE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "DataConvert/DataType.h"
#include <signal.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "DataHelper/LogHelper.h"
#include "DataHelper/LogFile.h"

#define _bufferSize 10240

using namespace std;

class TcpDevice
{
public:

    TcpDevice(int port ,string ip );
    TcpDevice(int port ,string ip , string name);
    virtual ~TcpDevice();

    struct sockaddr_in address;
    bool Connect();
    virtual void Close();
    virtual bool InitDevice();
    void TcpSocketClose();
    bool SocketInit(int port,string ip);
    void test();
    bool threadprocess();
    bool Restart_Tcp();
    bool Stop();
    int GetSocketFd();
    void SetSocketFd(int m_fd);
    void CloseReconThread();
    static void *Start_Thread(void*arg);
    virtual bool CanAcess() = 0;
    string endStr;

protected:

    bool m_communicateStatus;
    //受限方法
    bool Write(string command, string charType);        //向端口写入指令
    string Read(string charType);                       //读端口缓冲区所有数据
    string WriteAndRead(string command, string charType, unsigned long delay = 0);  //向端口写入字符串命令后，间隔一段时间后读取端口缓冲区的所有数据
    string WriteAsciiAndReadHex(string command, unsigned long delay);//向端口写入字符串命令（ASCII字符串）后，间隔一段时间后读取端口缓冲区的所有数据（十六进制字符串格式）
    string WriteHexAndReadAscii(string command, unsigned long delay);//向端口写入字符串命令（十六进制字符串）后，间隔一段时间后读取端口缓冲区的所有数据（ASCII字符串格式）

public:

    string IPaddr;
    string Name;
    int Port;
    int Socket_fd;
    char recvbuff[_bufferSize];
    bool isReConnect;
    bool stopRecon;
    bool flag;
    bool isStopflag;

    pthread_t pth;  
    char* ReadBytes();                  //从端口缓冲区读取所有数据
    string ReadHex();                   //从端口缓冲区读取16进制数据
    char* ReadBytes(int length);        //从端口缓冲区读取指定长度数据
    bool WriteAscii(string command);    //向端口口写入ASCII码字符串格式命令
    bool WriteHex(string command);      //向端口写入十六进制格式字符串命令
    string WriteAndReadAscii(string command, unsigned long delay);//向端口口写入ASCII码字符串格式命令，间隔一段时间后读取端口缓冲区的所有数据
    char* WriteAndReadAscii(char* command, int length, unsigned long delay);//向端口口写入ASCII码字符串格式命令，间隔一段时间后读取端口缓冲区的指定长度数据
    string WriteAndReadHex(string command, unsigned long delay);//向端口写入16进制格式命令，间隔一段时间后读取端口缓冲区的所有数据
    char* WriteAndReadHex(char* command, int length, unsigned long delay);//向端口写入16进制格式命令，间隔一段时间后读取端口缓冲区的指定长度数据

};
#endif // TCPDEVICE_H
