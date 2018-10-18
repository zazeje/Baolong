#ifndef ITECHPOWER_H
#define ITECHPOWER_H

#include "LineDevice/Device/ComDevice.h"
#include "DataConvert/DataType.h"
#include <stdlib.h>

class ItechPower : public ComDevice
{
    //稳压电源
    //作者 武汉分公司/李世龙
    //时间 2017-2-7
public:

    ItechPower(SerialPortParameters *spp);      //构造函数
    ItechPower(SerialPortParameters *spp, string name);

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

#endif // ITECHPOWER_H
