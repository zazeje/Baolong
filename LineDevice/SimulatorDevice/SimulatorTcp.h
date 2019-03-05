#ifndef SIMULATORTCP_H
#define SIMULATORTCP_H

#include "LineDevice/Device/TcpDevice.h"

class SimulatorTcp : public TcpDevice
{
public:

    SimulatorTcp(int port , string ip);      //构造函数
    SimulatorTcp(int port , string ip, string name);
    ~SimulatorTcp();

    bool CanAcess();
    bool SendSimulateData();


    string yxms;//'运行模式，0单次，1循环',
    string sjkz;//'衰减控制，步进0.5dB',
    string dk;//'带宽，步进1MHz',
    string mk;//'脉宽，步进0.1us'
    string mbjl1;//'目标距离1，步进0.1m',
    string mbjl2;//'目标距离2，步进0.1m',
    string mbjl3;//'目标距离3，步进0.1m',
    string mbjl4;//'目标距离4，步进0.1m',
    string mbsd1;//'目标速度1，步进0.01m/s',
    string mbsd2;//'目标速度2，步进0.01m/s',
    string mbsd3;//'目标速度3，步进0.01m/s',
    string mbsd4;//'目标速度4，步进0.01m/s',
    string mbkg1;//'目标开关1，0关，1开',
    string mbkg2;//'目标开关2，0关，1开',
    string mbkg3;//'目标开关3，0关，1开',
    string mbkg4;//'目标开关4，0关，1开',
    string jg;//'间隔，步进1ms',
    string zxpl;//'中心频率，步进1MHz',
    string jlbc;//'距离补偿，步进0.1m',
    string plbc1;//'频率补偿1',
    string plbc2;//'频率补偿2',
    string plbc3;//'频率补偿3',
    string plbc4;//'频率补偿4',
    bool setPara(vector<string> Para);
    string CreateCommand();

private:

};


#endif // SIMULATORTCP_H
