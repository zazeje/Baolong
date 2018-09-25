#ifndef VIEWCHECK_H
#define VIEWCHECK_H

#include "LineDevice/Device/TcpDevice.h"

using namespace std;

class ViewCheck : public TcpDevice
{
    //视觉检测设备
    //作者 武汉分公司/李世龙
    //时间 2017-2-7
public:

    ViewCheck(int port ,string ip);
    ViewCheck(int port , string ip, string name);

    bool CanAcess();                                //通信检测
    string GetCheckResult(string id);               //取视觉检测结果

private:

    string startCheck;                              //视觉检测开始
    string heartCheck;                              //通信检测

};

#endif // VIEWCHECK_H
