#ifndef CNET100TCP_H
#define CNET100TCP_H

#include "LineDevice/Device/TcpDevice.h"

enum workmode
{
    staticMode = 0,
    dynamicMode = 1
};

class Cnet100Tcp : public TcpDevice
{
public:
    Cnet100Tcp(int port ,string ip);
    Cnet100Tcp(int port ,string ip, string name);
    ~Cnet100Tcp();

    bool setWorkMode24G1T1R(workmode mode);
    string getMessage();
    vector<float> analysis77G500(const string &str);
    vector<unsigned int> analysis24G1T1R323(const string &str);
    bool CanAcess();

};

#endif // CNET100TCP_H
