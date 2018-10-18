#include "DeltaPlc.h"

DeltaPlc::DeltaPlc(SerialPortParameters *spp, string deviceName) : ComDevice(spp,deviceName),registerIndexOutOfRange("RegisterIndexOutOfRange"),
    STX(":"),_cmd01("01"),_cmd02("02"),_cmd03("03"),_cmd05("05"),_cmd06("06"),_cmd15("0F"),
    _cmd16("10"),_cmd17("11"),S0(0),X0(1024),Y0(1280),T0(1536),M0(2046),C0(3584),D0(4096)
{
    _stationNum = "00";
    _delay = 50;
}


DeltaPlc::DeltaPlc(SerialPortParameters *spp, string deviceName,int delay) : ComDevice(spp,deviceName),registerIndexOutOfRange("RegisterIndexOutOfRange"),
    STX(":"),_cmd01("01"),_cmd02("02"),_cmd03("03"),_cmd05("05"),_cmd06("06"),_cmd15("0F"),
    _cmd16("10"),_cmd17("11"),S0(0),X0(1024),Y0(1280),T0(1536),M0(2046),C0(3584),D0(4096)
{
    _stationNum = "00";
    _delay = delay;
}


DeltaPlc::DeltaPlc(SerialPortParameters *spp, int stationNum, string deviceName) : ComDevice(spp,deviceName),registerIndexOutOfRange("RegisterIndexOutOfRange"),
    STX(":"),_cmd01("01"),_cmd02("02"),_cmd03("03"),_cmd05("05"),_cmd06("06"),_cmd15("0F"),
    _cmd16("10"),_cmd17("11"),S0(0),X0(1024),Y0(1280),T0(1536),M0(2046),C0(3584),D0(4096)
{
    _delay = 50;
    char tmp[10] = {0};
    sprintf(tmp,"%02x",stationNum);
    _stationNum = tmp;
}

DeltaPlc::~DeltaPlc()
{}

/**
 * @brief DeltaPlc::CanAcess                通信检测
 */
bool DeltaPlc::CanAcess()
{
    string result = ReadD(1);
    return result.find("3") != string::npos ? false : true;
}

/**
 * @brief DeltaPlc::ReadD                   读多个寄存器
 * @param address                           地址
 * @param length                            长度
 * @return
 */
string DeltaPlc::ReadD(int address, int length)
{
    char* value = new char[length];
    for(int i = 0;i < length;i++)
    {
        value[i] = '3';
    }
    char tmp[10] = {0};
    sprintf(tmp,"%02X",address);
    string baseAddress = "10";
    baseAddress.append(tmp);
    memset(tmp,0,10);
    sprintf(tmp,"%X",length);
    string tmpStr = tmp;
    string len = padZero(tmpStr, 4);
    string command = _stationNum + _cmd03 + baseAddress + len;
    string checkLrc = GetLrcCheck(command);
    command = ":" + command + checkLrc + endStr;
    string result = WriteAndRead(command, "Ascii", 100);
    if(!result.empty() && result.length() == (11 + length * 4))
    {
        for (int i = 0; i < length; i++)
        {
            value[i] = result[6 + (i + 1) * 4];
        }
    }
    string data = value;
    delete value;
    return data;
}

/**
 * @brief DeltaPlc::ReadD                       读单个D寄存器
 * @param address                               寄存器地址
 * @return
 */
string DeltaPlc::ReadD(int address)
{
    char tmp[10] = {0};
    sprintf(tmp,"%02X",address);
    string baseAddress = "10";
    baseAddress.append(tmp);
    string command = _stationNum + _cmd03 + baseAddress + "0001";
    string checkLrc = GetLrcCheck(command);
    command = ":" + command + checkLrc + endStr;
    string result = WriteAndRead(command,"Ascii",_delay);
    if (!result.empty() && result.length() == 15)
    {
        //初步检验为合格响应,PLC返回13个字符+cr lf
        if (result[10] == '1')
        {
            return "1";
        }
        else return "0";
    }
    return "3";
}

/**
 * @brief DeltaPlc::WriteD                      设置单个寄存器
 * @param address                               地址
 * @param state                                 状态
 */
void DeltaPlc::WriteD(int address, string state)
{
    char tmp[10] = {0};
    sprintf(tmp,"%03X",address);
    string baseAddress = "1";
    baseAddress.append(tmp);
    string command = _stationNum + _cmd06 + baseAddress;
    if(state == "1")
    {
        command += "0001";
    }
    else if(state == "0")
    {
        command += "0000";
    }
    string checkLrc = GetLrcCheck(command);
    command = ":" + command + checkLrc + endStr;
    WriteAndRead(command,"Ascii",50);
}

/**
 * @brief DeltaPlc::WriteD                      设置多个寄存器
 * @param address
 * @param state
 * @param length
 */
void DeltaPlc::WriteD(int address, string state, int length)
{

}

/**
 * @brief DeltaPlc::ReadM                       读多个M寄存器
 * @param address                               地址
 * @param length                                长度
 * @return
 */
string DeltaPlc::ReadM(int address, int length)
{
    char* value = new char[length];
    for (int i = 0; i < length; i++)
    {
        value[i] = '3';
    }
    char tmp[10] = {0};
    sprintf(tmp,"%02X",address);
    string baseAddress = "08";
    baseAddress.append(tmp);
    memset(tmp,0,10);
    sprintf(tmp,"%04X",length);
    string len = tmp;
    string command = _stationNum + _cmd01 + baseAddress + len;
    string checkLrc = GetLrcCheck(command);
    command = ":" + command + checkLrc + endStr;
    string result = WriteAndRead(command, "Ascii", _delay);
    if (!result.empty() && result.length() == (length/8 + 1)*2 + 11)
    {
        int count = hextodec(result.substr(5, 2),2);
        string state = "";
        for (int i = 0; i < count; i++)
        {
            //十六进制字符串
            string hexStr = result.substr(7 + (i * 2), 2);
            //十进制数值
            int DecInt = hextodec(hexStr,hexStr.length());
            //二进制字符串
            string bitStr = intToBinaryStr(DecInt);
            //二进制字符串补足8位（前面补零）
            string bStr = padZero(bitStr, 8);
            //2进制字符串转置得到对应结果字符串
            reverse(bStr.begin(),bStr.end());
            state += bStr;
        }
        if (state.length() >= length)
        {
            return state.substr(0, length);
        }
    }
    string data = value;
    delete value;
    return data;
}

/**
 * @brief DeltaPlc::ReadM                       读单个M寄存器
 * @param address                               寄存器地址
 * @return
 */
string DeltaPlc::ReadM(int address)
{\
    char tmp[10] = {0};
    sprintf(tmp,"%02X",address);
    string sAddress = tmp;
    string baseAddress = "08" + sAddress;
    string command = _stationNum + _cmd01 + baseAddress + "0001";
    string checkLrc = GetLrcCheck(command);
    command = ":" + command + checkLrc + endStr;
    string result = WriteAndRead(command, "Ascii", _delay);
    if (!result.empty() && result.length() == 13)
    {
        //初步检验为合格响应,PLC返回13个字符+cr lf
        string bitStr = intToBinaryStr(hextodec(result.substr(7, 2)));
        if(bitStr[bitStr.length() - 1] == '1')
        {
            return "1";
        }
        else if(bitStr[bitStr.length() - 1] == '0')
        {
            return "0";
        }
    }
    return "3";
}

/**
 * @brief DeltaPlc::WriteM                      设置单个M寄存器
 * @param address                               地址
 * @param state                                 状态
 */
void DeltaPlc::WriteM(int address, string state)
{
    char tmp[10] = {0};
    sprintf(tmp,"%02X",address);
    string sAddress = tmp;
    string baseAddress = "08" + sAddress;
    string command = _stationNum + _cmd05 + baseAddress;
    if(state == "1")
    {
        command += "FF00";
    }
    else if(state == "0")
    {
        command += "0000";
    }
    string checkLrc = GetLrcCheck(command);
    command = ":" + command + checkLrc + endStr;
    WriteAndRead(command,"Ascii",_delay);
}

/**
 * @brief DeltaPlc::WriteM                      设置多个寄存器
 * @param address                               地址
 * @param state                                 状态
 * @param length                                长度
 */
void DeltaPlc::WriteM(int address, string state, int length)
{
    char tmp[10] = {0};
    sprintf(tmp,"%02X",address);
    string sAddress = tmp;
    string baseAddress = "08" + sAddress;
    memset(tmp,0,10);
    sprintf(tmp,"%04X",length);
    string len = tmp;
    memset(tmp,0,10);
    sprintf(tmp,"%X",(length / 8 + 1));
    string command = _stationNum + _cmd15 + baseAddress + len + tmp;
    for (int i = 0; i < (length/8 + 1); i++)
    {
        //待完善
        //二进制串
        string bStr = state.substr(0, (i + 1) * 8);
        intToBinaryStr(atoi(state.substr(0, (i + 1) * 8).data()));
    }

    string checkLrc = GetLrcCheck(command);
    command = ":" + command + checkLrc + endStr;
    string result = WriteAndRead(command, "Ascii", _delay);
}

/**
 * @brief DeltaPlc::ReadRegister                读单个寄存器
 * @param address                               地址
 * @param registerType                          寄存器类型
 * @return
 */
string DeltaPlc::ReadRegister(int address,char registerType)
{
    if (registerType == 'M')
    {
        return ReadM(address);
    }
    else if (registerType == 'D')
    {
        return ReadD(address);
    }
    else
        return "";
}

/**
 * @brief DeltaPlc::ReadRegister                读多个寄存器
 * @param address                               地址
 * @param length                                长度
 * @param registerType                          寄存器类型
 * @return
 */
string DeltaPlc::ReadRegister(int address, int length, char registerType)
{
    if (registerType == 'M')
    {
        return ReadM(address, length);
    }
    else if (registerType == 'D')
    {
        return ReadD(address, length);
    }
    else
        return "";
}


/**
 * @brief DeltaPlc::WriteRegister               写入单个寄存器
 * @param address                               地址
 * @param state                                 状态
 * @param registerType                          寄存器类型
 */
void DeltaPlc::WriteRegister(int address, string state, char registerType)
{
    if (registerType == 'M')
    {
        WriteM(address, state);
    }
    else if (registerType == 'D')
    {
        WriteD(address, state);
    }
}

/**
 * @brief DeltaPlc::WriteRegister               写入多个寄存器
 * @param address                               地址
 * @param state                                 状态
 * @param length                                长度
 * @param registerType                          寄存器类型
 */
void DeltaPlc::WriteRegister(int address, string state, int length, char registerType)
{
    if (registerType == 'M')
    {
        WriteM(address, state, length);
    }
    else if (registerType == 'D')
    {
        WriteD(address, state, length);
    }
}


