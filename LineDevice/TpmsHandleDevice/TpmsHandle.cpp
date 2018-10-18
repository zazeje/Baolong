#include "TpmsHandle.h"
#if 1
TpmsHandle::TpmsHandle():Enter_KEY("FC8B0000008B"),Light_KEY("FC8100000081"),\
    Power_KEY("FC8200000082"),Down_KEY("FC8500000085"),Up_KEY("FC8700000087"),\
     Esc_KEY("FC8900000089"),TESTCMD("ver?")
{
    idLen = 8;
    DELAY = 300;
    m_communicateStatus = true;
}

TpmsHandle::TpmsHandle(SerialPortParameters* spp) : ComDevice(spp),Enter_KEY("FC8B0000008B"),Light_KEY("FC8100000081"),\
    Power_KEY("FC8200000082"),Down_KEY("FC8500000085"),Up_KEY("FC8700000087"),\
     Esc_KEY("FC8900000089"),TESTCMD("ver?")
{
    idLen = 8;
    DELAY = 300;
    m_communicateStatus = true;
}

TpmsHandle::TpmsHandle(SerialPortParameters* spp,string devicename) : ComDevice(spp,devicename),Enter_KEY("FC8B0000008B"),Light_KEY("FC8100000081"),\
    Power_KEY("FC8200000082"),Down_KEY("FC8500000085"),Up_KEY("FC8700000087"),\
     Esc_KEY("FC8900000089"),TESTCMD("ver?")
{
    idLen = 8;
    DELAY = 300;
    m_communicateStatus = true;
    deviceName = devicename;
}

TpmsHandle::~TpmsHandle()
{}

/**
 * @brief TpmsHandle::CanAcess              通信测试
 * @return                                  成功返回true，失败返回false
 */
bool TpmsHandle::CanAcess()
{
    string str = WriteAsciiAndReadHex(TESTCMD,200);
    if (str.empty())
        m_communicateStatus = false;
    else
        m_communicateStatus = true;
    return m_communicateStatus;
}

/**
 * @brief TpmsHandle::SendId                发送ID
 * @param id
 */
string TpmsHandle::SendId(string id)
{
    string res = "";
    if(!id.empty() && StringToUpper(id).compare("NULL"))
    {
        id = padZero(id,idLen);
        string command = "FE" + id + GetXorCheckByHexStr(id);
        res = WriteAndRead(command,"Hex",50);
    }
    return res;
}

/**
 * @brief TpmsHandle::GetData               读取接收机接收的数据
 * @return                                  接收到的数据
 */
string TpmsHandle::GetData(string devName)
{
    string hexStr = Read("Hex");
    return hexStr;
}

/**
 * @brief TpmsHandle::SendEsc               发送Esc
 * @return
 */
string TpmsHandle::SendEsc()
{
    return WriteAndRead(Esc_KEY, "Hex", DELAY);
}

/**
 * @brief TpmsHandle::SendEnter             发送Enter
 * @return
 */
string TpmsHandle::SendEnter()
{
    return WriteAndRead(Enter_KEY, "Hex", DELAY);
}

/**
 * @brief TpmsHandle::SendUp                发送Up
 * @return
 */
string TpmsHandle::SendUp()
{
    return WriteAndRead(Up_KEY, "Hex", DELAY);
}

/**
 * @brief TpmsHandle::SendDown              发送Down
 * @return
 */
string TpmsHandle::SendDown()
{
    return WriteAndRead(Down_KEY, "Hex", DELAY);
}

/**
 * @brief TpmsHandle::SendPower             发送Power
 * @return
 */
string TpmsHandle::SendPower()
{
    return WriteAndRead(Power_KEY, "Hex", DELAY);
}

/**
 * @brief TpmsHandle::SendLight             发送Light
 * @return
 */
string TpmsHandle::SendLight()
{
    return WriteAndRead(Light_KEY, "Hex", DELAY);
}
#endif
