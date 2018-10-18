#ifndef DEVICECONFIG_H
#define DEVICECONFIG_H

#include <string.h>
#include <string>
#include <map>
#include <list>
#include <stdio.h>
#include <vector>
#include "LineInformation.h"

using namespace std;

typedef list<string> INTLIST;

class ServerProductInfo
{
public:
    string partSeqNo;
    string partId;
    string collectValue;
    string collectValue1;
    string collectValue2;
    string message;
    string name;
    string position;
    vector<string> value;   //采集值容器

    void init();
};


enum MachDriverNo
{
    None = 0,                        //无效设备
    SerialCyclone,                  //串口烧程器
    UdpCyclone,                     //网口烧程器
    KeyenceSr,                      //KeyenceSr扫码器
    DataMan60,                      //DataMan60扫码器
    Wenglor,                        //威格乐扫码器
    FXPLC,                          //三菱FXPLC
    DelPLC,                         //台达DelPLC
    DgPLC,                          //大工DgPLC
    AgilMultMet,                    //安捷伦万用表
    GwMultMet,                      //固伟万用表
    GSP830,                         //GSP830频谱仪
    GSP930,                         //GSP930频谱仪
    AgilentN9010,                   //是德科技/安捷伦频谱仪
    TekTronix,                      //泰克频谱仪
    RohdSpecAnal,                   //罗德施瓦茨频谱仪
    Xse6,                           //气压测试数显表
    SwpC80,                         //SwpC80数字仪表
    ModbusMet,                      //ModbusMet仪表
    TpmsHand,                       //TpmsHand手柄
    LM,                             //LaserMarker打标机
    VC,                             //ViewCheck视觉检测
    Itech,                          //Itech稳压电源
    MicroMeter,                    //集线器
    LIN,                            //LIN设备
    MarkView,                        //MarkView视觉检测
    FX5U,                            //FX5U PLC
    MicroCaliper,                    //测微计
    TekDMM6500
};

class TagM
{
public:
    string Name;
    string value;
};

class Tag
{
public:
    string Name;                //采集点名称
    string Description;         //采集点描述,
    int Address;                //采集点地址,
    string LogicalMode;         //采集点逻辑模式
    string TagCode;             //采集点代码，
    string TagName;             //采集点点名
    string MemortValue;         //采集点内存数据库值
    string PlcValue;            //采集点PLC值
    int ControlTagCount;        //控制点个数，
    INTLIST CTagNames;          //控制点点名
    void Init();
};

// 采集单元配置信息
class UnitInfo
{
public:
    string Name;        //默认描述等于采集单元名称
    string Desc;        //默认描述等于采集单元描述
    bool Enable;
    int StartAddress;   //起始地址
    int Length;             //长度
    map<string,Tag> Tags;
    void Init();
};


// 设备配置信息
class DeviceInfo
{
public:
    string Name;
    string DeviceCode;      //设备编码
    string type;            //设备通信方式
    bool Enable;            //设备使能
    string Ip;
    string Port;
    string BaudRate;
    string DataBits;
    string StopBits;
    string Parity;        
    string Prefix;          //前缀
    string IdFlag;
    string StateFlag;
    string StartFlag;       //开始标志位
    string SnFlag;          //界面显示序列号
    string MiFlag;          //扫码状态位（“扫码成功”或“扫码失败”）
    string JudgeResult;     //判定结果（逻辑判定）
    string ProductCount;    //产品计数
    string EndFlag;         //结束测试标志位（清空数据）
    string MAV;             //设备平均值(气压表)
    string EigenValue;      //特征值(界面显示)
    string iValue;          //瞬时值(界面显示)
    string BarCode;         //扫码结果(界面显示)
    string DeviceState;

    string currentOperationNo;          //当前工序号
    string machDriverNo;                //设备驱动编号

    /*********************加工参数类成员变量*********************/
    vector<string> parameterNo;         //参数编码
    vector<string> parameter;           //参数（路径、量程、频谱仪等）
    vector<string> max;
    vector<string> min;

    vector<string> parameterNoInfo;
    vector<string> parameterDesc;
    /*********************加工参数类成员变量*********************/

    /*********************测试参数类成员变量*********************/
    vector<string> testItemCode;        //测试项条目
    vector<int> judgeEneble;             //判定使能
    vector<int> judgeMethod;             //判定方式
    vector<string> errorCode;               //错误代码
    vector<double> standardValue;           //标准值
    vector<double> maxValue;                //最大值
    vector<double> minValue;                //最小值
    vector<double> deviation;               //偏差值
    vector<int> minPassrate;             //最低合格率
    vector<int> maxLimit;                //最大超限数

    vector<string> testItemCodeInfo;
    vector<string> testItemDesc;
    /*********************测试参数类成员变量*********************/
    map<string,UnitInfo> Units;

    string serverId;
    string serverSeqNo;
    string serverCollectValue;
    string serverMessage;
    string serverAlarm;
    string serverCommunication;
    string serverNote;
    string testStartPos;            //测试项起始位置（针对于采集值CV1\CV2）
    string Alarm;
    string communicateOk;
    string communicateERR;
    string devParaAlarm;            //加工参数报警
    string testParaAlarm;           //测试项参数报警
    string beat;                    //PLC心跳检测
    string devInitAlarm;            //设备初始化失败报警

    bool communiAlarmFlag;
    bool devInitFlag;               //设备初始化失败报警标志位
    bool devParaFlag;               //加工参数报警标志位
    bool testParaFlag;              //测试项参数报警标志位

    void Init();
//    bool operator < (const DeviceInfo& di);
};

// 工位配置信息
class StationInfo
{
public:
    string Name;            //工艺名称
    string ProductionLine;     //产品线
    string Code;            //工作中心编号
    string Prefix;          //前缀
    string StartFlag;
    string SnFlag;
    string IdFlag;
    string StateFlag;
    string IpEth0;
    string IpEth1;
    map<string,DeviceInfo> Dis;
    void Init();
    vector<ServerProductInfo> Sp;       //产品数量容器（向上位机发送信息）
    string communicationStatus;
    string noteMessage;
    string iValue;
    string endNum;           //测试结束点位个数
    string testCodeNum;      //上位机需要显示的测试项条目个数(显示N组采集值,CV1/CV2)
    string alarmStatus;     //M115机台报警
    int judgeMethod;
    int productNumber;
    bool judgeEnable;

//    string ProductCount;
    string JudgeResult;       //工位判定结果

    static int ProductCount;    //工位产品计数
    DeviceInfo di;

};

// 产线配置信息
class LineInfo
{
public:
    string Name;
    StationInfo Si;
    void Init();
};

#endif // DEVICECONFIG_H
