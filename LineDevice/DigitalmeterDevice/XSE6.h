#ifndef XSE6_H
#define XSE6_H

#include "./LineDevice/Device/ComDevice.h"

class XSE6 : public ComDevice
{
public:

    XSE6();
    XSE6(SerialPortParameters* spp);
    XSE6(SerialPortParameters* spp,string devicename);
    ~XSE6();

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

#endif //XSE6_H
