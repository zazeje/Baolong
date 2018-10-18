#ifndef AGILENTMULTIMETERSERIAL_H
#define AGILENTMULTIMETERSERIAL_H

#include "./LineDevice/Device/ComDevice.h"
#include "./DataConvert/DataType.h"
#include <stdlib.h>

class AgilentMultimeterSerial : public ComDevice
{

public:

    AgilentMultimeterSerial();
    AgilentMultimeterSerial(SerialPortParameters*);
    AgilentMultimeterSerial(SerialPortParameters*, string);

    bool CanAcess();            //通信测试
    string GetValue();          //读万用表的值
    bool SendCommand(string command);       //发送指令到万用表
    string ReadData();          //从万用表接收数据
    string GetIdn();            //从万用表读取IDN
    bool SetDisplay(bool display);      //设置万用表屏幕是否显示
    string GetSystemError();    //从万用表读取错误信息
    bool Beep();                //向万用表写入报警请求
    bool CleraError();          //清除万用表错误信息

private:

    string DispOn;
    string DispOff;
    string SysiBeep;
    string MeasVoltDc;
    string MeasCurrDc;
    string _read;
    string SystErr;
    string Idn;
    string Cls;
    string endStr;

};

#endif // AGILENTMULTIMETERSERIAL_H
