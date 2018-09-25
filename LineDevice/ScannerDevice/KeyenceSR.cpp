#include "KeyenceSR.h"
#include "./DataConvert/DataType.h"


/**
 * @brief 通过端口号、ip地址执行构造
 * @param port  端口号
 * @param ip    IP地址
 */
KeyenceSR::KeyenceSR(int port, string ip):TcpDevice(port, ip),testCmd("KEYENCE"),startCmd("LON\r\n"),stopCmd("LOFF\r\n"),endStr("\r\n")
{
    _stop = false;
}

/**
 * @brief 通过端口号、ip地址、设备名称执行构造
 * @param port  端口号
 * @param ip    IP地址
 * @param name  设备名称
 */
KeyenceSR::KeyenceSR(int port, string ip, string name):TcpDevice(port,ip,name),testCmd("KEYENCE"),startCmd("LON\r\n"),stopCmd("LOFF\r\n"),endStr("\r\n")
{
    _stop = false;
}

/**
 * @brief 通信检测
 * @return
 */
bool KeyenceSR::CanAcess()
{
    string result = WriteAndRead(testCmd+endStr,"Ascii", 200);
    if (!result.empty() && result.length() > 0)
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}

/**
 * @brief 开始扫码
 */
void KeyenceSR::StartScan()
{
    Write(startCmd,"Ascii");
}

void KeyenceSR::Close()
{
    _stop = true;
    if (Socket_fd != -1)
    {
        close(Socket_fd);
        _log.LOG_DEBUG("KeyenceSR 设备【%s】 端口关闭成功！",Name.data());
    }
}

/**
 * @brief 停止扫码
 */
void KeyenceSR::StopScan()
{
    Write(stopCmd,"Ascii");
}

/**
 * @brief 获取扫码结果
 * @return
 */
string KeyenceSR::GetBarCode()
{
    return Read("Ascii");
}

/**
 * @brief 自动扫码
 * @param times     超时时间
 * @return
 */
string KeyenceSR::AutoScan(double times)
{
    string barcode = "";
    struct timeval tv_start,tv_current;
    double total_time=0;
    gettimeofday(&tv_start,0);
    while(!_stop)
    {
        barcode = WriteAndRead(startCmd,"Ascii",200);
        Write(stopCmd,"Ascii");
        gettimeofday(&tv_current,0);
        total_time = (tv_current.tv_sec - tv_start.tv_sec)*1000.0;
        total_time += (tv_current.tv_usec - tv_start.tv_usec)/1000.0;    //ms
        _log.LOG_DEBUG("KeyenceSR 【%s】 AutoScan total_time = 【%f】 times = 【%f】",Name.data(),total_time/1000.0,times);
        if(total_time > times * 1000.0)
        {
            _stop = true;
        }
        if(!barcode.empty() && StringToUpper(barcode).find("ER") == string::npos)
        {
            break;
        }
        //usleep(50 * 1000);   //200ms
    }
    _stop = false;
    return barcode.empty() ? "": Trim(barcode);
}
