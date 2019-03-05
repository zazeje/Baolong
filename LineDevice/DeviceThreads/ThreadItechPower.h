#ifndef THREADITECHPOWER_H
#define THREADITECHPOWER_H

#include "LineDevice/Device/DeviceThread.h"
#include "LineDevice/PowerSupply/ItechPower.h"
#include "LineDriver/LogicalDriver.h"
#include "LineModel/DeviceConfig.h"
#include "DataHelper/MemoryDBHelper.h"
#include "DataHelper/LogFile.h"
#include "LineDevice/PowerSupply/ItechPowerTcp.h"

using namespace std;

class ThreadItechPower : public DeviceThread
{
    //稳压电源
    //作者 武汉分公司/李世龙
    //时间 2017-2-7
public:

    ThreadItechPower(DeviceInfo di);
    ThreadItechPower();
    ~ThreadItechPower();

    bool Start();
    void threadprocess();
    static void *Start_Thread(void*arg);
    bool Stop();

private:
    bool getValue(string &staticV,string &dynamicV);
    bool checkValue(const string staticV,const string dynamicV);

    ItechPowerTcp *myTcpDevice;
    ItechPower *myComDevice;
    bool _stopprocess;
    pthread_t pth;
    string staticValue;
    string dynamicValue;



    string m_PointOk;                                        //PLC扫码良品点
    string m_PointNG;                                        //PLC扫码不良品点

    string D3GetPLCPointOK();                                //D3线获取PLC“扫码不良”点位
    string D3GetPLCPointNG();                                //D3线获取PLC“扫码良”点位

};



#endif // THREADITECHPOWER_H
