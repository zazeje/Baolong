#ifndef MEMORYDBHELPER_H
#define MEMORYDBHELPER_H

#include "./LineModel/DeviceConfig.h"
#include <string>
#include <stdio.h>
#include <errno.h>
#include "DataConvert/DataType.h"
#include <iostream>
#include "DataHelper/LogFile.h"

#define SMALL_BORD_SERIAL_NO "SmallBordSerialNo"

using namespace std;

class MemoryDB
{
public:

    MemoryDB();
    ~MemoryDB();

    string Read_TagMValue(string key);

    bool Write_TagMValue(string key, string value);

private:

    pthread_rwlock_t m_lock;
    map<string,TagM> M;

};

extern MemoryDB m_db;

#endif // MEMORYDBHELPER_H
