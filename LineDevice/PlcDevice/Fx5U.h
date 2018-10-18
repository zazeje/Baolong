#ifndef FX5U_H
#define FX5U_H

#include "PlcBase.h"

class Fx5U : public PlcBase
{
public:

    Fx5U(SerialPortParameters *spp, string deviceName, bool blockMode = true);
    Fx5U(SerialPortParameters *spp, int stationNum, string deviceName, bool blockMode = true);

    bool CanAcess();

    //批量读取字元件（D）
    vector<int> ReadWord(int address, int length);
    //批量写字元件（D）
    bool WriteWord(int address, vector<int> value);
    //批量读取位元件
    string ReadBits(int address, int length, string registerType);
    //批量写入位元件
    bool WriteBits(int address, string value, string registerType);

private:

    string _framehead;
    string _readByBit;
    string _readByWord;
    string _writeByBit;
    string _writeByWord;
    string _registerM;
    string _registerD;
    int _delay;

    char _ENQ;
    char _ACK;
    char _NAK;
    char _STX;
    char _ETX;

    string ReadBitBy232(int address, int length);
    bool WriteBitBy232(int address, string value);
    vector<int> ReadWordBy232(int address, int length, RegisterBytes registerBytes);
    bool WriteWordBy232(int address, vector<int> value, RegisterBytes registerBytes);
};

#endif // FX5U_H
