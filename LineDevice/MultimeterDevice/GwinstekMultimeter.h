#ifndef GWINSTEKMULTIMETER_H
#define GWINSTEKMULTIMETER_H

#include "./LineDevice/Device/ComDevice.h"
#include "./DataConvert/DataType.h"
#include <stdlib.h>

class GwinstekMultimeter : public ComDevice
{
    //固伟万用表
    //作者 武汉分公司/李世龙
    //时间 2017-2-7

public:

    GwinstekMultimeter(SerialPortParameters *spp);      //构造函数
    GwinstekMultimeter(SerialPortParameters *spp, string name);

    bool CanAcess();                    //通信测试
    void SetDisplay(bool display);      //设置显示模式
    string GetIdn();                    //获取IDN
    void ClearError();                  //清除万用表错误信息
    string GetValue();                  //读万用表的值
    vector<string> GetValues(int count, int wait);//批量读取万用表的值
    bool SendCommand(string command);                 //发指令到万用表
    string Read();                      //读万用表的值

private:

    string _readCmd;            //读取命令
    string _setBatchCount;      //设置读取万用表数量命令
    string _setDisplayClose;    //设置万用表关闭命令
    string _setDisplayOpen;     //设置万用表打开命令
    string _cls;                //清除命令
    string _idn;                //通信检测命令
    int _delay;                 //延迟时间

};

#endif // GWINSTEKMULTIMETER_H
