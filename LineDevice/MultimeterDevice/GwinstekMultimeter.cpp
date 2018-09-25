#include "GwinstekMultimeter.h"


/**
 * @brief GwinstekMultimeter::GwinstekMultimeter        //构造函数
 * @param spp                                           //SerialPortParameters结构体类型
 */
GwinstekMultimeter::GwinstekMultimeter(SerialPortParameters *spp) : ComDevice(spp)
{
    _readCmd = "va11?";
    _setBatchCount = "samp:coun ";
    _setDisplayClose = "SYST:DISP OFF";
    _setDisplayOpen = "SYST:DISP ON";
    _cls = "*CLS";
    _idn = "*idn?";
    _delay = 200;
}


/**
 * @brief GwinstekMultimeter::GwinstekMultimeter        //构造函数
 * @param spp                                           //SerialPortParameters结构体类型
 * @param name                                          //设备名称
 */
GwinstekMultimeter::GwinstekMultimeter(SerialPortParameters *spp, string name) : ComDevice(spp,name)
{
    _readCmd = "va11?";
    _setBatchCount = "samp:coun ";
    _setDisplayClose = "SYST:DISP OFF";
    _setDisplayOpen = "SYST:DISP ON";
    _cls = "*CLS";
    _idn = "*idn?";
    _delay = 200;
}


/**
 * @brief GwinstekMultimeter::CanAcess          通信测试
 * @return                                      成功返回true；否则返回false
 */
bool GwinstekMultimeter::CanAcess()
{
    string result = GetIdn();
//    cout << "result = "<<result<<endl;
    if(!result.empty() && result.length() > 0)
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}


/**
 * @brief GwinstekMultimeter::SetDisplay            设置显示模式
 * @param display                                   显示模式
 */
void GwinstekMultimeter::SetDisplay(bool display)
{
    string command = display ? _setDisplayOpen : _setDisplayClose + ComDevice::endStr;
    Write(command,"Ascii");
}


/**
 * @brief GwinstekMultimeter::GetIdn                获取IDN
 * @return                                          返回收到的数据
 */
string GwinstekMultimeter::GetIdn()
{
    return WriteAndRead(_idn + ComDevice::endStr,"Ascii",_delay);
}


/**
 * @brief GwinstekMultimeter::ClearError            清除万用表错误信息
 */
void GwinstekMultimeter::ClearError()
{
    Write(_cls + endStr,"Ascii");
}


/**
 * @brief GwinstekMultimeter::GetValue          读万用表的值
 * @return                                      成功返回万用表的值；否则返回空字符串
 */
string GwinstekMultimeter::GetValue()
{
    string result = WriteAndRead(_readCmd + endStr,"Ascii",_delay);
    char** str;
    if (!result.empty() && result.length() > 0)
    {
        //将字符串中的换行符，空格去掉，转换成double类型
        double num = strtod(Trim(result).data(),str);
        return DigitChange(num,10);
    }
    return "";
}


/**
 * @brief GwinstekMultimeter::GetValues        批量读取万用表的值
 * @param count                                数量
 * @param wait                                 延迟时间
 * @return                                     返回读到万用表数据的数组
 */
vector<string> GwinstekMultimeter::GetValues(int count, int wait)
{
    vector<string> results;
    vector<string> values;
    char** str;
    string command = _setBatchCount + IntToString(count) + endStr + _readCmd + endStr;
    string result = WriteAndRead(command,"Ascii",wait);
    if(!result.empty() && result.length() > 9)
    {
        string tmp = TrimTo(result);        //把“/r/n”换成“，”
        splittostring(tmp,",",results);
        for(int i = 0 ; i < results.size();i++)
        {
            if(results.empty())
                continue;
            double num = strtod(results.at(i).data(),str);
            values.push_back(DigitChange(num,3));
        }
    }
    return values;
}


/**
 * @brief GwinstekMultimeter::SendCommand           发指令到万用表
 * @param command                                   参数命令
 */
bool GwinstekMultimeter::SendCommand(string command)
{
    if(!command.empty())
    {
        return Write(command + endStr,"Ascii");
    }
}


/**
 * @brief GwinstekMultimeter::Read                  读万用表的值
 * @return                                          返回读到万用表的值
 */
string GwinstekMultimeter::Read()
{
    return ComDevice::Read("Ascii");
}



