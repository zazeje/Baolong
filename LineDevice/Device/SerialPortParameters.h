#ifndef SERIALPORPARAMETERS_H
#define SERIALPORPARAMETERS_H

#include "LineModel/DeviceConfig.h"
#include "DataConvert/DataType.h"
#include <stdlib.h>

class SerialPortParameters
{
public:
    // 端口名称
    string _portName;
    // 比特率
    int _baudRate;
    // 数据位
    int _dataBits;
    // 停止位
    int _stopBits;
    // 奇偶校验位
    int _parity;

public:
    SerialPortParameters();
    SerialPortParameters(string protName);
    SerialPortParameters(string portName,int baudRate,int dataBits,int stopBits,int parity);
    SerialPortParameters(string portName,string baudRate,string dataBits,string stopBits,string parity);


    static SerialPortParameters* GetSpp(DeviceInfo di);

};



#endif //SERIALPORPARAMETERS_H
