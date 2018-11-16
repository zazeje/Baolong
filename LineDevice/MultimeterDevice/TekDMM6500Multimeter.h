#ifndef TEKDMM6500_H
#define TEKDMM6500_H

#include "./LineDevice/Device/TcpDevice.h"
#include "./DataConvert/DataType.h"
#include <stdlib.h>

class TekDMM6500MultimeterTcp : public TcpDevice
{
public:
    TekDMM6500MultimeterTcp(int port,string ip);
    TekDMM6500MultimeterTcp(int port, string ip, string name);

    bool CanAcess();                        //通信测试
    string GetValue();                      //读万用表的值
    bool SendCommand(string command);       //发送指令到万用表
    string ReadData();                      //从万用表接收数据
    string GetIdn();                        //从万用表读取IDN
    string GetSystemError();                //从万用表读取错误信息
    bool Beep();                            //向万用表写入报警请求
    bool ClearError();                      //清除万用表错误信息

    bool SetWorkModeByBatch();              //设置批量读取模式
    string GetValueByBatch(int _coefficient);               //批量读取万用表的值
    bool InitDevice();                      //初始化万用表设备
    void InitPara(string gearParaStr,string rangParaStr);

private:

    string SysiBeep;
    string FuncVoltDc;
    string FuncCurrDc;
    string VoltRang;
    string CurrRang;
    string VoltNplc;
    string CurrNplc;
    string m_Read;
    string SystErr;
    string Idn;
    string Cls;
    string Nplc1;
    string endStr;

    string MakeBuf100;
    string LoadBuf100;
    string InitRead;
    string WaitRead;
    string GetBuf100;
    string ClearBuf100;

    string m_gearPara;
    string m_rangePara;
};

#endif // TEKDMM6500_H
