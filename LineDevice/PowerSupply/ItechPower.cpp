 #include "ItechPower.h"

/**
 * @brief ItechPower::ItechPower            构造函数
 * @param spp
 */
ItechPower::ItechPower(SerialPortParameters *spp) : ComDevice(spp)
{
    _idn = "*IDN?\n";
    _openOutPut = "OUTP ON\n";
    _setPower = "CURR 2A;VOLT 3V\n";
    _clearError = "*CLS\n";
    _remoteCtrl = "SYST:REM\n";             //这样才能远程使用OUTP命令
}


/**
 * @brief ItechPower::ItechPower        构造函数，初始化命令
 * @param spp                           串口信息句柄
 * @param name                          设备名
 */
ItechPower::ItechPower(SerialPortParameters *spp, string name) : ComDevice(spp, name)
{
    _idn = "*IDN?\n";
    _openOutPut = "OUTP ON\n";
    _setPower = "CURR 2A;VOLT 3V\n";
    _clearError = "*CLS\n";
    _remoteCtrl = "SYST:REM\n";             //这样才能远程使用OUTP命令
}


/**
 * @brief ItechPower::CanAcess              通信测试
 * @return                                  通信正常返回true；否则返回false
 */
bool ItechPower::CanAcess()
{
    string result = ComDevice::WriteAndRead(_idn ,"Ascii",500);
//    cout<<"result = "<<result<<endl;
    if(!result.empty())
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}


/**
 * @brief ItechPower::SetPower              设定电源输出
 */
bool ItechPower::SetPower()
{
    cout<<" ---- ItechPower SetPower CURR 2A;VOLT 3.2V ---- "<<endl;
    return Write("CURR 2A;VOLT 3.2V\n", "Ascii");
}

/**
 * @brief ItechPower::SetRemoteCtrl         设定远程控制模式
 */
bool ItechPower::SetRemoteCtrl()
{
    return Write(_remoteCtrl,"Ascii");
}


/**
 * @brief ItechPower::clearError            清楚错误信息，每次开线初始化必做
 */
bool ItechPower::ClearError()
{
    return Write(_clearError,"Ascii");
}


/**
 * @brief ItechPower::OpenOutput            打开输出
 */
bool ItechPower::OpenOutput()
{
    return Write(_openOutPut,"Ascii");
}


/**
 * @brief ItechPower::CloseOutput
 */
bool ItechPower::CloseOutput()
{
    return Write("OUTP OFF\n","Ascii");
}

bool ItechPower::Init()
{
    if(!ClearError() || !SetRemoteCtrl() || !OpenOutput()|| !SetPower())
        return false;
    else
        return true;
}

