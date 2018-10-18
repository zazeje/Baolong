#ifndef TPMSHANDLETCP_H
#define TPMSHANDLETCP_H

#include "LineDevice/Device/TcpDevice.h"

class TpmsHandleTcp : public TcpDevice
{

public:

    TpmsHandleTcp();
    TpmsHandleTcp(int port, string ip);
    TpmsHandleTcp(int port, string ip, string name);
    ~TpmsHandleTcp();

    bool CanAcess();
    string SendId(string id);
    string GetData();
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

#endif // TPMSHANDLETCP_H
