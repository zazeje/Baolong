#ifndef MICROCALIPERTCP_H
#define MICROCALIPERTCP_H

#include "LineDevice/Device/TcpDevice.h"


class MicrocaliperTcp : public TcpDevice
{
public:

    MicrocaliperTcp(int port , string ip);      //构造函数
    MicrocaliperTcp(int port , string ip, string name);
    ~MicrocaliperTcp();

    bool CanAcess();
    double GetValue();
    void ClearValue();

private:
    string _getValue;
    string _clearValue;
};

#endif // MICROCALIPERTCP_H
