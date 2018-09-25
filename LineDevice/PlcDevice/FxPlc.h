#ifndef FXPLC_H
#define FXPLC_H

#include "./PlcBase.h"

//enum RegisterType
//{
//    D,M,X,Y
//};

class FxPlc : public PlcBase
{

public:

    FxPlc();
    FxPlc(SerialPortParameters *spp, string deviceName,ProtocolMode protocolMode,bool blockMode = true);
    FxPlc(SerialPortParameters *spp, int stationNum, char* deviceName, ProtocolMode protocolMode,bool blockMode = true);

    bool CanAcess();
    vector<int> ReadWord(int address, int length);                      //批量读取字元件（D）
    bool WriteWord(int address, vector<int> value);                     //批量写字元件（D）
    string ReadBits(int address, int length, string registerType);      //批量读位元件
    bool WriteBits(int address, string value, string registerType);     //批量写位元件

private:

    string _pc;
    string _readM;
    string _writeM;
    string _readD;
    string _writeD;
    string _programPortWriteCmd;
    string _programPortForceONCmd;
    string _programPortForceOFFCmd;
    string _onePoint;
    string _off;
    string _on;
    string _programPortReadCmd;
    string _programPortWriteONCmd;
    string _programPortWriteOFFCmd;
    string _stationNum;
    ProtocolMode _protocolMode;
    int _delay;

    vector<int> ReadWordBy232(int address, int length, RegisterBytes registerBytes);             //批量读取字元件寄存器D（232协议）
    bool WriteWordBy232(int address, vector<int> value);            //批量写入字元件D寄存器（232协议）
    string ReadBitBy232(int address, int length);                   //批量读取连续M点地址（232协议）
    bool WriteBitBy232(int address, string state);                  //批量写辅助继电器M地址（232协议）
    vector<int> ReadWordBy422(int address, int length, RegisterBytes registerBytes);    //批量读字元件（D寄存器422协议）
    bool WriteWordBy422(int address, vector<int> value, RegisterBytes registerBytes);   //批量写字元件（D寄存器422协议）
    string ReadBitBy422(int address, int length);                   //批量读取位元件（M寄存器422协议）
    bool WriteBitBy422(int address, string value);                  //置位/复位位元件（M寄存器422协议）
    string ReadBitM(int address, int length);                       //批量读位元件（M寄存器）
    bool WriteBitM(int address, string value);                      //批量写位元件（M寄存器）

};


#endif // FXPLC_H
