#include "SwpC80.h"


/**
 * @brief SwpC80::SwpC80
 * @param spp
 */
SwpC80::SwpC80(SerialPortParameters *spp) : ComDevice(spp)
{
    _cmdPress = "@01RD17";                  //环境气压命令
}


/**
 * @brief SwpC80::SwpC80                    构造函数初始化
 * @param spp                               SerialPortParameters结构体类型
 * @param name
 */
SwpC80::SwpC80(SerialPortParameters *spp, string name) : ComDevice(spp,name)
{
    _cmdPress = "@01RD17";                  //环境气压命令
}

/**
 * @brief SwpC80::~SwpC80                   析构函数
 */
SwpC80::~SwpC80()
{}

/**
 * @brief SwpC80::CanAccess                 通信测试
 * @return                                  通信成功返回true；否则返回false
 */
bool SwpC80::CanAcess()
{
    double value = GetValue();
    if(value!=0.0)
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}


/**
 * @brief SwpC80::GetValue              获取气压值
 * @return                              成功返回气压值；否则返回0.0
 */
double SwpC80::GetValue()
{
    string result = WriteAndRead(_cmdPress,"Ascii",150);
    if(!result.empty() && result.length() > 13)
    {
        int tmp = atoi((result.substr(11,2) + result.substr(9,2)).data());
        return (double)tmp/10;
    }
    return 0.0;
}
