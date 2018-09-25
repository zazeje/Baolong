#ifndef DATABASEINFO_H
#define DATABASEINFO_H

#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include "LogFile.h"
#include <fstream>

using namespace std;

class DatabaseInfo
{
public:

    DatabaseInfo();
    ~DatabaseInfo();

    bool Init();

    string GetHostIP();
    string GetDBUserName();
    string GetPassword();
    string GetDatabaseName();
    string GetWorkCenterNo();
    bool SetWorkCenterNo(string WorkCenterNo);

    string m_LineName;  //线路名称

private:

    string m_workCenterNo;
    string m_hostIP;
    string m_userName;
    string m_password;
    string m_databaseName;


};

extern DatabaseInfo g_dbInfo;

#endif // DATABASEINFO_H
