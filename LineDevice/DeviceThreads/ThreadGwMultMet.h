#ifndef THREADGWMULTMET_H
#define THREADGWMULTMET_H
#if 0
#include "./LineDevice/Device/TcpDevice.h"
#include "./LineDevice/MultimeterDevice/GwinstekMultimeter.h"
#include "./LineDevice/Device/DeviceThread.h"
#include "./DataConvert/DataType.h"
#include "./DataHelper/MemoryDBHelper.h"
#include "./DataHelper/MySQLHelper.h"
#include "./PBOXUI/display.h"
#include "./LineDriver/LogicalDriver.h"
#include <stdlib.h>
#include <time.h>

class ThreadAgilent;

class ThreadGwMultMet : public DeviceThread
{
    //固伟万用表
    //作者 武汉分公司/李世龙
    //时间 2017-2-7
public:
    ThreadGwMultMet();
    ThreadGwMultMet(DeviceInfo di);
    ~ThreadGwMultMet();

    bool Start();
    bool Stop();

    void threadprocess();
    static void *Start_Thread(void *arg);

private:

    bool _reinitflag;
    bool _stopprocess;
    bool _updateFlag;
    string _iValue;
    string _cValue;

    string _productModel;
    string _lastProductModel;
    string startFlag;
    CommunicationType type;
    GwinstekMultimeter *myComDevice;

    static bool _stopColl;

    static void* CollectData(void *);
    bool InitDevice();
    string CheckModel(DeviceInfo &m_di);
    void CollectDataProcess();
};
#endif

#endif // THREADGWMULTMET_H
