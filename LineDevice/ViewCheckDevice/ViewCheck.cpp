#include "ViewCheck.h"


/**
 * @brief ViewCheck::ViewCheck              初始化构造函数
 * @param port                              端口号
 * @param ip                                IP地址
 */
ViewCheck::ViewCheck(int port, string ip) : TcpDevice(port, ip)
{
    startCheck = "checkStart";
    heartCheck = "heartbeat";
}


/**
 * @brief ViewCheck::ViewCheck              初始化构造函数
 * @param port                              端口号
 * @param ip                                IP地址
 * @param name                              设备名
 */
ViewCheck::ViewCheck(int port, string ip, string name) : TcpDevice(port, ip, name)
{
    startCheck = "checkStart";
    heartCheck = "heartbeat";
}

/**
 * @brief ViewCheck::CanAcess                   通信检测
 * @return                                      通信成功返回true；否则返回false
 */
bool ViewCheck::CanAcess()
{
    string result = WriteAndRead(heartCheck + endStr, "Ascii",200);     //heartbeat\r\n
    if(!result.empty() && result.length() > 0)
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}


/**
 * @brief ViewCheck::GetCheckResult             取视觉检测结果
 * @param id                                    ID号
 * @return                                      读取成功返回“1”；否则返回“0”
 */
string ViewCheck::GetCheckResult(string id)
{
    Write(startCheck + "/" + id + endStr ,"Ascii");
    sleep(2);
    string result = Read("Ascii");
    cout<<"--- ViewCheck result = "<<result<<endl;
    if(result == "NG\r\n")
        return "0";
    else if(result == "OK\r\n")
        return "1";
    else
        return "0";
}

