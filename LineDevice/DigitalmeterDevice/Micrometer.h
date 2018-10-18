#ifndef MICROMETER_H
#define MICROMETER_H

#include "./LineDevice/Device/ComDevice.h"

class Micrometer : public ComDevice
{
public:

    Micrometer(SerialPortParameters *spp);      //构造函数
    Micrometer(SerialPortParameters *spp, string name);
    ~Micrometer();

    bool CanAcess();
    vector<double> GetValue();
    void ClearValue();

private:
    string _getValue;
    string _clearValue;
};

#endif // MICROMETER_H
