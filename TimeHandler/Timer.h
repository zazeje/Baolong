#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <signal.h>
#include <string>
#include <QFile>
#include "./DataHelper/MySQLHelper.h"
#include "./PBOXUI/mainwidget.h"

using namespace std;

class Timer
{

public:

    Timer();
    void SetIntervalTime(int sec);
    void TimerStart();

private:

    struct itimerval m_itv;
    struct itimerval m_oldtv;

};

#endif //TIMER_H
