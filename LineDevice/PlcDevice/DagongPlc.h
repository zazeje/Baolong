#ifndef DAGONGPLC_H
#define DAGONGPLC_H

#include "../Device/ModBus.h"

class DagongPlc : public ModbusDevice
{

public :

    DagongPlc(int port,string ip,string name);

    //  读单个线圈
    string Read(int address);
    //  读批量线圈值
    string Read(int address, int length);
    //  写线圈值
    bool Write(int address, string state);
    //  批量写线圈值
    bool Write(int address, int length, string state);
    //  通信检测
    bool CanAcess();

};

#endif // DAGONGPLC_H
