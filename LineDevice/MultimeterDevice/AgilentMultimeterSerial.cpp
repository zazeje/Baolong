#include "AgilentMultimeterSerial.h"
#define _DelayTime 100

/**
 * @brief 无参构造
 */
AgilentMultimeterSerial::AgilentMultimeterSerial()
{
    DispOn = "DISP:ON\n";
    DispOff = "DISP:OFF\n";
    SysiBeep = "SYSI:BEEP\n";
    MeasVoltDc = "MEAS:VOLT:DC?\n";
    MeasCurrDc = "MEAS:CURR:DC?\n";
    _read = "READ?\n";
    SystErr = "SYST:ERR?\n";
    Idn = "*IDN?\n";
    Cls = "*CLS\n";
    endStr = "\n";
}

/**
 * @brief 通过串口参数构造
 * @param spp 包含串口参数信息的指针
 */
AgilentMultimeterSerial::AgilentMultimeterSerial(SerialPortParameters* spp) : ComDevice(spp)
{
    DispOn = "DISP:ON\n";
    DispOff = "DISP:OFF\n";
    SysiBeep = "SYSI:BEEP\n";
    MeasVoltDc = "MEAS:VOLT:DC?\n";
    MeasCurrDc = "MEAS:CURR:DC?\n";
    _read = "READ?\n";
    SystErr = "SYST:ERR?\n";
    Idn = "*IDN?\n";
    Cls = "*CLS\n";
    endStr = "\n";
}

/**
 * @brief 通过串口参数以及设备名称构造
 * @param spp 包含串口参数信息的指针
 * @param name  设备名称
 */
AgilentMultimeterSerial::AgilentMultimeterSerial(SerialPortParameters* spp, string name) : ComDevice(spp,name)
{
    DispOn = "DISP:ON\n";
    DispOff = "DISP:OFF\n";
    SysiBeep = "SYSI:BEEP\n";
    MeasVoltDc = "MEAS:VOLT:DC?\n";
    MeasCurrDc = "MEAS:CURR:DC?\n";
    _read = "READ?\n";
    SystErr = "SYST:ERR?\n";
    Idn = "*IDN?\n";
    Cls = "*CLS\n";
    endStr = "\n";
}

/**
 * @brief 通信测试
 * @return
 */
bool AgilentMultimeterSerial::CanAcess()
{
    string respones = WriteAndRead(Idn,"Ascii",_DelayTime);
    //修改：二代雨量线使用的万用表型号为Keysight 34450a，发送通讯测试命令回码为Keysight开头的字符串
    //修改人：魏亮
    //修改时间：2016.10.21
    //if (StringToLower(respones).find("keysight") != std::string::npos)
    if(!respones.empty())
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}

/**
 * @brief 读万用表的值
 * @return
 */
string AgilentMultimeterSerial::GetValue()
{
    string result = WriteAndRead(_read,"Ascii",_DelayTime);
    char** str;
    if (!result.empty() && result.length() > 0)
    {
        double num = strtod(Trim(result).data(),str);
        return DigitChange(num,10);
    }
    return "";
}

/**
 * @brief 发送指令到万用表
 * @param 指令内容
 * @return 发送成功返回true，否则返回false
 */
bool AgilentMultimeterSerial::SendCommand(string command)
{
    if (command.empty()) return false;
    return Write(command + endStr, "Ascii");
}

/**
 * @brief 从万用表接收数据
 * @return
 */
string AgilentMultimeterSerial::ReadData()
{
    return ComDevice::Read("Ascii");
}

/**
 * @brief 从万用表读取IDN
 * @return
 */
string AgilentMultimeterSerial::GetIdn()
{
    return WriteAndRead(Idn,"Ascii",_DelayTime);
}

/**
 * @brief 设置万用表是否显示
 * @param display
 * @return
 */
bool AgilentMultimeterSerial::SetDisplay(bool display)
{
    return Write((display ? DispOn : DispOff),"Ascii");
}

/**
 * @brief 读取错误信息
 * @return
 */
string AgilentMultimeterSerial::GetSystemError()
{
    return WriteAndRead(SystErr,"Ascii",_DelayTime);
}

/**
 * @brief 写报警请求
 * @return
 */
bool AgilentMultimeterSerial::Beep()
{
    return Write(SysiBeep,"Ascii");
}

/**
 * @brief 清楚万用表错误信息
 * @return
 */
bool AgilentMultimeterSerial::CleraError()
{
    return Write(Cls,"Ascii");
}
