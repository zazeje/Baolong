#ifndef MICROMETERTCP_H
#define MICROMETERTCP_H

#include "LineDevice/Device/TcpDevice.h"

class MicrometerTcp : public TcpDevice
{
public:

    MicrometerTcp(int port , string ip);      //构造函数
    MicrometerTcp(int port , string ip, string name);
    ~MicrometerTcp();

    bool CanAcess();
    vector<double> GetValue();
    void ClearValue();

private:
    string _getValue;
    string _clearValue;
};


#endif // MICROMETERTCP_H
