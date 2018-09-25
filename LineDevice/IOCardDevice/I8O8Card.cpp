#include "I8O8Card.h"

/**
 * @brief I808Card::I808Card            根据COM对象和设备名称初始化COM设备对象
 * @param spp                           Com对象
 * @param deviceName                    设备名称
 */
I808Card::I808Card(SerialPortParameters* spp, string deviceName) : ComDevice(spp , deviceName)
{
    _controlCommand = "005A56{0}{1}{2}0000";
    _readStateCommand = "005A56{0}07000000";
    //responsePrefix = "005A56{0}2000"; //后面是y,x,check
    getAddressCmd = "005A56FF10000000BF";
}

/**
 * @brief I808Card::I808Card            根据COM端口和设备名称初始化COM设备对象
 * @param portName                      COM端口名称
 * @param deviceName                    设备名称
 */
I808Card::I808Card(string portName, string deviceName) : ComDevice(new SerialPortParameters(portName), deviceName)
{
    _controlCommand = "005A56{0}{1}{2}0000";
    _readStateCommand = "005A56{0}07000000";
    //responsePrefix = "005A56{0}2000"; //后面是y,x,check
    getAddressCmd = "005A56FF10000000BF";
}

/**
 * @brief I808Card::~I808Card           析构函数
 */
I808Card::~I808Card()
{}

/**
 * @brief I808Card::CanAcess            通信检测
 * @return                              读继电器状态，状态非空返回true，否则返回false
 */
bool I808Card::CanAcess()
{
    if(!ReadYStatus().empty())
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}

/**
 * @brief I808Card::ControlY            控制继电器
 * @param position                      继电器位置
 * @param isClose                       开关类型
 * @return
 */
string I808Card::ControlY(int position, bool isClose)
{
    //仅考虑8路
    string positonHex = "0" + IntToString(position);
    string controlMode = isClose ? "02" : "01";
    string command = _controlCommand + cardAddress + controlMode + positonHex;
    command = command + GetSumCheckByHexStr(command);
    string statusHex = WriteAndRead(command, "Hex", 200);
    if (statusHex.length() == 18)
    {
        string statusStr = padZero(intToBinaryStr(hextodec(statusHex.substr(12, 2), 2)), 8);
        reverse(statusStr.begin(),statusStr.end());
        return statusStr;
    }
    return "";
}

/**
 * @brief I808Card::GetCardAddress      获取地址
 */
void I808Card::GetCardAddress()
{
    string result = WriteAndRead(getAddressCmd,"Hex",200);
    if (result.length() == 18)
    {
        cardAddress = result.substr(6, 2);
    }
}

/**
 * @brief I808Card::OpenY               开继电器
 * @param position                      继电器位置
 * @return
 */
string I808Card::OpenY(int position)
{
    return ControlY(position,false);
}

/**
 * @brief I808Card::OpenY               开继电器
 * @param position                      继电器位置
 * @return
 */
string I808Card::OpenY(string position)
{
    return ControlY(atoi(position.data()), false);
}


/**
 * @brief I808Card::CloseY              关继电器
 * @param position                      继电器位置
 * @return
 */
string I808Card::CloseY(int position)
{
    return ControlY(position,true);
}

/**
 * @brief I808Card::CloseY              关继电器
 * @param position                      继电器位置
 * @return
 */
string I808Card::CloseY(string position)
{
    return ControlY(atoi(position.data()), true);
}

/**
 * @brief I808Card::ControlAll          控制所有继电器
 * @param IsClose                       开关类型
 */
void I808Card::ControlAll(bool IsClose)
{
    string controlMode = IsClose ? "04" : "03";
    string command = _controlCommand + cardAddress + controlMode + "00";
    command = command + GetSumCheckByHexStr(command);
    WriteHex(command);
}

/**
 * @brief I808Card::ControlSome         控制部分继电器
 * @param positions                     继电器位置字符串
 * @param isClose                       开关类型
 */
void I808Card::ControlSome(string positions, bool isClose)
{
    //仅考虑8路
    int pos = binaryStrToInt(positions);
    char tmp[10] = {0};
    sprintf(tmp,"%x",pos);
    string posHex = tmp;
    if (posHex.length() == 1)
    {
        posHex = "0" + posHex;
    }
    string controlMode = isClose ? "06" : "05";
    string command = _controlCommand + cardAddress + controlMode + posHex;
    command = command + GetSumCheckByHexStr(command);
    WriteHex(command);
}

/**
 * @brief I808Card::ReadYStatus         读Y继电器状态
 * @return
 */
string I808Card::ReadYStatus()
{
    string command = _readStateCommand + cardAddress;
    command = command + GetSumCheckByHexStr(command);
    string statusHex = WriteAndRead(command, "Hex", 200);
    if (statusHex.length() == 18)
    {
        string hexStr = statusHex.substr(12, 2);
        string statusStr = padZero(intToBinaryStr(hextodec(hexStr,hexStr.length())), 8);
        reverse(statusStr.begin(),statusStr.end());
        return statusStr;
    }
    return "";
}


/**
 * @brief I808Card::ReadXStatus         读X继电器状态
 * @return
 */
string I808Card::ReadXStatus()
{
    string command = _readStateCommand + cardAddress;
    command = command + GetSumCheckByHexStr(command);
    string statusHex = WriteAndRead(command, "Hex", 200);
    if (statusHex.length() == 18)
    {
        string hexString = statusHex.substr(14,2);
        string binaryStr = intToBinaryStr(hextodec(hexString,hexString.length()));
        string statusStr = padZero(binaryStr,8);
        reverse(statusStr.begin(),statusStr.end());
        return statusStr;
    }
    return "";
}
