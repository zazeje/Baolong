#ifndef AGILENTMULTIMETERTCP_H
#define AGILENTMULTIMETERTCP_H


#include "LineDevice/Device/TcpDevice.h"
#include "DataConvert/DataType.h"
#include <stdlib.h>


class AgilentMultimeterTcp : public TcpDevice
{

public:

    AgilentMultimeterTcp(int port,string ip);
    AgilentMultimeterTcp(int port, string ip, string name);

    bool CanAcess();                        //通信测试
    string GetValue();                      //单点读万用表的值
    bool SendCommand(string command);       //发送指令到万用表
    string ReadData();                      //从万用表接收数据
    string GetIdn();                        //从万用表读取IDN
    bool SetDisplay(bool display);          //设置万用表屏幕是否显示
    string GetSystemError();                //从万用表读取错误信息
    bool Beep();                            //向万用表写入报警请求
    bool CleraError();                      //清除万用表错误信息
    bool SetWorkModeByBatch();              //设置批量读取模式
    string GetValueByBatch(int _coefficient);               //批量读取万用表的值
    bool InitDevice();                      //初始化万用表设备
    void InitPara(string para1 = "",string para2 = "");

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
    string _readByBatch;
    string endStr;
    string BatchCount;
    string INIT;
//    readBatch = "READ:ARR?\n";
//    readBatch = ":FETC:ARR?\n";
    string Trg;
    string SourBus;
    string Tim;
    string m_para1;
    string m_para2;

};


#endif
