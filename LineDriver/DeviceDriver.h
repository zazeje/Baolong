#ifndef DEVICEDRIVER_H
#define DEVICEDRIVER_H

#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "DataCommunication/TcpServer.h"
#include "LineDevice/Device/DeviceThread.h"


class DeviceThread;


class DeviceDriver
{

public:

    DeviceDriver(LineInfo li);
    ~DeviceDriver();
    DeviceDriver();

    bool Start();
    bool Stop();

private:

    LineInfo _li;
    string _productModel;
    list<DeviceThread *> dts;

};

#endif
