#include "LogHelper.h"

/**
 * @brief 构造函数
 */
LogHelper::LogHelper()
{
    //初始化线程锁
    pthread_rwlock_init(&log_lock,NULL);
}

/**
 * @brief 写日志到文件
 * @param Log所在地址(工程名+类名+函数名)
 * @param Log内容
 */
void LogHelper::WriteToLogFile(string lineAuto,string logInfo)
{
    //加线程写锁
    if(pthread_rwlock_wrlock(&log_lock) != 0)
    {
        return;
    }
    string buff = "";
    int fileNum = 0;
    string dirPath = "./Log/";
    string file_name = "";
    file_name = "Log0.txt";
    struct dirent* entry;
    DIR* dir;
    FILE* fp;
    string filePath = dirPath + file_name;
    //文件路径是否存在，不存在则创建
    if(access(dirPath.data(),F_OK) != 0)
    {
        mkdir(dirPath.data(),0755);
    }
    if((fp = fopen(filePath.data(),"a+")) == NULL)
    {
        pthread_rwlock_unlock(&log_lock);
        return;
    }
    fseek(fp,0,SEEK_END);
    long length = ftell(fp);
    //计算文件大小，当文件超过10M时，将所有Log文件后缀加一，
    //当本地保存的Log文件超过10个时，将最后一个Log文件删除
    if(length > (10240 * 1024))      //10M
    {
        if((dir = opendir(dirPath.data())) == NULL)
        {
            return ;
        }
        while((entry = readdir(dir)) != NULL)
        {
            if(entry->d_type == 8)
            {
                fileNum++;
            }
        }
        closedir(dir);
        for(int i = fileNum;i > 0;i--)
        {
            if(i >= 200)
            {
                string fileName = "./Log/Log" + IntToString(i - 1) + ".txt";
                remove(fileName.data());
            }
            else
            {
                string newFileName = "./Log/Log" + IntToString(i) + ".txt";
                string oldFileName = "./Log/Log" + IntToString(i - 1) + ".txt";
                rename(oldFileName.data(),newFileName.data());
            }
        }
    }
    buff = GetLocalTime();
    fprintf(fp,"%s",buff.data());
    fprintf(fp,"%s",lineAuto.append(logInfo).append("\n").data());
    fclose(fp);
    pthread_rwlock_unlock(&log_lock);
}

/**
 * @brief 获取当前系统时间
 * @return
 */
string LogHelper::GetLocalTime()
{
    QDateTime now;
    QDate m_date;
    QTime m_time;
    now.setDate(m_date.currentDate());
    now.setTime(m_time.currentTime());
    QString currenttime = now.toString("  yyyy-MM-dd  hh:mm:ss:zzz  ");
    return currenttime.toStdString();
}
