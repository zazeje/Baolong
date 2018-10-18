#ifndef LOCALLINEINFO_H
#define LOCALLINEINFO_H
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include "LogFile.h"
#include <fstream>

using namespace std;

class LocalLineInfo
{
public:
    LocalLineInfo();
    string m_localPartNo;
    string m_localDispatchNo;
    string m_localOperationNo;
    string m_localUser;
    string m_localProjectNo;
    string m_localProductNumber;
    string m_localPartNofirst;
    string m_localDispatchNofirst;
    string m_localProjectNofirst;
    bool LoadLineInfo();
    bool SaveLineInfo();
};

extern LocalLineInfo g_locallineInfo;

#endif // LOCALLINEINFO_H
