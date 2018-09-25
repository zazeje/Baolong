#ifndef PLCBASE_H
#define PLCBASE_H

#include "./LineDevice/Device/ComDevice.h"
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

enum ProtocolMode
{
    // 计算机专有协议,我们一般用于485组网或232单路
    dedicatedProtocol,
    // 三菱自己用的编程口协议，是422协议
    programProtocol
};

enum RegisterBytes
{
    TWO = 2,
    FOUR = 4
};

using namespace std;

class PlcBase : public ComDevice
{
public:

    virtual bool CanAcess() = 0;
    virtual vector<int> ReadWord(int address, int length) = 0;
    virtual bool WriteWord(int address, vector<int> value) = 0;
    virtual string ReadBits(int address, int length, string registerType) = 0;
    virtual bool WriteBits(int address, string value, string registerType) = 0;

    ~PlcBase();
protected:

    PlcBase();
    PlcBase(SerialPortParameters* spp, string deviceName,bool blockMode = true);
    PlcBase(SerialPortParameters* spp, bool blockMode = true);

    pthread_rwlock_t m_lock;
};

#endif // PLCBASE_H
