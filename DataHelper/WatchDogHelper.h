#ifndef WATCHDOGHELPER_H
#define WATCHDOGHELPER_H

using namespace std;

class WatchDog
{
public:
	static WatchDog *instance();

	~WatchDog();

	void init(int timeout, int sleep);
	
	void run();
	
private:
	WatchDog();

private:
	static WatchDog *m_wd;
	int m_fd;
	int m_timeout;
	int m_sleep;
};

#endif //WATCHDOGHELPER_H
