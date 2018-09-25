#include "LaserMarker.h"

string _getVersion = "getVersion";

/**
 * @brief LaserMarker::LaserMarker                      通过端口号以及ip地址构造设备
 * @param port                                          端口号
 * @param ip                                            ip地址
 */
LaserMarker::LaserMarker(int port ,string ip):TcpDevice(port,ip)
{

}

/**
 * @brief LaserMarker::LaserMarker                      通过端口号、ip地址以及设备名称构造
 * @param port
 * @param ip
 * @param name
 */
LaserMarker::LaserMarker(int port, string ip, string name):TcpDevice(port,ip,name)
{

}

/**
 * @brief LaserMarker::test                             测试函数
 */
void LaserMarker::test()
{
    while(1)
    {
        string str = Read("Ascii");
    }

}

/**
 * @brief LaserMarker::CanAcess                         检测通信状态
 * @return
 */
bool LaserMarker::CanAcess()
{
    string result = TcpDevice::WriteAndRead(_getVersion,"Ascii",200);
    if(!result.empty() && result.length() > 0)
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}

/**
 * @brief LaserMarker::DoPrint                          打标函数
 * @param msg                                           打标内容
 * @return
 */
string LaserMarker::DoPrint(string msg)
{
//    Write(msg,"Ascii");
    string result = WriteAndRead(msg, "Ascii", 200);
    _log.LOG_DEBUG("【%s】 LaserMarker DoPrint result = 【%s】",Name.data(),result.data());
    if(!result.empty())
    {
        result= result.substr(result.length() - 8,8);
    }
    return result;
}

