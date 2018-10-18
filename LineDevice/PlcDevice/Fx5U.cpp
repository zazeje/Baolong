#include "Fx5U.h"


/**
 * @brief Fx5U::Fx5U
 * @param spp
 * @param deviceName
 */
Fx5U::Fx5U(SerialPortParameters *spp, string deviceName, bool blockMode) : PlcBase(spp,deviceName,blockMode)
{
//    string _framehead = "F9{0}00FF00";      //帧识别编号-指定从外部设备(PC)访问的站-网络编号-PLC站号-可编程控制器编号-本站站号
    _framehead = "F90000FF00";       //帧识别编号-指定从外部设备(PC)访问的站-网络编号-PLC站号-可编程控制器编号-本站站号
    _readByBit = "04010001";         //指令（批量读取）- 子指令（按位）
    _readByWord = "04010000";        //指令（批量读取）- 子指令（按字/16位）
    _writeByBit = "14010001";        //指令（批量写入）- 子指令（按位）
    _writeByWord = "14010000";       //指令（批量写入）- 子指令（按字/16位）
    _registerM = "M*";               //软元件M
    _registerD = "D*";               //软元件D
    _delay = 70;

    _ENQ = (char)5;                    //请求：请求发送
    _ACK = (char)6;                    //PLC正确响应：响应的校验通过,即帧有效,可以正确读取
    _NAK = (char)15;                   //PLC错误响应:没通过校验，帧无效,给串口发送方的回应头
    _STX = (char)2;                    //报文开始:回答或响应的开头
    _ETX = (char)3;

    m_communicateStatus = true;

    pthread_rwlock_init(&m_lock,NULL);
}

/**
 * @brief Fx5U::Fx5U
 * @param spp
 * @param stationNum
 * @param deviceName
 */
Fx5U::Fx5U(SerialPortParameters *spp, int stationNum, string deviceName, bool blockMode)
      :PlcBase(spp,deviceName,blockMode)
{
//    string _framehead = "F9{0}00FF00";      //帧识别编号-指定从外部设备(PC)访问的站-网络编号-PLC站号-可编程控制器编号-本站站号
    _framehead = "F9" + padZero(dectoHex(stationNum),2) + "00FF00";         //帧识别编号-指定从外部设备(PC)访问的站-网络编号-PLC站号-可编程控制器编号-本站站号
    _readByBit = "04010001";                                     //指令（批量读取）- 子指令（按位）
    _readByWord = "04010000";                                    //指令（批量读取）- 子指令（按字/16位）
    _writeByBit = "14010001";                                    //指令（批量写入）- 子指令（按位）
    _writeByWord = "14010000";                                   //指令（批量写入）- 子指令（按字/16位）
    _registerM = "M*";                                           //软元件M
    _registerD = "D*";                                           //软元件D
    _delay = 70;

    _ENQ = (char)5;                    //请求：请求发送
    _ACK = (char)6;                    //PLC正确响应：响应的校验通过,即帧有效,可以正确读取
    _NAK = (char)15;                   //PLC错误响应:没通过校验，帧无效,给串口发送方的回应头
    _STX = (char)2;                    //报文开始:回答或响应的开头
    _ETX = (char)3;

    m_communicateStatus = true;

    pthread_rwlock_init(&m_lock,NULL);
}


/**
 * @brief Fx5U::CanAcess
 * @return
 */
bool Fx5U::CanAcess()
{
    string result = ReadBitBy232(119, 1);
    if(!result.compare("3"))
        m_communicateStatus = false;
    else
        m_communicateStatus = true;
    return m_communicateStatus;
}


/**
 * @brief Fx5U::ReadBitBy232                    批量读位元件，目前仅支持M
 * @param address
 * @param length
 * @return
 */
string Fx5U::ReadBitBy232(int address, int length)
{
    string result;
    int tmpLength = length;
    do
    {
        int readLength = tmpLength > 255 ? 255 : tmpLength;
        //读取字数（一字16位2个字节）
        int bytes = readLength % 16 == 0 ? readLength / 16 : readLength / 16 + 1;
        //起始地址格式化，6位10进制地址
        char base[10]={0};
        sprintf(base, "%d",address);
        string baseAddress = padZero(base,6);
        //读取点数格式化，4位16进制
        string len = padZero(dectoHex(readLength), 4);

        string command = _framehead + _readByBit + _registerM + baseAddress + len;
        string checkSum = GetSumCheckByAsciiStr(command);
        command = _ENQ + command + checkSum;

        int delay = 60 + /*((bytes % 20 == 0) ? bytes / 20 : (bytes / 20) + 1)*/bytes * 40;
        string buff = WriteAndRead(command , "Ascii", delay);

        if(!buff.empty() && buff[0] == _STX)
        {
            if(buff.length() == 14 + readLength)
                result.append(buff.substr(11,readLength));
            else
                break;
        }
        address += readLength;
        tmpLength -= readLength;
    }while(tmpLength > 0);
    if(result.size() != length)
    {
        string value(length,'3');
        result = value;
    }
    return result;
}

/**
 * @brief Fx5U::WriteBitBy232           批量写位元件，目前仅支持M
 * @param address
 * @param value
 * @return
 */
bool Fx5U::WriteBitBy232(int address, string value)
{
    //起始地址格式化，6位10进制地址
    char base[1024]={0};
    sprintf(base, "%d",address);
    string baseAddress = padZero(base,6);
    //读取点数格式化，4位16进制
    string len = padZero(dectoHex(value.length()), 4);
    //读取字数（一字16位2个字节）
    int bytes = value.length() % 16 == 0 ? value.length() / 16 : value.length() / 16 + 1;

    string command = _framehead + _writeByBit + _registerM + baseAddress + len + value;
    string checkSum = GetSumCheckByAsciiStr(command);
    command = _ENQ + command + checkSum;

    int delay = 60 + ((bytes % 20 == 0) ? bytes / 20 : (bytes / 20) + 1) * 40;
    string result = WriteAndRead(command, "Ascii" , delay);
    if(!result.empty() && result[0] == _ACK)
        return true;
    else
        return false;
}

/**
 * @brief 232协议读字元件
 * @param address           地址
 * @param length            长度
 * @param registerBytes     寄存器字节数
 * @return
 */
vector<int> Fx5U::ReadWordBy232(int address, int length, RegisterBytes registerBytes)
{
    vector<int> result;
    int tmpLength = length;
    do
    {
        int readLength = tmpLength > 63 ? 63 : tmpLength;
        //起始地址格式化，6位10进制地址
        char base[1024]={0};
        sprintf(base, "%d",address);
        string baseAddress = padZero(base,6);
        //读取字数（一字16位2个字节）
        int bytes = (int)registerBytes * readLength;
        //读取点数格式化，4位16进制
        string len = padZero(dectoHex(bytes / 2), 4);
        string command = _framehead + _readByWord + _registerD + baseAddress + len;
        string checkSum = GetSumCheckByAsciiStr(command);
        command = _ENQ + command + checkSum;

        int delay = (43 + 4 * bytes) + 160;
        string value = WriteAndRead(command , "Ascii" , delay);
        if(value.length() > 0 && value.at(0) == _STX)
        {
            if((value.length() == 14 + bytes * 2) && value[11 + bytes * 2] == _ETX)
            {
                switch(registerBytes)
                {
                    case TWO:
                        for(int i = 0;i < readLength;i++)
                        {
                            result.push_back(hextodec16(value.substr(11 + i*4 ,4)));
                        }
                    break;
                    case FOUR:
                        for(int i = 0;i < readLength;i++)
                        {
                            string hex = value.substr(11 + i*8, 8);
                            result.push_back(hextodec(hex.substr(4,4) + hex.substr(0,4)));
                        }
                    break;
                }
            }
        }
        address += readLength;
        tmpLength -= readLength;
    }while(tmpLength > 0);
    if(result.size() != length)
    {
        vector<int> value(length,-1);
        result = value;
    }
    return result;
}

/**
 * @brief 232协议写字元件
 * @param address       地址
 * @param value         写入值
 * @param registerBytes 寄存器字节数
 * @return
 */
bool Fx5U::WriteWordBy232(int address, vector<int> value, RegisterBytes registerBytes)
{
    if(value.empty())
        return false;
    //起始地址格式化，6位10进制地址
    char base[1024]={0};
    sprintf(base, "%d",address);
    string baseAddress = padZero(base,6);
    //读取字数（一字16位2个字节）
    int bytes = (int)registerBytes * value.size();
    //读取点数格式化，4位16进制
    string len = padZero(dectoHex(bytes/2), 4);
    int delay = (43 + 4 * bytes) + 160;
    string hexValue;
    switch(registerBytes)
    {
        case TWO:
            for(int i=0;i<value.size();i++)
            {
                hexValue.append(padZero(dectoHex(toInt16(value[i])), 4));
            }
        break;
        case FOUR:
            for(int i=0;i<value.size();i++)
            {
                 //高低位交换
                string hex = padZero(dectoHex(value[i]), 8);
                string firstByte = hex.substr(4,4);
                string secondByte = hex.substr(0,4);
                hexValue.append(firstByte).append(secondByte);
            }
        break;
    }

    string command = _framehead + _writeByWord + _registerD + baseAddress + len + hexValue;
    string checkSum = GetSumCheckByAsciiStr(command);
    command = _ENQ + command + checkSum;

    string result = WriteAndRead(command, "Ascii", delay);
    if(!result.empty() && result[0] == _ACK)
        return true;
    else
        return false;
}

/**
 * @brief 批量读取字元件
 * @param address   起始地址
 * @param length    长度
 * @return
 */
vector<int> Fx5U::ReadWord(int address, int length)
{
    vector<int> res;
    if(m_communicateStatus)
    {
        pthread_rwlock_wrlock(&m_lock);
        res = ReadWordBy232(address, length, FOUR);
        pthread_rwlock_unlock(&m_lock);
    }
    return res;
}

/**
 * @brief 写字元件
 * @param address   地址
 * @param value     写入值
 * @return
 */
bool Fx5U::WriteWord(int address, vector<int> value)
{
    bool res = false;
    if(m_communicateStatus)
    {
        pthread_rwlock_wrlock(&m_lock);
        res = WriteWordBy232(address, value, FOUR);
        pthread_rwlock_unlock(&m_lock);
    }
    return res;
}

/**
 * @brief 批量读取位元件
 * @param address       起始地址
 * @param length        长度
 * @param registerType  寄存器类型
 * @return
 */
string Fx5U::ReadBits(int address, int length, string registerType)
{
    string res = "";
    if(m_communicateStatus)
    {
        pthread_rwlock_wrlock(&m_lock);
        if(registerType == "M")
            res = ReadBitBy232(address, length);
        pthread_rwlock_unlock(&m_lock);
    }
    return res;
}

/**
 * @brief 写位元件
 * @param address   地址
 * @param value     值
 * @param registerType  寄存器类型
 * @return
 */
bool Fx5U::WriteBits(int address, string value, string registerType)
{
    bool res = false;
    if(m_communicateStatus)
    {
        pthread_rwlock_wrlock(&m_lock);
        if(registerType == "M")
            res = WriteBitBy232(address, value);
        pthread_rwlock_unlock(&m_lock);
    }
    return res;
}

