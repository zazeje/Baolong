#ifndef ABBROBOT_H
#define ABBROBOT_H

#include "LineDevice/Device/TcpDevice.h"

class ABBRobot : public TcpDevice
{
public:
    ABBRobot(int port ,string ip);
    ABBRobot(int port ,string ip, string name);
    ~ABBRobot();
    string getMessage();
    bool CanAcess();
};

#endif // ABBROBOT_H
