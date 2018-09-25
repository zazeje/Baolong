#include "ItechPowerTcp.h"


/**
 * @brief ItechPowerTcp::ItechPowerTcp            构造函数
 * @param spp
 */
ItechPowerTcp::ItechPowerTcp(int port , string ip) : TcpDevice(port, ip)
{
    _idn = "*IDN?\n";
    _openOutPut = "OUTP ON\n";
    _setPower = "CURR 2A;VOLT 3V\n";
    _clearError = "*CLS\n";
    _remoteCtrl = "SYST:REM\n";             //这样才能远程使用OUTP命令
}


/**
 * @brief ItechPowerTcp::ItechPowerTcp        构造函数，初始化命令
 * @param spp                           串口信息句柄
 * @param name                          设备名
 */
ItechPowerTcp::ItechPowerTcp(int port , string ip, string name) : TcpDevice(port, ip, name)
{
    _idn = "*IDN?\n";
    _openOutPut = "OUTP ON\n";
    _setPower = "CURR 2A;VOLT 3V\n";
    _clearError = "*CLS\n";
    _remoteCtrl = "SYST:REM\n";             //这样才能远程使用OUTP命令
}


/**
 * @brief ItechPowerTcp::CanAcess              通信测试
 * @return                                  通信正常返回true；否则返回false
 */
bool ItechPowerTcp::CanAcess()
{
    string result = WriteAndRead(_idn ,"Ascii",500);
//    cout<<"result = "<<result<<endl;
    if(!result.empty())
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}


/**
 * @brief ItechPowerTcp::SetPower              设定电源输出
 */
bool ItechPowerTcp::SetPower()
{
    cout<<" ---- ItechPowerTcp SetPower CURR 2A;VOLT 3.2V ---- "<<endl;
    return Write("CURR 2A;VOLT 3.2V\n", "Ascii");
}

/**
 * @brief ItechPowerTcp::SetRemoteCtrl         设定远程控制模式
 */
bool ItechPowerTcp::SetRemoteCtrl()
{
    return Write(_remoteCtrl,"Ascii");
}


/**
 * @brief ItechPowerTcp::clearError            清楚错误信息，每次开线初始化必做
 */
bool ItechPowerTcp::ClearError()
{
    return Write(_clearError,"Ascii");
}


/**
 * @brief ItechPowerTcp::OpenOutput            打开输出
 */
bool ItechPowerTcp::OpenOutput()
{
    return Write(_openOutPut,"Ascii");
}


/**
 * @brief ItechPowerTcp::CloseOutput
 */
bool ItechPowerTcp::CloseOutput()
{
    return Write("OUTP OFF\n","Ascii");
}

bool ItechPowerTcp::Init()
{
    if(!ClearError() || !SetRemoteCtrl() || !OpenOutput()|| !SetPower())
        return false;
    else
        return true;
}
