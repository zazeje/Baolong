#ifndef UDPDEVICE_H
#define UDPDEVICE_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include "DataConvert/DataType.h"

using namespace std;

#define _bufferSize 10240

class UdpDevice
{
public:

    UdpDevice();
    UdpDevice(int port ,string ip);
    UdpDevice(int port ,string ip ,string name);
    virtual ~UdpDevice();

    bool SocketInit(int port, string ip);
    void Open();
    void Close();
    virtual bool CanAcess() = 0;
    string Name;

private:

    int Socket_fd;
    string IPaddr;
    bool isNeedConnect;
    int Port;
    struct sockaddr_in address;//处理网络通信的地址
    bool isConnected;
    string ReadBytes();         //从端口缓冲区读取所有数据
    string ReadHex();           //从端口缓冲区读取16进制字符串
    bool WriteHex(string buf);  //向端口写入十六进制格式字符串命令
    bool WriteAscii(string buf);//向端口口写入ASCII码字符串格式命令
    string WriteAndReadAscii(string command, int delay);//向端口口写入ASCII码字符串格式命令，间隔一段时间后读取端口缓冲区的所有数据
    string WriteAndReadHex(string command , int delay);//向端口写入16进制格式命令，间隔一段时间后读取端口缓冲区的所有数据

protected:

    //受限方法
    bool Write(string command, string CharType);//向端口写入指令
    string Read(string CharType);               //读端口缓冲区所有数据
    string WriteAndRead(string command, string CharType, int delay);//向端口写入字符串命令后，间隔一段时间后读取端口缓冲区的所有数据
    string WriteAsciiAndReadHex(string command, int delay);//向端口写入字符串命令（ASCII字符串）后，间隔一段时间后读取端口缓冲区的所有数据（十六进制字符串格式）
    string WriteHexAndReadAscii(string command, int delay);//向端口写入字符串命令（十六进制字符串）后，间隔一段时间后读取端口缓冲区的所有数据（ASCII字符串格式）
    bool tryConnect;
};


#endif // UDPDEVICE_H
