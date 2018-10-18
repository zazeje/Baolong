#ifndef LOGHELPER_H
#define LOGHELPER_H

#include "DataConvert/DataType.h"
#include "QDateTime"
using namespace std;

class LogHelper
{
public:

    LogHelper();

    void WriteToLogFile(string lineAuto = "LineAuto",string logInfo = "");

private:

    pthread_rwlock_t log_lock;

    string GetLocalTime();
};

extern LogHelper m_log;
#endif // LOGHELPER_H
