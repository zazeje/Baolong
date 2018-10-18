#ifndef DEVICETHREAD_H
#define DEVICETHREAD_H

#include <sys/time.h>
#include "SerialPortParameters.h"
#include "LineModel/DeviceConfig.h"
#include "DataHelper/MySQLHelper.h"
#include "LineModel/ProductInfo.h"
#include "DataConvert/DataType.h"
#include "PBOXUI/display.h"
#include "DataHelper/MemoryDBHelper.h"
#include "DataCommunication/TcpServer.h"
#include "LineDevice/Device/TcpDevice.h"
#include "LineModel/LineInformation.h"
#include "LineDriver/AlarmDriver.h"


enum JudgeMethod
{
    lowPassrate = 1,        //静态电流
    maxLimit,               //发射电流
    pressJudge,             //环境气压判定
    tpmsJudge,              //手柄气压判定
    minLimit
};


class DeviceThread
{
public:

    DeviceThread(DeviceInfo di);
    DeviceThread();
    virtual ~DeviceThread();

    void ProcessCValue(PartTestItemInfo& pi, int index, JudgeMethod type, string cValue);

    bool SaveProductInfo(PartTestItemInfo &pi);

    void InitProductInfo(PartTestItemInfo& pi);

    virtual bool Start() = 0;
    virtual bool Stop() = 0;    

    void SetProInfo(PartTestItemInfo m_pi);
    PartTestItemInfo GetProInfo();

    DeviceInfo GetDeviceInfo();
    void SetDeviceInfo(DeviceInfo m_di);
    void FetchSmallBoardSeraialNo(string barcode);

    vector<string> GetSmallBoardSeraialNo();

    static void SetProductModel(string productmodel);
    static string GetProductModel();

    static string GetPartNo();
    static void SetPartNo(string);

    static void SetBillNum(string billnum);
    static string GetBillNum();

    vector<string> GetPartID();

private:

    static string productModel;
    static string BillNo;
    static string m_partNo;

protected:

    DeviceInfo _di;
    bool repeatFlag;
    bool _stopprocess;
    bool _updateFlag;
    string _lastProductModel;
    string _productModel;
    pthread_t pth;

    //add
    pthread_attr_t pthattr;

    int _counter;
    string _connectstatus;
    string _preConnectstatus;
    bool _debug;
    int pcount;

    //产品测试信息
    PartTestItemInfo pi;

    //设置单个流程中设备是否需要多次工作
    void SetDeviceRepeatWork(bool );

    int _devicePosition;
    string _num;
    ServerProductInfo spi;

    virtual bool getDeviceEnable();
    virtual void getDeviceNum();
    virtual void noJudge();
    virtual void saveToSql(string partNoId, string partSeqNo,int testItemCodeIndex);
    virtual void sendValueToTcpServer(string cValue);
    string _endFlag;
    virtual void processEndFlag();
};

enum CommunicationType
{
    Serial,TCP,NONE
};


#endif // DEVICETHREAD_H
