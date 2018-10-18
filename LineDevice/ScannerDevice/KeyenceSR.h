#ifndef KEYENCESR_H
#define KEYENCESR_H

#include "LineDevice//Device/TcpDevice.h"
#include <sys/time.h>

using namespace std;

class KeyenceSR : public TcpDevice
{

public:

    KeyenceSR(int port ,string ip);
    KeyenceSR(int port , string ip, string name);

    void Close();
    bool CanAcess();
    void StartScan();
    void StopScan();
    string GetBarCode();
    string AutoScan(double times);

private:

    const string testCmd;
    const string startCmd;
    const string stopCmd;
    const string endStr;
    bool _stop;

};


#endif // KEYENCESR_H
