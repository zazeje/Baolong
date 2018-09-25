#ifndef THREADFXPLC_H
#define THREADFXPLC_H

#include <vector>
#include <unistd.h>
#include <map>
#include <list>
#include <string.h>
#include "LineDevice/Device/DeviceThread.h"
#include "LineDevice/PlcDevice/FxPlc.h"
#include "LineModel/DeviceConfig.h"
#include "DataHelper/MemoryDBHelper.h"
#include "DataHelper/ParseCsv.h"


using namespace std;

class ThreadFxPlc : public DeviceThread
{
public:

    ThreadFxPlc(DeviceInfo di);
    ~ThreadFxPlc();

    bool Start();
    bool Stop();
    void threadprocess();
    static void *Start_Thread(void*arg);

    static void setPlcAlarmValue(vector<int> pointValue);
    static vector<int> getPlcAlarmValue();
    static vector<int> m_alarmAddress;            //排序后存放寄存器地址
    static vector<int> m_pointValue;
    vector<PlcPointInfo> plc;


private:

    bool _stopprocess;
    bool _updateFlag;
    string _iValue;
    string _cValue;
    string _lastProductModel;
    string _productModel;
    pthread_t pth;

    FxPlc *myDevice;

    void ProcessUnit(UnitInfo);

    void readAlarmPlc();
    vector<int> sortAlarmPlc(vector<PlcPointInfo> PlcAlarmVec);

};

#endif // THREADFXPLC_H
