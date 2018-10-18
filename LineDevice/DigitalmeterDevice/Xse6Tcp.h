#ifndef XSE6TCP_H
#define XSE6TCP_H

#include "LineDevice/Device/TcpDevice.h"

class Xse6Tcp : public TcpDevice
{
public:

    Xse6Tcp(int port , string ip);
    Xse6Tcp(int port , string ip, string name);
    ~Xse6Tcp();

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

#endif // XSE6TCP_H
