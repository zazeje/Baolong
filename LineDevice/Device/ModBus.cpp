#include "ModBus.h"
#include "./DataConvert/DataType.h"

ModbusDevice::ModbusDevice(int port,string ip,string name,unsigned char num = 1) : TcpDevice(port,ip,name)
{
    StationNum = num;
}

ModbusDevice::ModbusDevice(int port,string ip,string name) : TcpDevice(port,ip,name)
{
}


int ModbusDevice::GetNewTransactionId()
{
    if(_transactionId_S > 255)
    {
        _transactionId_S = 0;
        _transactionId_F += 1;
    }
    _transactionId_S++;
    return 0;
}

/**
 * @brief 读多点
 * @param 起始地址
 * @param 长度
 * @return
 */
unsigned char* ModbusDevice::ReadCoils(int address,int length)
{
    memset(command,0,sizeof(command));
    GetNewTransactionId();
    command[0] = _transactionId_F;
    command[1] = _transactionId_S;
    command[2] = 0;
    command[3] = 0;
    command[4] = 0;
    command[5] = 6;
    command[6] = 1;
    command[7] = 1;
    int data[2] = {0};
    TransAddress(address,data);
    command[8] = data[0];
    command[9] = data[1];
    command[10] = length / 255;
    command[11] = length % 255;
    string hexcommand = ByteArrToHexStr(command,12);
    string Buffer = WriteAndRead(hexcommand,"Hex",100);
    if(Buffer.empty())
    {
        printf("DgPlc writeandread error\n");
        return NULL;
    }
    unsigned char buffer[20] = {0};
    HexStrToByteArr(Buffer,buffer);
    int m_length = buffer[5] - 3;
    unsigned char byte[m_length];
    memset(byte,0,sizeof(byte));
    for(int i = 0;i < m_length;i++)
    {
        byte[i] = buffer[i + 9];
    }
    memset(buffer,0,sizeof(buffer));
    unsigned char respones[m_length*8];
    memset(respones,0,sizeof(respones));
    ByteArray(byte,m_length,respones);
    return respones;
}

/**
 * @brief 写单点数据
 * @param 地址
 * @param 值
 * @return
 */
bool ModbusDevice::WriteCoil(int address,bool w_num)
{
    memset(command,0,sizeof(command));
    GetNewTransactionId();
    command[0] = _transactionId_F;
    command[1] = _transactionId_S;
    command[2] = 0;
    command[3] = 0;
    command[4] = 0;
    command[5] = 6;
    command[6] = 1;
    command[7] = 5;
    int data[2] = {0};
    TransAddress(address,data);
    command[8] = data[0];
    command[9] = data[1];
    command[10] = ((w_num == true) ? 255 : 0);
    command[11] = 0;

    string hexcommand = ByteArrToHexStr(command,12);

    string Buffer = WriteAndRead(hexcommand,"Hex",10);

    if(!Buffer.compare(hexcommand))
    {
        return true;
    }
    else
        return false;
}

int ByteArray(unsigned char *Byte,int length,unsigned char*response)
{
    int multi = 1;
    int j = 0;
    for(int i = 0;i < length * 8;i++)
    {
        j = i / 8;
        if(i % 8 == 0)
        {
            multi = 1;
        }
        multi *= ((i % 8 == 0) ? 1 : 2);
        response[i] = ((Byte[j] & multi) == 0 ? 0 : 1);
    }
    return 1;
}

int TransAddress(int address,int *data)
{
    int first = 0,second = 0,third = 0,last = 0;
    last = address % 16;
    address /= 16;
    third = address % 16;
    address /= 16;
    second = address % 16;
    address /= 16;
    first = address % 16;

    data[0] = (first * 16) + second;
    data[1] = (third * 16) + last;
    return 1;
}
