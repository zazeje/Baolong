#include "WatchDogHelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <unistd.h>

WatchDog * WatchDog::m_wd = NULL;

WatchDog *WatchDog::instance()
{
	if (m_wd == NULL)
		m_wd = new WatchDog();
	return m_wd;
}

WatchDog::WatchDog() : m_fd(-1), m_timeout(10), m_sleep(2)
{
	m_fd = open("/dev/watchdog", O_WRONLY);
    if (m_fd == -1)
    {
		printf("open watchdog error\n");
        perror("watchDog:");
    }
}

WatchDog::~WatchDog()
{
	
}

void WatchDog::init(int timeout, int sleep)
{
	m_timeout = timeout;
	m_sleep = sleep;
	ioctl(m_fd, WDIOC_SETTIMEOUT, &timeout);
}

void WatchDog::run()
{
	while (1)
	{
		ioctl(m_fd, WDIOC_KEEPALIVE, 0);
        usleep(m_sleep * 1000 * 1000);
	}

	return;
}

