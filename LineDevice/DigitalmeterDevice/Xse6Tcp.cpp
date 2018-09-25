#include "Xse6Tcp.h"


Xse6Tcp::Xse6Tcp(int port, string ip):TcpDevice(port, ip),VSION("99"),MAX_VALUE("01"),MIN_VALUE("02"),MIN_RANGE("12"),\
    MAX_RANGE("13"),BAUD("25"),DATA_LEN(7),addr("01")
{
    DELAY = 200;
    endStr = "\r\n";
}

Xse6Tcp::Xse6Tcp(int port, string ip, string name):TcpDevice(port, ip, name),VSION("99"),MAX_VALUE("01"),MIN_VALUE("02"),MIN_RANGE("12"),\
    MAX_RANGE("13"),BAUD("25"),DATA_LEN(7),addr("01")
{
    DELAY = 200;
    endStr = "\r\n";
}

Xse6Tcp::~Xse6Tcp()
{

}

/**
 * @brief 通信测试
 * @return
 */
bool Xse6Tcp::CanAcess()
{
    string vsion = ReadValue(VSION);
//    cout << "vsion = " << vsion << endl;
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
double Xse6Tcp::GetValue()
{
    return ValueHandler(ReadValue(""));
}

/**
 * @brief 读最大值
 * @return
 */
double Xse6Tcp::GetMaxValue()
{
    return ValueHandler(ReadValue(MAX_VALUE));
}

/**
 * @brief 读最小值
 * @return
 */
double Xse6Tcp::GetMinValue()
{
    return ValueHandler(ReadValue(MIN_VALUE));
}

/**
 * @brief 读仪表当前波特率
 * @return
 */
int Xse6Tcp::GetBaud()
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
double Xse6Tcp::GetRangeMax()
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
double Xse6Tcp::GetRangeMin()
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
string Xse6Tcp::ReadValue(string code)
{
    string command = "#" + addr + code + endStr;
    string result = WriteAndRead(command, "Ascii", DELAY);
    return result;
}

/**
 * @brief 解析测量结果
 * @param value
 * @return
 */
double Xse6Tcp::ValueHandler(string value)
{
//    cout<<"Xse6Tcp value = "<<value<<" -- length = "<<value.length()<<endl;
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
string Xse6Tcp::ReadParam(string code)
{
    string command = "$" + addr + code + endStr;
    return WriteAndRead(command, "Ascii", DELAY);
}
