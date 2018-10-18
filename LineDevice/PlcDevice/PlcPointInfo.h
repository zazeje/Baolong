#ifndef PLCPOINTINFO_H
#define PLCPOINTINFO_H

#include <string>

using namespace std;

//每个PLC点位的信息
class PlcPointInfo
{

public:

    PlcPointInfo();

    string m_pointName;         //PLC点位名称
    string m_registerType;      //PLC点位寄存器类型
    string m_pointUnit;         //PLC项目单位

    int m_registerAddr;         //PLC点位寄存器地址
    int m_pointNum;             //PLC点位序号
    int m_pointType;            //PLC点位类型
    int m_pointCoefficient;     //PLC项目系数

//    int m_pointValue;

    void clear();
    int TransPointType(string type);
};

#endif // PLCPOINTINFO_H
