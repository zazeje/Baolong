#ifndef ITECHPOWERTCP_H
#define ITECHPOWERTCP_H


#include "./LineDevice/Device/TcpDevice.h"
#include "./DataConvert/DataType.h"
#include <stdlib.h>

class ItechPowerTcp : public TcpDevice
{
    //稳压电源
    //作者 武汉分公司/李世龙
    //时间 2017-2-7
public:

    ItechPowerTcp(int port , string ip);      //构造函数
    ItechPowerTcp(int port , string ip, string name);

    bool CanAcess();
    bool SetPower();
    bool SetRemoteCtrl();
    bool ClearError();
    bool OpenOutput();
    bool CloseOutput();
    string ReadCurrent();
    string ReadVoltage();
    bool Init();

private:

    string _idn;
    string _openOutPut;
    string _setPower;
    string _clearError;
    string _remoteCtrl;

};

#endif // ITECHPOWERTCP_H
