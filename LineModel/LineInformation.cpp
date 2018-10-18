#include "LineInformation.h"

//LineInformation::LineInformation()
//{

//}

void PartTestItemInfo::init()
{
    dispatchNo = "";
    partNo = "";                  //件号
    partNoId = "";                //产品ID
    partSeqNo = "";               //产品序列号
    operationNo = "";             //工序号
    machCode = "";                //设备编码
    testItemCode = "";;            //测试项编号
    testItemCollectValue = "";    //测试项采集数据
    testItemEigenValue = "";      //测试项特征值
    testItemJudgeResult = 0;     //测试项判定结果
    enterUser = "";               //录入人
    enterDate = "";               //录入日期  
    ProductCount =0;
}


void PartTestItemInfo::Clear()
{
    dispatchNo.clear();
    partNo.clear();                  //件号
    partNoId.clear();                //产品ID
    partSeqNo.clear();               //产品序列号
    operationNo.clear();             //工序号
    machCode.clear();                //设备编码
    testItemCode.clear();            //测试项编号
    testItemCollectValue.clear();    //测试项采集数据
    testItemEigenValue.clear();      //测试项特征值
    testItemJudgeResult = 0;        //测试项判定结果
    enterUser.clear();               //录入人
    enterDate.clear();               //录入日期    
    ProductCount =0;
}

void MachineInfo::init()
{
     machCode = "";                //设备编号
     machDesc = "";                //设备名称（描述）
     workCenterNo = "";            //所属工作中心
     machDriverNo = "";            //设备驱动编号
     type = "";                    //通信接口类型
     ipAddress = "";               //IP地址
     port = "";                    //端口
     baudrate= "";                //波特率
     databits = "";                //数据位
     stopbits = "";                //停止位
     parity = "";                  //校验位
     enable = "1";                  //使能
}

void MachParameterInfo::init()
{
     partNo = "";                  //件号
     machCode = "";                //设备编号
     type = "";                    //参数类型（加工/环境）
     parameterNo = "";             //参数编码
     standardValue = "";           //标准值
     maxValue = "";                //最大值
     minValue = "";                //最小值
}

void MachTestItemInfo::init()
{
     partNo = "";                  //件号
     machCode = "";                //设备编号
     testItemCode = "";            //测试项编号(0不启动，1电流测试,2电压测试)
     judgeEneble = 1;             //判定使能
     judgeMethod = 0;             //判定方式
     errorCode = "";               //错误代码
     standardValue = 0;           //标准值
     maxValue = 0;                //最大值
     minValue = 0;                //最小值
     deviation = 0;               //偏差值
     minPassrate = 0;             //最低合格率
     maxLimit = 0;                //最大超限数
}

/**
 * @brief StartCmdInfo::init 启动命令参数的结构体初始化
 */
void StartCmdInfo::init()
{
    projectNo = "";
    dispatchNo = "";
    partNo = "";
    productNum = 1;
    operationNo = "";
    user = "";
}

/**
 * @brief StartCmdInfo::setField 设置型号、派工单号、件号到启动命令参数结构体变量。
 * @param projectno 型号
 * @param dispatchno 派工单号
 * @param partno 件号
 */
void StartCmdInfo::setField(string projectno,string dispatchno,string partno)
{
    projectNo = projectno;
    dispatchNo = dispatchno;
    partNo = partno;
}

/**
 * @brief StartCmdInfo::setField  设置型号、派工单号、件号、用户名、操作号到启动命令参数结构体变量。
 * @param projectno 型号
 * @param dispatchno 派工单号
 * @param partno 件号
 * @param username 用户名
 * @param operationno 操作号
 */
void StartCmdInfo::setField(string projectno,string dispatchno,string partno,string username,string operationno)
{
    projectNo = projectno;
    dispatchNo = dispatchno;
    partNo = partno;
    user = username;
    operationNo = operationno;             //工序号
}
