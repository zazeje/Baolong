#include "Microcaliper.h"


/**
 * @brief Microcaliper::Microcaliper            测微计设备驱动
 * @param spp
 */
Microcaliper::Microcaliper(SerialPortParameters *spp) : ComDevice(spp)
{
    _getValue = "010300000002C40B";
    _clearValue = "01060800AB5674A4";
//    ClearValue();
}

/**
 * @brief Microcaliper::Microcaliper            测微计设备驱动
 * @param spp
 * @param name
 */
Microcaliper::Microcaliper(SerialPortParameters *spp, string name) : ComDevice(spp,name)
{
    _getValue = "010300000002C40B";
    _clearValue = "01060800AB5674A4";
//    ClearValue();
}

/**
 * @brief Microcaliper::~Microcaliper
 */
Microcaliper::~Microcaliper()
{

}


/**
 * @brief Microcaliper::GetValue               读测微计的值
 * @return
 */
double Microcaliper::GetValue()
{
    double value=0.0;
    string result = WriteAndRead(_getValue, "Hex", 70);
//    _log.LOG_DEBUG("Microcaliper GetValue  result = %s",result.data());
    if(result.empty())
        return 0.0;
    if(result.length() == 18 && !result.substr(0,6).compare("010304"))
    {
        result = result.substr(6,8);
        if(result.length() == 8)
        {
            int sign = 0;
            if(!result.substr(0,2).compare("01"))
                sign = -1;
            else if(!result.substr(0,2).compare("00"))
                sign = 1;
            value = hextodec(result.substr(4,4)) * sign / 1000;
        }
    }
    return value;
}

/**
 * @brief Microcaliper::ClearValue              清除测微计的值
 */
void Microcaliper::ClearValue()
{
    WriteAndRead(_clearValue, "Hex", 70);
}

/**
 * @brief Microcaliper::CanAcess                通信检测
 * @return
 */
bool Microcaliper::CanAcess()
{
    string result = WriteAndRead(_getValue, "Hex", 70);
    if(!result.empty())
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}



