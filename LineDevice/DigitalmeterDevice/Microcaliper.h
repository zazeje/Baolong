#ifndef MICROCALIPER_H
#define MICROCALIPER_H

#include "LineDevice/Device/ComDevice.h"


class Microcaliper : public ComDevice
{
public:

    Microcaliper(SerialPortParameters *spp);      //构造函数
    Microcaliper(SerialPortParameters *spp, string name);
    ~Microcaliper();

    bool CanAcess();
    double GetValue();
    void ClearValue();

private:
    string _getValue;
    string _clearValue;
};

#endif // MICROCALIPER_H
