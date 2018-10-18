#include "PlcPointInfo.h"

PlcPointInfo::PlcPointInfo()
{
    m_pointName = "";
    m_registerType = "";
    m_registerAddr = 0;
    m_pointNum = 0;
    m_pointType = 0;
    m_pointCoefficient = 1;
    m_pointUnit = "";
//    m_pointValue = 0;
}

void PlcPointInfo::clear()
{
    m_pointName.clear();
    m_registerType.clear();
    m_registerAddr = 0;
    m_pointNum = 0;
    m_pointType = 0;
    m_pointCoefficient = 1;
    m_pointUnit = "";
//    m_pointValue = 0;
}

int PlcPointInfo::TransPointType(string type)
{
    if(type == "显示")
        return 1;
    else if(type == "机械动作")
        return 2;
    else if(type == "参数")
        return 3;
}
