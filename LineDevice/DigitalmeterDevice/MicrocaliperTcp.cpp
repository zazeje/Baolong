#include "MicrocaliperTcp.h"


/**
 * @brief MicrocaliperTcp::MicrocaliperTcp            测微计设备驱动
 * @param spp
 */
MicrocaliperTcp::MicrocaliperTcp(int port, string ip) : TcpDevice(port,ip)
{
    _getValue = "010300000002C40B";
    _clearValue = "01060800AB5674A4";
    ClearValue();
}

/**
 * @brief MicrocaliperTcp::MicrocaliperTcp            测微计设备驱动
 * @param spp
 * @param name
 */
MicrocaliperTcp::MicrocaliperTcp(int port , string ip, string name) : TcpDevice(port,ip,name)
{
    _getValue = "010300000002C40B";
    _clearValue = "01060800AB5674A4";
    ClearValue();
}

/**
 * @brief MicrocaliperTcp::~MicrocaliperTcp
 */
MicrocaliperTcp::~MicrocaliperTcp()
{

}


/**
 * @brief MicrocaliperTcp::GetValue               读测微计的值
 * @return
 */
double MicrocaliperTcp::GetValue()
{
    double value=0.0;
    string result = WriteAndRead(_getValue, "Hex", 70);
    _log.LOG_DEBUG("MicrocaliperTcp  GetValue  result=【%s】",result.data());
    if(result.empty())
        return 0.0;
    if(result.length() == 18 && !result.substr(0,6).compare("010304"))
    {
        result = result.substr(6,8);
        if(result.length() == 8)
        {
            int sign;
            if(!result.substr(0,2).compare("01"))
                sign = -1;
            else if(!result.substr(0,2).compare("00"))
                sign = 1;            
            value = hextodec(result.substr(4,4)) * sign / 1000.0;
        }
    }
    return value;
}

/**
 * @brief MicrocaliperTcp::ClearValue              清除测微计的值
 */
void MicrocaliperTcp::ClearValue()
{
    WriteAndRead(_clearValue, "Hex", 70);
}

/**
 * @brief MicrocaliperTcp::CanAcess                通信检测
 * @return
 */
bool MicrocaliperTcp::CanAcess()
{
    string result = WriteAndRead(_getValue, "Hex", 70);
    if(!result.empty())
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}



