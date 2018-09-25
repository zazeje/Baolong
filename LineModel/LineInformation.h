#ifndef LINEINFORMATION_H
#define LINEINFORMATION_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;


class ProductionLineInfo            //生产线信息
{
public:

    string productionLine;          //生产线编号
    string productionLineName;      //生产线名称（描述）
    string ipAddressMes;            //地址1（与MES通信）
    string ipADdressPBox;           //地址2（与PBOX通信）
};

class WorkCenterInfo                //工作中心信息
{
public:

    string workCenterNo;            //工作中心编号
    string workCenterName;          //工作中心名称（描述）
    string productionLine;          //所属生产线
    string judgeMethod;             //判定方式（0不判定，1判定）
    string ipAddressPbox;           //PBOX地址

};

class MachineInfo                   //设备信息
{
public:

    string machCode;                //设备编号
    string machDesc;                //设备名称（描述）
    string workCenterNo;            //所属工作中心
    string machDriverNo;            //设备驱动编号
    string type;                    //通信接口类型
    string ipAddress;               //IP地址
    string port;                    //端口
    string baudrate;                //波特率
    string databits;                //数据位
    string stopbits;                //停止位
    string parity;                  //校验位
    string enable;                  //使能

    void init();
};

class MachDriverInfo                //设备驱动信息
{
public:

    string machDriverNo;            //设备驱动编号
    string machDriverDesc;          //设备驱动描述
};

class MachParameterInfo                  //设备参数信息
{
public:

    string partNo;                  //件号
    string machCode;                //设备编号
    string type;                    //参数类型（加工/环境）
    string parameterNo;             //参数编码
    string standardValue;           //标准值
    string maxValue;                //最大值
    string minValue;                //最小值

    void init();
};

class MachParameterCodeInfo              //设备参数编号信息
{
public:

    string machCode;                //设备编号
    string type;                    //参数类型（加工/环境）SAP文件，电路表量程
    string parameterNo;             //参数编码
    string parameterDesc;           //参数描述

};

class MachTestItemInfo              //设备测试项信息
{
public:

    string partNo;                  //件号
    string machCode;                //设备编号
    string testItemCode;            //测试项编号(0不启动，1电流测试,2电压测试)
    int judgeEneble;             //判定使能
    int judgeMethod;             //判定方式
    string errorCode;               //错误代码
    double standardValue;           //标准值
    double maxValue;                //最大值
    double minValue;                //最小值
    double deviation;               //偏差值
    int minPassrate;             //最低合格率
    int maxLimit;                //最大超限数

    void init();
};

class MachTestItemCodeInfo
{
public:

    string machCode;
    string testItemCode;
    string testItemDesc;
};

class DispatchInfo                  //派工单信息
{
public:

    string dispatchNo;              //派工单号
    string orderNo;                 //车间订单号
    string productionLine;          //生产线编号
    string partNo;                  //件号
    string projectNo;               //项目代号（产品型号）
    string user;                    //创建用户名
};

class ProcessInfo                   //工艺信息
{
public:

    string partNo;                  //件号
    string operationNo;             //工序号（sationnum）
    string workCenterNo;            //工作中心编号（板子）
    string operationDesc;           //工序描述
};

class PartTestItemInfo              //产品测试信息
{
public:
    string dispatchNo;              //派工单号
    string partNo;                  //件号
    string partNoId;                //产品ID
    string partSeqNo;               //产品序列号
    string operationNo;             //工序号
    string machCode;                //设备编码
    string testItemCode;            //测试项编号
    string testItemCollectValue;    //测试项采集数据
    string testItemEigenValue;      //测试项特征值
    int testItemJudgeResult;     //测试项判定结果
    string enterUser;               //录入人
    string enterDate;               //录入日期

//    int valueCount;
//    string testItemAvgValue;
//    string testItemMaxValue;
//    string testItemMinValue;

//    int passCount;
//    int lowCount;
//    int hightCount;
//    double badValue;
//    int passRate;
//    string errorCode;

    int ProductCount;

//    vector<double> NormalValues;
//    vector<double> LowValues;
//    vector<double> HigthValues;

    void Clear();
    void init();
};


class StartCmdInfo
{
public:
    string operationNo;             //工序号
    string projectNo;               //工程号
    string dispatchNo;              //派工单号
    string partNo;                  //件号
    string user;
    string currentTime;
    int productNum;                //产品ID
    void init();
    void setField(string projectno,string dispatchno,string partno);
    void setField(string projectno,string dispatchno,string partno,string username,string operationno);
};
//class LineInformation
//{
//public:
//    LineInformation();

//};



#endif // LINEINFORMATION_H
