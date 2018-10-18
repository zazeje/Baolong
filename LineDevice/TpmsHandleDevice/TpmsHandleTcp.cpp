#include "TpmsHandleTcp.h"


TpmsHandleTcp::TpmsHandleTcp(int port, string ip) : TcpDevice(port, ip),Enter_KEY("FC8B0000008B"),Light_KEY("FC8100000081"),\
    Power_KEY("FC8200000082"),Down_KEY("FC8500000085"),Up_KEY("FC8700000087"),\
     Esc_KEY("FC8900000089"),TESTCMD("ver?")
{
    idLen = 8;
    DELAY = 300;
    m_communicateStatus = true;
}

TpmsHandleTcp::TpmsHandleTcp(int port, string ip, string name) : TcpDevice(port, ip, name),Enter_KEY("FC8B0000008B"),Light_KEY("FC8100000081"),\
    Power_KEY("FC8200000082"),Down_KEY("FC8500000085"),Up_KEY("FC8700000087"),\
     Esc_KEY("FC8900000089"),TESTCMD("ver?")
{
    idLen = 8;
    DELAY = 300;
    m_communicateStatus = true;
}

TpmsHandleTcp::~TpmsHandleTcp()
{}

/**
 * @brief TpmsHandleTcp::CanAcess              通信测试
 * @return                                  成功返回true，失败返回false
 */
bool TpmsHandleTcp::CanAcess()
{
    string str = WriteAsciiAndReadHex(TESTCMD,200);
    if (str.empty())
        m_communicateStatus = false;
    else
        m_communicateStatus = true;
    return m_communicateStatus;
}

/**
 * @brief TpmsHandleTcp::SendId                发送ID
 * @param id
 */
string TpmsHandleTcp::SendId(string id)
{
    string res = "";
    if(!id.empty() && StringToUpper(id).compare("NULL"))
    {
        id = padZero(id,idLen);
        string command = "FE" + id + GetXorCheckByHexStr(id);
        res = WriteAndRead(command,"Hex",50);
    }
    cout<<"--- res = "<<res<<endl;
    return res;
}

/**
 * @brief TpmsHandleTcp::GetData               读取接收机接收的数据
 * @return                                     接收到的数据
 */
string TpmsHandleTcp::GetData()
{
    string hexStr = Read("Hex");
    return hexStr;
}

/**
 * @brief TpmsHandleTcp::SendEsc               发送Esc
 * @return
 */
string TpmsHandleTcp::SendEsc()
{
    return WriteAndRead(Esc_KEY, "Hex", DELAY);
}

/**
 * @brief TpmsHandleTcp::SendEnter             发送Enter
 * @return
 */
string TpmsHandleTcp::SendEnter()
{
    return WriteAndRead(Enter_KEY, "Hex", DELAY);
}

/**
 * @brief TpmsHandleTcp::SendUp                发送Up
 * @return
 */
string TpmsHandleTcp::SendUp()
{
    return WriteAndRead(Up_KEY, "Hex", DELAY);
}

/**
 * @brief TpmsHandleTcp::SendDown              发送Down
 * @return
 */
string TpmsHandleTcp::SendDown()
{
    return WriteAndRead(Down_KEY, "Hex", DELAY);
}

/**
 * @brief TpmsHandleTcp::SendPower             发送Power
 * @return
 */
string TpmsHandleTcp::SendPower()
{
    return WriteAndRead(Power_KEY, "Hex", DELAY);
}

/**
 * @brief TpmsHandleTcp::SendLight             发送Light
 * @return
 */
string TpmsHandleTcp::SendLight()
{
    return WriteAndRead(Light_KEY, "Hex", DELAY);
}
