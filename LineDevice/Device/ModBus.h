#ifndef MODBUS_H
#define MODBUS_H

#include <stdio.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include "TcpDevice.h"
#include "DataConvert/DataType.h"

using namespace std;
class ModbusDevice : public TcpDevice
{
public:

    ModbusDevice(int,string,string);
    ModbusDevice(int,string,string,unsigned char);

    int _transactionId_F ;
    int _transactionId_S ;
    unsigned char StationNum; 
    unsigned char command[12];

    int GetNewTransactionId();

    unsigned char* ReadCoils(int address,int length);

protected:

    bool WriteCoil(int address,bool data);
    virtual bool CanAcess() = 0;

};

int ByteArray(unsigned char*,int length,unsigned char*);
int TransAddress(int address,int*);

#endif // MODBUS_H
