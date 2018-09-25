#ifndef LASERMARKER_H
#define LASERMARKER_H

#include "LineDevice/Device/TcpDevice.h"

using namespace std;

class LaserMarker : public TcpDevice
{

public:

    LaserMarker(int port ,string ip);
    LaserMarker(int port , string ip, string name);
    void test();

    bool CanAcess();
    string DoPrint(string msg);

};

#endif // LASERMARKER_H
