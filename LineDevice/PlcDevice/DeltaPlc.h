#ifndef DELTAPLC_H
#define DELTAPLC_H

#include "./LineDevice/Device/ComDevice.h"

class DeltaPlc : public ComDevice
{

public:

    DeltaPlc(SerialPortParameters *spp, string deviceName);
    DeltaPlc(SerialPortParameters *spp, string deviceName,int delay);
    DeltaPlc(SerialPortParameters *spp, int stationNum, string deviceName);
    ~DeltaPlc();

    bool CanAcess();
    string ReadRegister(int address, char registerType);
    string ReadRegister(int address, int length, char registerType);
    void WriteRegister(int address, string state, char registerType);
    void WriteRegister(int address, string state, int length, char registerType);

private:

    const string registerIndexOutOfRange;
    const string STX ;
    const string _cmd01;//读线圈状态,作用范围：S、Y、M、T、C。
    const string _cmd02;//读输入状态,作用范围：S、X、Y、M、T、C。
    const string _cmd03;//读寄存器值,作用范围：T、C、D。
    const string _cmd05;//设定单一线圈,作用范围：S、Y、M、T、C。
    const string _cmd06;//预设单一寄存器,作用范围：T、C、D。
    const string _cmd15;//设定多个线圈,作用范围：S、Y、M、T、C。
    const string _cmd16;//预设多个寄存器,作用范围：T、C、D。
    const string _cmd17;//状态,机种报告。
    const int S0;
    const int X0;
    const int Y0;
    const int T0;
    const int M0;
    const int C0;
    const int D0;

    int _delay;

    string cmd;
    //protected ModbusMaster master = null;
    string _stationNum;
    string result;

    string ReadD(int address, int length);
    string ReadD(int address);
    void WriteD(int address, string state);
    void WriteD(int address, string state, int length);
    string ReadM(int address, int length);
    string ReadM(int address);
    void WriteM(int address, string state);
    void WriteM(int address, string state, int length);

};

#endif // DELTAPLC_H
