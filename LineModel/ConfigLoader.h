#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <stdio.h>
#include <QFile>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include "DeviceConfig.h"
#include "DataConvert/DataType.h"

class ConfigLoader
{

public:

    ConfigLoader();

    LineInfo LoadConfig(string linename);
    void test();
    static int m_deviceNumcount;
};

extern int g_deviceNum;     //当前工位设备总数
extern int testCodeNum;

#endif // CONFIGLOADER_H
