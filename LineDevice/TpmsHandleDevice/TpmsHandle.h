#ifndef TPMSHANDEL_H
#define TPMSHANDEL_H

#include "LineDevice/Device/ComDevice.h"

class TpmsHandle : public ComDevice
{

public:

    TpmsHandle();
    TpmsHandle(SerialPortParameters* spp);
    TpmsHandle(SerialPortParameters* spp,string devicename);
    ~TpmsHandle();

    bool CanAcess();
    string SendId(string id);
    string GetData(string devName = "");
    string SendEsc();
    string SendEnter();
    string SendUp();
    string SendDown();
    string SendPower();
    string SendLight();

private:

    const string Enter_KEY;
    const string Light_KEY;
    const string Power_KEY;
    const string Down_KEY;
    const string Up_KEY;
    const string Esc_KEY;
    const string TESTCMD;
    int idLen;
    int DELAY;

    string deviceName;
};
#endif //TPMSHANDEL_H
