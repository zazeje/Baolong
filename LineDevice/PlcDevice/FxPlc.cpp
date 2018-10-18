#include "FxPlc.h"
#include "./DataConvert/DataType.h"
#include "../Device/ComDevice.h"
#include "../Device/SerialPortParameters.h"


char ENQ[1] = {(char)5};                    //请求：请求发送
char ACK[1] = {(char)6};                    //PLC正确响应：响应的校验通过,即帧有效,可以正确读取
char NAK[1] = {(char)15};                   //PLC错误响应:没通过校验，帧无效,给串口发送方的回应头
char STX[1] = {(char)2};                    //报文开始:回答或响应的开头
char ETX[1] = {(char)3};

using namespace std;

/**
 * @brief 无参构造
 */
FxPlc::FxPlc()
{}

/**
 * @brief 通过串口参数信息、设备名称以及协议类型进行构造
 * @param spp           包含串口参数信息的指针
 * @param deviceName    设备名称
 * @param protocolMode  协议类型
 */
FxPlc::FxPlc(SerialPortParameters *spp, string deviceName,ProtocolMode protocolMode,bool blockMode)
    :PlcBase(spp,deviceName,blockMode)
{
    _pc = "FF";                      //fx系列都是FF
    _readM = "BR0M";                 //读取辅助继电器 (固定写成了等待时间0ms,M:辅助继电器)
    _writeM = "BW0M";                //写入辅助继电器(固定写成了等待时间0ms,M:辅助继电器)
    _readD = "WR0D";                 //读取字元件D（固定写成等待时间0ms）
    _writeD = "WW0D";                //写入字元件D（固定写成等待时间0ms）
    _onePoint = "01";                //用于读写一个点
    _programPortReadCmd = "E00";
    _programPortWriteCmd = "E10";
    _programPortForceONCmd = "E7";    //按照协议文档，此处指令为"7"，根据串口监视工具破解指令，此处应该为"E7"
    _programPortForceOFFCmd = "E8";   //按照协议文档，此处指令为"8"，根据串口监视工具破解指令，此处应该为"E8"

    _delay = 100;

    _stationNum = "00";
    _protocolMode = protocolMode;
    m_communicateStatus = true;

    pthread_rwlock_init(&m_lock,NULL);
}

/**
 * @brief 通过串口参数信息、站号、设备名称以及协议类型进行构造
 * @param spp
 * @param stationNum
 * @param deviceName
 * @param protocolMode
 */
FxPlc::FxPlc(SerialPortParameters *spp, int stationNum, char* deviceName, ProtocolMode protocolMode,bool blockMode)
    :PlcBase(spp,deviceName,blockMode)
{
    _pc = "FF";                      //fx系列都是FF
    _readM = "BR0M";                 //读取辅助继电器 (固定写成了等待时间0ms,M:辅助继电器)
    _writeM = "BW0M";                //写入辅助继电器(固定写成了等待时间0ms,M:辅助继电器)
    _readD = "WR0D";                 //读取字元件D（固定写成等待时间0ms）
    _writeD = "WW0D";                //写入字元件D（固定写成等待时间0ms）
    _onePoint = "01";                //用于读写一个点
    _programPortReadCmd = "E00";
    _programPortWriteCmd = "E10";
    _programPortForceONCmd = "E7";    //按照协议文档，此处指令为"7"，根据串口监视工具破解指令，此处应该为"E7"
    _programPortForceOFFCmd = "E8";   //按照协议文档，此处指令为"8"，根据串口监视工具破解指令，此处应该为"E8"
    _stationNum = "";                           //十六进制两位字符 485最多32个 422最多10个

    _delay = 100;
    char m_stationNum[10] = {0};
    sprintf(m_stationNum,"%X",stationNum);
    _stationNum = (char *)padZero(m_stationNum, 2).data();
    _protocolMode = protocolMode;
    m_communicateStatus = true;

    pthread_rwlock_init(&m_lock,NULL);
}


/**
 * @brief FxPlc::ReadWordBy232      批量读取字元件寄存器D
 * @note                            注意:批量读取最大点数为64，点数为64时延迟时间至少为290ms
 * @param address                   起始地址
 * @param length                    点数
 * @return
 */
vector<int> FxPlc::ReadWordBy232(int address, int length, RegisterBytes registerBytes)
{
    //默认值为3
    vector<int> value(length,3);
    int tmpLength = length;
    int alreadyLength = 0;
    int delay = 0;
    do
    {
        //字元件一次性最多读取128个字节，所以一次性最多只能读取64个字元件
        int readLength = tmpLength * registerBytes / 2;
        readLength = readLength > 64 ? 64 : readLength;
        char m_address[10] = {0};
        char m_length[10] = {0};
        //地址补齐4位
        sprintf(m_address,"%d",address);
        string baseAddress = padZero(m_address,4);
        //长度转换为16进制，补齐2位。这里的读取长度需要以16位字元件为单位进行计算
        sprintf(m_length,"%X",readLength);
        string len = padZero(m_length,2);
        //指令内容
        string command = _stationNum + _pc + _readD + baseAddress + len;
        //和校验
        string checkSum = GetSumCheckByAsciiStr(command);
        //完整指令拼接
        command = ENQ + command + checkSum;
        //延迟时间处理，与读取点位多少相关，初略算法
        delay = 75 + readLength * 5;
        string result = WriteAndRead(command, "Ascii", delay);
        if(result.length() > 0 && result[0] == STX[0])
        {
            //1个寄存器元件的值占2个字节
            if(result.length() == (8 + readLength * 4) && result.data()[5 +  readLength * 4] == ETX[0])
            {
//                _log.LOG_DEBUG("ReadWordBy232 数据读取成功，延时时间【%d】 ms",delay);
                switch(registerBytes)
                {
                    case TWO:
                        for(int i=0; i<readLength; i++)
                        {
                            value[i + alreadyLength] = hextodec16(result.substr(1 + (i+1)*4,4));
                        }
                        alreadyLength += readLength;
                        tmpLength -= readLength;
                     break;
                     //针对两个字元件代表32位整数的情况时，需要把以16位为单位读取的数据进行换算得到32位数据
                     case FOUR:
                         for(int i = 0;i < readLength / 2; i++)
                         {
                             string hex = result.substr(5 + i*8,8);
                             string hi = hex.substr(4,4);
                             string lo = hex.substr(0,4);
                             string v = hi + lo;
                             int tmp = hextodec(v);
                             value[i + alreadyLength] = tmp;
                         }
                        alreadyLength += readLength / 2;
                        tmpLength -= readLength / 2;
                    break;
                default:
                    break;
                }
            }
            else
                break;
        }
        else
            break;
        address += readLength;
    }while(tmpLength > 0);
    if(value.size() != length)
        _log.LOG_DEBUG("ReadWordBy232 数据读取失败，延时时间【%d】 ms",delay);
    return value;
}


/**
 * @brief FxPlc::WriteWordBy232         批量写入字元件D寄存器
 * @param address                       起始地址
 * @param value                         代写入值
 * @return
 */
bool FxPlc::WriteWordBy232(int address, vector<int> value)
{
    if(value.empty())
        return false;
    //地址补齐4位
    char m_address[10] = {0};
    sprintf(m_address,"%d",address);
    string baseAddress = padZero(m_address,4);
    string hexValue;
    vector<short int> value16(value.size());
    //将写入的值转换为4位16进制字符串，且统一拼接到一个字符串中
    char m_value16[10] = {0};
    for(int i = 0; i<value.size(); i++)
    {
        value16[i] = value[i];
        sprintf(m_value16,"%04X",value16[i]);
        hexValue += m_value16;
    }
    char len[10] = {0};
    sprintf(len, "%02X", value.size());
    //指令内容
    string command = _stationNum + _pc + _writeD + baseAddress + len + hexValue;
    string checkSum = GetSumCheckByAsciiStr(command);
    command = ENQ + command + checkSum;
    //延迟时间处理，与读取点位多少相关，粗略算法
    int delay = 30 + ((value.size() % 10 == 0) ? value.size() / 10 : (value.size() / 10) + 1) * 40;
    string result = WriteAndRead(command, "Ascii", 100);
//    _log.LOG_DEBUG("--- result = 【%s】",result.data());
    if(result[0] == ACK[0])
        return true;
    else
        return false;
}


/**
 * @brief FxPlc::ReadBitBy232       使用计算机转悠协议批量读取连续M点地址，适用RS232、RS485通信
 * @param address                   地址
 * @param length                    状态值
 * @return
 */
string FxPlc::ReadBitBy232(int address, int length)
{
    string result = "";
    int delay = 0;
    int tmpLength = length;
    if(length == 0)
        return result;
    //经测试232通信模式下，最多一次性连续读255个M点
    //如果剩余长度仍大于零则继续读取
    do
    {
        //总长度大于255则读取长度为255，否则等于总长度
        int readLength = tmpLength > 255 ? 255 : tmpLength;
        int bytes = readLength % 8 == 0 ? \
                    readLength / 8 : readLength / 8 + 1;
        char m_address[10] = {0};
        char m_length[10] = {0};
        //地址补齐4位
        sprintf(m_address,"%d",address);
        string baseAddress = padZero(m_address,4);
        //长度转换为16进制，补齐2位
        sprintf(m_length,"%X",readLength);
        string len = padZero(m_length,2);
        string command = _stationNum + _pc + _readM + baseAddress + len;
        string checkSum = GetSumCheckByAsciiStr(command);
        command = ENQ + command + checkSum;
        delay = 75 + bytes * 10;
        string buff = WriteAndRead(command, "Ascii",delay);
        if(!buff.empty() && buff.data()[0] == STX[0])
        {
            if(buff.size() == 8 + readLength)
            {
                result.append(buff.substr(5, readLength));
//                _log.LOG_DEBUG("ReadBitBy232 数据读取成功，延时时间【%d】 ms",delay);
            }
            else
                break;
        }
        address += readLength;
        tmpLength -= readLength;
    }while(tmpLength > 0);
    if(result.size() != length)
    {
        _log.LOG_DEBUG("ReadBitBy232 数据读取失败，延时时间【%d】 ms",delay);
        string value(length,'3');
        result = value;
    }
    return result;
}


/**
 * @brief FxPlc::WriteBitBy232      使用计算机专有协议写辅助继电器M地址，适用RS232，485通信
 * @param address                   地址
 * @param state                     状态值
 * @return
 */
bool FxPlc::WriteBitBy232(int address, string state)
{
    char m_address[10] = {0};
    char m_length[10] = {0};
    sprintf(m_address,"%d",address);
    string baseAddress = padZero(m_address,4);
    sprintf(m_length,"%X",state.length());
    string len = padZero(m_length,2);
    string command = _stationNum + _pc + _writeM + baseAddress + len + state;
    string checkSum = GetSumCheckByAsciiStr(command);
    command = ENQ + command + checkSum;
    string result = ComDevice::WriteAndRead((char *)command.data(), "Ascii", _delay);
    if(!result.empty() && result.data()[0] == ACK[0])
        return true;
    else
        return false;
}


/**
 * @brief FxPlc::ReadWordBy422      批量读字元件（D寄存器）
 * @param address                   起始地址
 * @param length                    寄存器个数（单个寄存器占2个字节时最大127，单个寄存器占4个字节时最大63）
 * @param registerBytes             单个寄存器字节数，2或者4,2表示16位有符号整数，4表示32位有符号整数
 * @return
 */
vector<int> FxPlc::ReadWordBy422(int address, int length, RegisterBytes registerBytes)
{
    vector<int> result;
    result.clear();
    int tmpLength = length;
    //读取字元件一次性最大读取63个长度
    do
    {
        int readLength = tmpLength > 63 ? 63 : tmpLength;
        char m_bytes[10] = {0};
        int bytes = (int)registerBytes * readLength;
        sprintf(m_bytes, "%02X", bytes);
        char group[10] = {0};
        sprintf(group,"%04X",address * 2 + 16384);
        string groupAddress = group;
        string command = _programPortReadCmd + groupAddress + m_bytes + ETX;
        string checkSum = GetSumCheckByAsciiStr(command);
        command = STX + command + checkSum;
        //延迟时间处理，与读取字节数多少相关，初略算法
        int delay = 30 + bytes * 40;
        string value = WriteAndRead(command, "Ascii", delay);
        if(value.length() > 0 && value.data()[0] == STX[0])
        {
            if((value.length() == 4 + bytes*2) && (value.data()[1 + bytes*2] == ETX[0]))
            {
                switch(registerBytes)
                {
                    case TWO:
                         for(int i=0; i< readLength; i++)
                         {
                             string hex = value.substr(1 + i*4,4);
                             result.push_back(hextodec16(hex.substr(2,2) + hex.substr(0,2)));
                         }
                     break;
                     case FOUR:
                         for(int i = 0;i < readLength; i++)
                         {
                             string hex = value.substr(1 + i*8,8);
                             string hi = hex.substr(4,4);
                             string lo = hex.substr(0,4);
                             string v = hi.substr(2,2) + hi.substr(0,2) + lo.substr(2,2) + lo.substr(0,2);
                             int tmp = hextodec(v);
                             result.push_back(tmp);
                         }
                    break;
                default:
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
 * @brief FxPlc::WriteWordBy422         批量写字元件（D寄存器）
 * @param address                       起始地址
 * @param value                         寄存器个数（单个寄存器占2个字节时最大127，单个寄存器占4个字节时最大63）
 * @param registerBytes                 单个寄存器字节数，2或者4,2表示16位有符号整数，4表示32位有符号整数
 * @return
 */
bool FxPlc::WriteWordBy422(int address, vector<int> value, RegisterBytes registerBytes)
{
    if(value.empty())
        return false;
    char group[10] = {0};
    sprintf(group,"%04X",address * 2 + 16384);
    string groupAddress = group;
    char m_bytes[10] = {0};
    int bytes = (int)registerBytes * value.size();
    sprintf(m_bytes, "%02X", bytes);

    string hexValue;
    char m_value[10] = {0};
    switch(registerBytes)
    {
        case TWO:
             for(int i=0; i < value.size(); i++)
             {
                 sprintf(m_value,"%04X",toInt16(value[i]));
                 string hex = m_value;
                 string firstByte = hex.substr(2,2);
                 string secondByte = hex.substr(0,2);
                 hexValue.append(firstByte).append(secondByte);
             }
             break;
        case FOUR:
            for (int i = 0; i < value.size(); i++)
            {
                //高低位交换
                sprintf(m_value,"%08X",value[i]);
                string hex = m_value;
                string firstByte = hex.substr(6, 2);
                string secondByte = hex.substr(4, 2);
                string threeByte = hex.substr(2, 2);
                string fourByte = hex.substr(0, 2);
                hexValue.append(firstByte).append(secondByte).append(threeByte).append(fourByte);
            }
        break;
        default:
            break;
    }
    //指令内容
    string command = _programPortWriteCmd + groupAddress + m_bytes + hexValue + ETX;
    string checkSum = GetSumCheckByAsciiStr(command);
    command = STX + command + checkSum;
    //延迟时间处理，与读取字节数多少相关，初略算法
    int delay = 30 + ((bytes % 20 == 0) ? bytes / 20 : (bytes / 20) + 1) * 40;
    string result = WriteAndRead(command,"Ascii",delay);
    if(!result.empty() && result.data()[0] == ACK[0])
        return true;
    else
        return false;
}


/**
 * @brief FxPlc::ReadBitBy422       批量读取位元件（M寄存器）
 * @param address
 * @param length
 * @return
 */
string FxPlc::ReadBitBy422(int address, int length)
{
    string result;
    int tmpLength = length;
    do
    {
        int readLength = tmpLength > 255 ? 255 : tmpLength;
        int bytes = (readLength + address % 8) % 8 == 0 ? \
                    (readLength + address % 8) / 8 : (readLength + address % 8) / 8 + 1;
        char byte[10] = {0};
        char base[10] = {0};
        sprintf(byte,"%02X",bytes);
        sprintf(base,"%04X",(address / 8 + 256));
        string baseAddress = base;
        string command = "0" + baseAddress + byte + ETX;
        string checkSum = GetSumCheckByAsciiStr(command);
        command = STX + command + checkSum;
        //延迟时间处理，与读取字节数多少相关，初略算法
        int delay = 30 + bytes * 40;
        timeval start;
        gettimeofday(&start,NULL);
        string buff = WriteAndRead(command,"Ascii",delay);
        timeval current;
        gettimeofday(&current,NULL);
        if(buff.length() > 0 && buff.data()[0] == STX[0])
        {
            if((buff.length() == 4 + bytes * 2) && (buff.data()[1 + bytes * 2] == ETX[0]))
            {
                string values;
                string tmp;
                for(int i = 0;i < bytes;i++)
                {
                    string hex = buff.substr(1 + i*2,2);
                    string bit = padZero(intToBinaryStr(hextodec(hex)),8);
                    tmp = InsersionString(bit);
                    values.append(InsersionString(bit));
                }
                result.append(values.substr(address % 8,readLength));
            }
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
 * @brief FxPlc::WriteBitBy422      置位/复位位元件（M寄存器）
 * @note                            只能单点写，每次只写一个寄存器
 * @param address
 * @param value
 * @return
 */
bool FxPlc::WriteBitBy422(int address, string value)
{

    char high[10] = {0};
    char low[10] = {0};
    sprintf(high,"%02X",address % 256);
    sprintf(low,"%02X",address / 256 + 64);
    string highAddress = high;
    string lowAddress = low;
    string command = (!value.compare("1") ? _programPortForceONCmd : _programPortForceOFFCmd) + highAddress + lowAddress + ETX;
    string checkSum = GetSumCheckByAsciiStr(command);
    command = STX + command + checkSum;
    string result = WriteAndRead(command, "Ascii",_delay);
    _log.LOG_DEBUG("FxPlc WriteBitBy422 读写命令 command = 【%s】 读写返回结果位 【%s】",command.data(),result.data());
    if(!result.empty() && result.data()[0] == ACK[0])
        return true;
    else
        return false;
}


/**
 * @brief FxPlc::ReadBitM
 * @param address
 * @param length
 * @return
 */
string FxPlc::ReadBitM(int address, int length)
{
    if(_protocolMode == dedicatedProtocol)
        return ReadBitBy232(address,length);
    else if(_protocolMode == programProtocol)
        return  ReadBitBy422(address,length);
}


/**
 * @brief FxPlc::WriteBitM
 * @param address
 * @param value
 * @return
 */
bool FxPlc::WriteBitM(int address, string value)
{
    if(_protocolMode == dedicatedProtocol)
        return WriteBitBy232(address, value);
    else if(_protocolMode == programProtocol)
        return  WriteBitBy422(address, value);
}


/**
 * @brief FxPlc::ReadWord
 * @param address
 * @param length
 * @return
 */
vector<int> FxPlc::ReadWord(int address, int length)
{
    vector<int> res;
    if(m_communicateStatus)
    {
        pthread_rwlock_wrlock(&m_lock);
        if(_protocolMode == dedicatedProtocol)
            res = ReadWordBy232(address, length, FOUR);
        else if(_protocolMode == programProtocol)
            res = ReadWordBy422(address, length, FOUR);
        pthread_rwlock_unlock(&m_lock);
    }
    return res;
}

/**
 * @brief FxPlc::WriteWord
 * @param address
 * @param value
 * @return
 */
bool FxPlc::WriteWord(int address, vector<int> value)
{
    bool res = false;
    if(m_communicateStatus)
    {
        pthread_rwlock_wrlock(&m_lock);
        if(_protocolMode == dedicatedProtocol)
            res = WriteWordBy232(address, value);
        else if(_protocolMode == programProtocol)
            res = WriteWordBy422(address, value, FOUR);
        pthread_rwlock_unlock(&m_lock);
    }
    return res;
}

/**
 * @brief 批量读位元件
 * @param 起始地址
 * @param 读取长度
 * @param 寄存器类型
 * @return 字符串类型的值
 */
string FxPlc::ReadBits(int address, int length, string registerType)
{
    string res = "";
    if(m_communicateStatus)
    {
        pthread_rwlock_wrlock(&m_lock);
        if(registerType == "M")
            res = ReadBitM(address, length);
        pthread_rwlock_unlock(&m_lock);
    }
    return res;
}

/**
 * @brief 批量写位元件
 * @param 起始地址
 * @param 写入值
 * @param 寄存器类型
 * @return
 */
bool FxPlc::WriteBits(int address, string value, string registerType)
{
    bool res = false;
    if(m_communicateStatus)
    {
        pthread_rwlock_wrlock(&m_lock);
        if(registerType == "M")
            res = WriteBitM(address, value);
        pthread_rwlock_unlock(&m_lock);
    }
    return res;
}

/**
 * @brief 通信检测
 * @return
 */
bool FxPlc::CanAcess()
{
    pthread_rwlock_wrlock(&m_lock);
    string result = ReadBitM(119,1);
    if(!result.compare("3"))
        m_communicateStatus = false;
    else
        m_communicateStatus = true;
    pthread_rwlock_unlock(&m_lock);
    return m_communicateStatus;
}

























#if 0
/**
 * @brief 使用计算机专有协议读辅助继电器M地址，适用RS232，485通信
 * @param address   继电器地址
 * @return
 */
string FxPlc::ReadRelayBy232(int address)
{
     char m_address[10];
     sprintf(m_address,"%d",address);
     string baseAddress = padZero((string)m_address,4);
     string command = _stationNum + _pc + _readM + baseAddress + _onePoint;
     string checkSum = GetSumCheckByAsciiStr(command);
     string cmd = "";
     cmd.append(ENQ).append(command).append(checkSum);
     string result = ComDevice::WriteAndRead(cmd,"Ascii", _delay);
     if (result.length() > 0 && result.data()[0] == STX[0])
     {
         //初步检验为合格响应,PLC返回13个字符+cr lf
         if (result.length() == 9)
         {
             if (result.data()[5] == '1')
             {
                 return "1";
             }
             else
             {
                 return "0";
             }
         }
     }
     return "3";
}

/**
 * @brief 使用计算机转悠协议批量读取连续M点地址，适用RS232、RS485通信
 * @param address   起始点地址
 * @param length    读取长度
 * @return
 */
string FxPlc::ReadRelayBy232(int address, int length)
{
    char value[length];
    for (int i = 0; i < length; i++)
    {
        value[i] = '3';
    }
    char m_address[10] = {0};
    char m_length[10] = {0};

    sprintf(m_address,"%d",address);
    string baseAddress = padZero(m_address,4);

    sprintf(m_length,"%X",length);
    string len = padZero(m_length,2);

    string command = _stationNum + _pc + _readM + baseAddress + len;
    string checkSum = GetSumCheckByAsciiStr(command);
    command = ENQ + command + checkSum;        //checkSum
    string result = ComDevice::WriteAndRead(command, "Ascii", _delay);
    if (!result.empty() && result.data()[0] == STX[0])
    {
         if (result.length() == 8 + (unsigned int)length)
            return result.substr(5,length);
    }
    return value;
}

/**
 * @brief 读单个辅助继电器M地址
 * @param address   继电器地址
 * @return
 */
string FxPlc::ReadRelayM(int address)
{
    return ReadRelayBy232(address);
}

/**
 * @brief 批量读M点地址状态值
 * @param address   起始地址
 * @param length    读取长度
 * @return          状态字符串
 */
string FxPlc::ReadRelayM(int address, int length)
{
    return ReadRelayBy232(address, length);
}

/**
 * @brief 写单个辅助继电器地址
 * @param address   继电器地址
 * @param state     状态值
 */
void FxPlc::WriteRelayM(int address, string state)
{
    WriteRelayBy232(address, state);
}

/**
 * @brief 批量写入M点地址状态
 * @param address   起始地址
 * @param state     待写入状态值字符串为长度为length的字符串，例如：length = 5时，state = "00000"或"11111"
 * @param length    写入点数
 */
void FxPlc::WriteRelayM(int address, string state, int length)
{
    WriteRelayBy232(address, state, length);
}

/**
 * @brief 使用计算机专有协议读辅助继电器M地址，适用于422协议
 * @param address   继电器地址
 * @return
 */
int FxPlc::ReadRelayBy422(int address)
{
    int sectionAddress = address / 8;       //段地址 0起始 从小到大(0~127)
    int sectionOffset = address % 8;        //段内偏移 0起始 从小到大(0~7)
    char m_sectionAddress[10];
    sprintf(m_sectionAddress,"%X",(256 + sectionAddress));
    string baseAddress = "0" + (string)m_sectionAddress;
    string command = _programPortReadCmd + baseAddress + _onePoint + ETX;
    string checkSum = GetSumCheckByAsciiStr(command);
    command = STX + command + (string)checkSum;
    string result = ComDevice::WriteAndRead((char *)command.data(), "Ascii", _delay);

    if (result.length() > 0 && result.data()[0] == STX[0])
    {
        if (result.length() == 6)
        {
            char h = (char)result.data()[1];
            char l = (char)result.data()[2];
            int hh = hextodec(&h);
            int ll = hextodec(&l);
            char _hh[10] = {0};
            char _ll[10] = {0};
            sprintf(_hh,"%d",hh);
            sprintf(_ll,"%d",ll);
            string s = ::padZero(_hh,4) + ::padZero(_ll, 4);

            //位是从大到小的7~0
            if (s.data()[7 - sectionOffset] == '1')
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
    return 3;
}

/**
 * @brief 使用计算机专有协议写辅助继电器M地址，适用RS232，485通信
 * @param address   继电器地址
 * @param state     状态值
 */
void FxPlc::WriteRelayBy232(int address, string state)
{
    char m_address[10];
    sprintf(m_address,"%d",address);
    string baseAddress = m_address;
    baseAddress = padZero(baseAddress,4);
    string command = _stationNum + _pc + _writeM + baseAddress + _onePoint + state;
    string checkSum = GetSumCheckByAsciiStr(command);
    command = ENQ + command + checkSum;
    ComDevice::WriteAndRead((char *)command.data(), "Ascii", _delay);
}

/**
 * @brief 计算机专有协议批量写M点地址，适用RS232，RS485通信
 * @param address   起始点地址
 * @param state     待写入状态值为长度为length的字符串，例如：length = 5时，state = "00000"或"11111"
 * @param length    写入点数
 */
void FxPlc::WriteRelayBy232(int address, string state, int length)
{
    char m_address[10] = {0};
    sprintf(m_address,"%d",address);
    string baseAddress = padZero(m_address,4);

    char m_length[10] = {0};
    sprintf(m_length,"%X",length);
    string len = padZero(m_length,2);
    string command = _stationNum + _pc + _writeM + baseAddress + len + state;
    string checkSum = GetSumCheckByAsciiStr(command);
    command = ENQ + command + checkSum;
    string result = ComDevice::WriteAndRead(command, "Ascii", _delay);
}

/**
 * @brief 三菱编程口协议，写单个辅助继电器地址，适用于422协议
 * @param address   继电器地址
 * @param state     状态值
 */
void FxPlc::WriteRelayBy422(int address, string state)
{
    char m_address[10];
    sprintf(m_address,"%X",(address+2048));
    string baseAddress = "0" + (string)m_address;
    //低位在前调整
    baseAddress = baseAddress.substr(2,2) + baseAddress.substr(0,2);
    string command;
    if(!state.compare("0"))
    {
        command = _programPortWriteOFFCmd;
    }
    else
    {
        command = _programPortWriteONCmd + baseAddress + ETX;
    }
    string checkSum = GetSumCheckByAsciiStr(command);
    command = STX + command + checkSum;
    ComDevice::WriteAndRead((char *)command.data(), "Ascii", _delay);
}




#endif

