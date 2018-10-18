#ifndef XSE6SERIAL_H
#define XSE6SERIAL_H

#include "./LineDevice/Device/ComDevice.h"

class Xse6Serial : public ComDevice
{
public:

    Xse6Serial();
    Xse6Serial(SerialPortParameters* spp);
    Xse6Serial(SerialPortParameters* spp,string devicename);
    ~Xse6Serial();

    bool CanAcess();
    double GetValue();
    double GetMaxValue();
    double GetMinValue();
    int GetBaud();
    double GetRangeMax();
    double GetRangeMin();

private:

    int DELAY;

    const string VSION;
    const string MAX_VALUE;
    const string MIN_VALUE;
    const string MIN_RANGE;
    const string MAX_RANGE;
    const string BAUD;
    const string addr;
    const unsigned int DATA_LEN;

    string ReadValue(string code);
    double ValueHandler(string value);
    string ReadParam(string code);
};

#endif // XSE6SERIAL_H
