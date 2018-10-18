#ifndef LOGFILE_H
#define LOGFILE_H

extern "C" {
 #include <log4c/init.h>
 #include <log4c/category.h>
 }

#include <string>
#include <assert.h>

#define MAX_LEN 1024
using namespace std;

class LogFile
{
public:
    LogFile();

    bool LogInit();
//    void LogOut(string buff);
    void LOG_DEBUG(const char *format, ...);
    void LOG_INFO(const char *format, ...);
    void LOG_ERROR(const char *format, ...);


private:
    log4c_category_t* mylog;
    log4c_category_t* mystdout;
    pthread_rwlock_t log_lock;

};

extern LogFile _log;


#endif // LOGFILE_H
