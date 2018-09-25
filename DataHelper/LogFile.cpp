#include "LogFile.h"


LogFile::LogFile()
{
    mylog = NULL;
    mystdout = NULL;
    pthread_rwlock_init(&log_lock,NULL);
}

bool LogFile::LogInit()
{
    //log4c_init初始化成功返回0
    if(log4c_init())
    {
        _log.LOG_ERROR("Log4c 初始化【失败】");
        return false;
    }
    else
    {
        mylog = log4c_category_get("LineAuto");
        mystdout = log4c_category_get("-LineAuto");
    }
    _log.LOG_DEBUG("Log4c 初始化【成功】");
    return true;
}

//void LogFile::LogOut(string buff)
//{
//    log4c_category_log(mylog, LOG4C_PRIORITY_INFO, buff.data());
//}

void LogFile::LOG_DEBUG(const char *format,...)
{
    pthread_rwlock_wrlock(&log_lock);
    va_list ap;
    assert(mylog != NULL || mystdout != NULL);
    va_start(ap, format);
    log4c_category_vlog(mylog , LOG4C_PRIORITY_DEBUG , format , ap);
    log4c_category_vlog(mystdout , LOG4C_PRIORITY_DEBUG , format , ap);
    va_end(ap);

    pthread_rwlock_unlock(&log_lock);
}

void LogFile::LOG_INFO(const char *format,...)
{
    pthread_rwlock_wrlock(&log_lock);

    va_list ap;
    assert(mylog != NULL || mystdout != NULL);
    va_start(ap, format);
    log4c_category_vlog(mylog , LOG4C_PRIORITY_INFO , format , ap);
    log4c_category_vlog(mystdout , LOG4C_PRIORITY_INFO , format , ap);

    va_end(ap);

    pthread_rwlock_unlock(&log_lock);
}

void LogFile::LOG_ERROR(const char *format,...)
{
    pthread_rwlock_wrlock(&log_lock);

    va_list ap;
    assert(mylog != NULL || mystdout != NULL);
    va_start(ap, format);
    log4c_category_vlog(mylog , LOG4C_PRIORITY_ERROR , format , ap);
    log4c_category_vlog(mystdout , LOG4C_PRIORITY_ERROR , format , ap);

    va_end(ap);

    pthread_rwlock_unlock(&log_lock);
}
