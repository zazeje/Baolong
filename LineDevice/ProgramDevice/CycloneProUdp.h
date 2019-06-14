#ifndef CYCLONEPROUDP_H
#define CYCLONEPROUDP_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string>
#include <vector>
#include "DataConvert/DataType.h"
#include "LineDevice/Device/UdpDevice.h"
#include "DataHelper/LogHelper.h"
#include "DataHelper/LogFile.h"

class CycloneProUdp :public UdpDevice
{

public:

    CycloneProUdp();
    CycloneProUdp(int port,string ip);
    CycloneProUdp(int port,string ip,string name);

    void test();

    bool CanAcess();
    bool StartProgram();
    bool IsInWorking();
    bool WriteProgramAndSetId(string address, string id);
    bool WriteId(string address, string id);
    // 删除所有程序文件
    bool DeleteAllSap();
    // 添加Sap文件
    bool AddSapFileByName(string filePath);
    // 重启烧程器，返回帧只有一帧
    bool Reset();
    string m_NameSAP;

    string SendAndCheck(string command, string result, long timeout);
    string WriteProgramAndGetId();
    string GetError();
    // 根据地址读ID
    string GetId(string addressString);
    //取得芯片ID，自动选择一个地址找ID
    string GetId();

private:

    int _commandNo;
    int _d1f5Count;
    int _dfb2Count;

    int _Port;
    string _Ip;
    string _Name;

    bool m_isProgramming;

    // 根据指令内容取完整指令及指令校验序号,返回字符串数组，元素1：完整指令帧，元素2：返回帧1帧头，元素3：返回帧2帧头
    vector<string> GetCommand(string commandContent);
    // 添加Sap文件，添加后烧程器中仅存在一个SAP文件
    bool AddSapFile(string fileContent, long timeout);

};

#endif // CYCLONEPROUDP_H
