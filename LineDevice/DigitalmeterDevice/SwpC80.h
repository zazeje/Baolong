#ifndef SWPC80_H
#define SWPC80_H

#include "./LineDevice/Device/ComDevice.h"
#include "./DataConvert/DataType.h"
#include <stdlib.h>

class SwpC80 : public ComDevice
{
    //气压测试
    //作者 武汉分公司/李世龙
    //时间 2017-2-7
public:

    SwpC80(SerialPortParameters *spp);      //构造函数
    SwpC80(SerialPortParameters *spp, string name);
    ~SwpC80();

    bool CanAcess();
    double GetValue();

private:

    string _cmdPress;

};

#endif // SWPC80_H
