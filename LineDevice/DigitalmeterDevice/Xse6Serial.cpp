#include "Xse6Serial.h"


Xse6Serial::Xse6Serial() : VSION("99"),MAX_VALUE("01"),MIN_VALUE("02"),MIN_RANGE("12"),\
    MAX_RANGE("13"),BAUD("25"),DATA_LEN(7),addr("01")
{
    DELAY = 200;
    endStr = "\r\n";
}

Xse6Serial::Xse6Serial(SerialPortParameters* spp) : ComDevice(spp),\
    VSION("99"),MAX_VALUE("01"),MIN_VALUE("02"),MIN_RANGE("12"),\
    MAX_RANGE("13"),BAUD("25"),DATA_LEN(7),addr("01")
{
    DELAY = 200;
    endStr = "\r\n";
}

Xse6Serial::Xse6Serial(SerialPortParameters* spp,string devicename) : ComDevice(spp,devicename),\
    VSION("99"),MAX_VALUE("01"),MIN_VALUE("02"),MIN_RANGE("12"),\
    MAX_RANGE("13"),BAUD("25"),DATA_LEN(7),addr("01")
{
    DELAY = 200;
    endStr = "\r\n";
}

Xse6Serial::~Xse6Serial()
{}

/**
 * @brief 通信测试
 * @return
 */
bool Xse6Serial::CanAcess()
{
    string vsion = ReadValue(VSION);
//    cout<<"-- vsion = "<<vsion<<" --- length = "<<vsion<<endl;
    if(vsion.length() == 13)
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}

/**
 * @brief 读测量值
 * @return
 */
double Xse6Serial::GetValue()
{
    return ValueHandler(ReadValue(""));
}

/**
 * @brief 读最大值
 * @return
 */
double Xse6Serial::GetMaxValue()
{
    return ValueHandler(ReadValue(MAX_VALUE));
}

/**
 * @brief 读最小值
 * @return
 */
double Xse6Serial::GetMinValue()
{
    return ValueHandler(ReadValue(MIN_VALUE));
}

/**
 * @brief 读仪表当前波特率
 * @return
 */
int Xse6Serial::GetBaud()
{
    string result = ReadParam(BAUD);
    if (!result.empty() && result.length() > DATA_LEN)
    {
        int i = atoi(result.substr(1, DATA_LEN - 1).data());
        switch (i)
        {
            case 0:
                return 1200;
            case 1:
                return 2400;
            case 2:
                return 4800;
            case 3:
                return 9600;
            case 4:
                return 115200;
        }
    }
    return 0;
}

/**
 * @brief 读仪表当前量程上限
 * @return
 */
double Xse6Serial::GetRangeMax()
{
    string result = ReadParam(MAX_RANGE);
    if (!result.empty() && result.length() > DATA_LEN + 1)
    {
        return atof(result.substr(1, DATA_LEN).data());
    }
    double num = 100000;
    return num;
}

/**
 * @brief 读仪表当前量程下限
 * @return
 */
double Xse6Serial::GetRangeMin()
{
    string result = ReadParam(MIN_RANGE);
    if (!result.empty() && result.length() > DATA_LEN + 1)
    {
        return atof(result.substr(1, DATA_LEN).data());
    }
    double num = -20000;
    return num;
}

/**
 * @brief 读仪表测量值
 * @param code 为 ""时读测量值，"01"读最大值，"02"读最小值,"0001":读输出模拟量值(变送器输出)
 * "0002":读开关量输入状态, "0003":读开关量输出状态(报警输出), "99":读仪表版本号
 * @return
 */
string Xse6Serial::ReadValue(string code)
{
    string command = "#" + addr + code + endStr;
    string result = WriteAndRead(command, "Ascii", DELAY);
    cout<<"Xse6Serial ReadValue --- result = "<<result<<endl;
    return result;
}

/**
 * @brief 解析测量结果
 * @param value
 * @return
 */
double Xse6Serial::ValueHandler(string value)
{
    while(value.find("[" + endStr + "]") != string::npos)
    {
        value.replace(value.find("[" + endStr + "]"),1,"");
    }
    if(!value.empty() && value.length() > DATA_LEN + 1)
    {
        return atof(value.substr(1, DATA_LEN).data());
    }
    return 0.00;
}

/**
 * @brief 读仪表参数
 * @param code
 * @return
 */
string Xse6Serial::ReadParam(string code)
{
    string command = "$" + addr + code + endStr;
    return WriteAndRead(command, "Ascii", DELAY);
}
