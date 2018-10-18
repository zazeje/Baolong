#ifndef MYSQLHELPER_H
#define MYSQLHELPER_H

#include <stdio.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "mysql.h"
#include "DataConvert/DataType.h"
#include "DataHelper/LogHelper.h"
#include "DataHelper/LogFile.h"

//#define HOSTIP   "192.168.127.100"
//#define USER   "root"
//#define PASSWD    "tpms"
//#define PBOX   "pbox"
//#define FLEXIBLELINE "flexibleline"

using namespace std;

class MySQLHelper
{
public:

    MYSQL_RES *m_result;

    MySQLHelper();
    MySQLHelper(string datasource,string username,string passwd,string database);

    bool MySQL_Init(MYSQL* m_connection);

    MYSQL_RES *LoadTable(MYSQL *m_connection, string command);

    bool InsertTable(MYSQL* m_connection,string command);

    MYSQL_RES *MySQL_GetContent(MYSQL* m_connection);

    void MySQL_Release(MYSQL_RES *result);

    void MySQL_Close(MYSQL* m_connection);

private:

    string DataSource;
    string UserName;
    string PassWord;
    string DataBase;

    pthread_mutex_t mutex1;

};

#endif // MYSQLHELPER_H
