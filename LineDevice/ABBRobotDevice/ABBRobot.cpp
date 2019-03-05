#include "ABBRobot.h"

ABBRobot::ABBRobot(int port, string ip) : TcpDevice(port,ip)
{

}

ABBRobot::ABBRobot(int port , string ip, string name) : TcpDevice(port,ip,name)
{

}

ABBRobot::~ABBRobot()
{

}

string ABBRobot::getMessage()
{
    return Read("Ascii");
}

bool ABBRobot::CanAcess()
{

}
