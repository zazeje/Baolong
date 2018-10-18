#include "DeviceConfig.h"

int StationInfo::ProductCount = 0;

void Tag::Init()
{
    Name = "";
    Description = "";
    Address = 0;
    LogicalMode = "None";
    TagCode = "";
    TagName = "";
    MemortValue = "";
    PlcValue = "";
    ControlTagCount = 0;
    CTagNames.clear();
}

void UnitInfo::Init()
{
     Name = "";
     Desc = "";
     Enable = false;
     StartAddress = 0;
     Length = 0;
     Tags.clear();
}

void DeviceInfo::Init()
{
     Name = "";
     DeviceCode = "";
     Prefix = "";
     Enable = true;
     Ip = "";
     Port = "";
     BaudRate = "";
     DataBits = "";
     StopBits = "";
     Parity = "";
     StartFlag = "";
     SnFlag = "";
     IdFlag = "";
     Units.clear();
     StateFlag = "";
     currentOperationNo = "";          //当前工序号
     machDriverNo = "";                //设备驱动编号
//     devInitFlag = true;
//     devParaFlag = true;
//     testPartFlag = true;
}

void StationInfo::Init()
{
     Name = "";
     Prefix = "";
     StartFlag = "";
     SnFlag = "";
     IdFlag = "";
     Dis.clear();
     IpEth0 = "";
     IpEth1 = "";
     StateFlag = "";
     di.Init();
}

void LineInfo::Init()
{
    Name = "";
    Si.Init();
}

void ServerProductInfo::init()
{
    partSeqNo = "";
    partId = "";
    message = "";
    name = "";
    position = "";
    collectValue = "";
    collectValue1 = "";
    collectValue2 = "";
}
