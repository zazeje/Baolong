#ifndef MARKVIEWCHECK_H
#define MARKVIEWCHECK_H

#include "LineDevice/Device/TcpDevice.h"

class MarkViewCheck : public TcpDevice
{
public:

    MarkViewCheck(int port ,string ip);
    MarkViewCheck(int port , string ip, string name);

    bool CanAcess();                                //通信检测
    vector<string> GetCheckResult();               //取视觉检测结果

private:

    string _getValue;
    string _getState;
};

#endif // MARKVIEWCHECK_H
