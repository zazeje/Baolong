#include "Timer.h"
#include "main.h"

//extern LineInfo gLine;

void TimingTrigger(int signo)
{
    string command;
    string logname = "./Log/";
    string dirname;
    char filename[10] = {0};
    string craftcode = gLine.Si.Code;
    if(craftcode.empty())
    {
        craftcode = "Log";
    }
    dirname = "./" + craftcode;
    if(craftcode != "Log")
    {
        if(opendir(dirname.data()) != NULL)
        {
            //rmdir(dirname.data());
            command = "rm " + dirname + " -rf";
            system(command.data());
        }
    }
    for(unsigned int i = 0;i < 10;i++)
    {
        sprintf(filename,"Log%d.txt",i);
        QFile file(QString::fromStdString(logname + filename));
        if(file.open(QFile::ReadOnly | QFile::Text))
        {
            file.close();
        }
    }
    if(craftcode != "Log")
    {
        command = "cp " + logname + " " + dirname + " -rf";
        system(command.data());
    }
    command.clear();
    command.append("tar -zcvf ").append(craftcode).append(".tar.gz ").append(dirname);
    system(command.data());
    command.clear();
    if(opendir("/mnt/PBox") == NULL)
    {
        command.append("mount -t cifs -o user=administrator,passwd=123,nounix,noserverino //");
        command.append("192.168.20.13");
        command.append("/share /mnt/");
        system(command.data());
    }
    command.clear();
    command.append("mv ").append(craftcode).append(".tar.gz ").append("/mnt/PBox/Log/").append(craftcode).append("_").append("Log.tar.gz");
    system(command.data());
    command.clear();
}

Timer::Timer()
{
    m_itv.it_interval.tv_sec = 20;
    m_itv.it_interval.tv_usec = 0;
    m_itv.it_value.tv_sec = 20;
    m_itv.it_value.tv_usec = 0;
}

void Timer::SetIntervalTime(int sec)
{
    m_itv.it_interval.tv_sec = sec;
}

void Timer::TimerStart()
{
    signal(SIGALRM, TimingTrigger);
    setitimer(ITIMER_REAL, &m_itv, &m_oldtv);
}


