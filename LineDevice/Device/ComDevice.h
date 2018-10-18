#ifndef COMDEVICE_H
#define COMDEVICE_H

#include <termio.h>
#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>     /*标准函数库定义*/
#include <unistd.h>     /*Unix标准函数定义*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>      /*文件控制定义*/
#include <errno.h>      /*错误号定义*/
#include <sys/time.h>
#include "DataConvert/DataType.h"
#include "SerialPortParameters.h"
#include "DataHelper/LogHelper.h"
#include "DataHelper/LogFile.h"

using namespace std;

#define SIZE 1024*1024

enum Parity
{
    none,
    odd,        //奇校验
    even,       //偶校验
};


class ComDevice
{
public:

    ComDevice();
    ComDevice(SerialPortParameters*, bool blockMode = true);
    ComDevice(SerialPortParameters*, string,  bool blockMode = true);
    virtual ~ComDevice();

    int SerialPort_fd;
    string Name;
    string portname;
    bool isReConnect;
    string endStr;
    pthread_t pth;

    void CloseReconThread();
    bool Open(string PortName, bool blockMode = true);
    void Close();
    bool threadprocess();
    static void *Start_Thread(void*arg);
    bool Restart_Serial();
    bool Stop();
    virtual bool CanAcess() = 0;

    string WriteAndRead(string command, string charType, int delay);
    string WriteAsciiAndReadHex(string command, int delay);
    string WriteHexAndReadAscii(string command,int delay);

private:

    bool stopRecon;
    bool isStopflag;

    static int speed_arr[];
    static int baudflag_arr[];

    SerialPortParameters* _spp;

    bool WriteAscii(string command);
    string ReadAscii();
    string WriteAndReadAscii(string command, int delay);
    string WriteAndReadHex(string command, int delay);
    int SerialPortInit(string,int,int,int,int,bool blockMode = true);

    bool setup_port(int fd, int baud, int databits, int parity, int stopbits);
    int speed_to_flag(int speed);

protected:

    bool m_communicateStatus;

    int ReadByte(void* tmp_buff);
    string ReadHex();
    bool WriteHex(string command);
    bool Write(string command, string charType);
    string Read(string charType);

    int GetSerialPortfd();

};

#endif // COMDEVICE_H
