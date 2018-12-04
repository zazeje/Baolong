#ifndef THREADKEYENCESR_H
#define THREADKEYENCESR_H

#include <vector>
#include <time.h>
#include <stdlib.h>
#include "LineDevice/ScannerDevice/KeyenceSR.h"
#include "LineDevice/Device/DeviceThread.h"
#include "LineModel/DeviceConfig.h"
#include "DataHelper/MemoryDBHelper.h"
#include "LineDevice/DeviceThreads/ThreadLaserMarker.h"
#include "LineDriver/LogicalDriver.h"
#include "DataCommunication/TcpServer.h"
#include "DataHelper/LogFile.h"


using namespace std;

enum KeyenceSRTest
{
    PartSeqenceNo = 1,          //产品序列号
    PartId,                     //小板取ID
};

enum KeyencePara
{
    SeqtoPartId = 1,        //通过序列号查找产品ID
    PartIdtoSeq,            //通过产品ID查找序列号
};

class ThreadKeyenceSR : public DeviceThread
{
public:

    ThreadKeyenceSR(DeviceInfo di);
    ~ThreadKeyenceSR();
    bool Start();
    bool Stop();
    void threadprocess();
    static void *Start_Thread(void*arg);
    string getJudgeResult(string barcode, KeyencePara type);                    //D2线获得上一个工位的判定结果
    string getPriorStationJudgeResult(string barcode, KeyencePara type);        //D2线获得上一个工位的判定结果
    bool GetStationValue(string barcode);                                       //D2线获取工位数据记录
    static int ProductCount;
    string GetPlcJR();

private:

    bool _stopprocess;
    bool _updateFlag;
    bool _controlFlag;
    pthread_t pth;
    KeyenceSR *myDevice;
    vector<string> m_scanOk;                                        //PLC扫码良品点
    vector<string> m_scanNG;                                        //PLC扫码不良品点
    string m_barCode;
    string m_value;
    string m_pointIndex;
    string D3GetBarCode(string barcode, KeyencePara type);          //D3线获得产品ID或者序列号
    string D3GetPriorStationJR(string flag, string barCode);        //D3线判断前工位判定结果
    vector<string> D3GetPLCScanOK();                                //D3线获取PLC“扫码不良”点位
    vector<string> D3GetPLCScanNG();                                //D3线获取PLC“扫码良”点位
    void clearData(string tagName);                                 //清除数据
    bool checkBarCodeState();                                       //检查扫码结果状态
    void processScanD3SC(string name);                              //处理烧程工位(扫序列号)信号
    void processScanD3SV(string partId, string partSeqNo);          //处理电池组装工位（视觉检测）信号
    void processScanD3SD(string name);                              //处理（电池焊接工位、涂覆工位、热熔、气压、打标、功率、侧漏、分拣）
    void processScanD3SQ(string name);                              //处理（电流测试、电池组装）工位信号
    void processScanD3NG();                                         //处理（外壳打标）工位信号
    void dealWithScanOkPoint(string name);                          //向PLC扫码良品点置位
    void dealWithScanNGPoint(string name);                          //向PLC扫码不良品点置位
    void writeToSeqNoAndID(string name, string partSeqNo, string partNoID);     //写入序列号、ID到内存缓冲区
    void getPointIndex(string name);                                //获取扫码点名下标
    void sendSeqNoAndIDToTcpServer(string partSeqNo, string partNoId);          //上传序列号、ID、位置给上位机
    void checkPriorStationAction(string name, string flag);         //检测前工位操作
    void processVisionCheck();                                      //处理视觉检测信号，进行判定
    void CommunicateTest();                                         //通信检测
};


#endif // THREADKEYENCESR_H
