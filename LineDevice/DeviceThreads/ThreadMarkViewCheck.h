#ifndef THREADMARKVIEWCHECK_H
#define THREADMARKVIEWCHECK_H

#include "LineDevice/ViewCheckDevice/MarkViewCheck.h"
#include "LineDevice/Device/DeviceThread.h"
#include "LineModel/DeviceConfig.h"
#include "DataHelper/MemoryDBHelper.h"
#include "LineDriver/LogicalDriver.h"
#include "DataCommunication/TcpServer.h"
#include "DataHelper/LogFile.h"

class ThreadMarkViewCheck : public DeviceThread
{
    //视觉检测设备
    //作者 武汉分公司/李世龙
    //时间 2017-6-29
public:
    ThreadMarkViewCheck();
    ThreadMarkViewCheck(DeviceInfo di);
    ~ThreadMarkViewCheck();

    bool Start();
    bool Stop();
    void threadprocess();
    static void *Start_Thread(void*arg);
private:

    pthread_t pth;
    MarkViewCheck *myDevice;
    string m_viewRes;
    vector<string> m_checkRes;
    bool CheckDate(string currentTime);
    void processViewCheck(string partSeqNo, string partNoId);
    void getViewCollectValue();
    void judgeViewCheck(string partNoId);
    void startToJudge(string partNoId);
    void clearData(string tagName);
    void CommunicateTest();

};

#endif // THREADMARKVIEWCHECK_H
