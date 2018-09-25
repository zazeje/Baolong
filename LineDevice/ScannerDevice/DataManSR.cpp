#include "DataManSR.h"

/**
 * @brief 无参构造
 */
DataManSR::DataManSR():TestCmd("DM60"),StartCmd("LON"),StopCmd("LOFF")
{
    cmd = "";
    _Stop = false;
}

/**
 * @brief 通过串口参数、设备名称执行构造
 * @param spp   包含串口参数的指针
 * @param name  设备名称
 */
DataManSR::DataManSR(SerialPortParameters *spp, string name):ComDevice(spp,name),\
    TestCmd("DM60"),StartCmd("LON"),StopCmd("LOFF")
{
    cmd = "";
    _Stop = false;
}

/**
 * @brief 通信检测
 * @return
 */
bool DataManSR::CanAcess()
{
    string cmd = TestCmd + endStr;
    m_communicateStatus = false;
    string result = WriteAndRead(cmd,"Ascii",250);
    if(!result.empty() && result.length()>0)
    {
        if(result.find("DM60") != std::string::npos)
        {
            m_communicateStatus = true;
        }
    }
    return m_communicateStatus;
}

/**
 * @brief 开始扫码
 */
void DataManSR::StartScan()
{
   Write(StartCmd + ComDevice::endStr,"Ascii");
}

/**
 * @brief 停止扫码
 */
void DataManSR::StopScan()
{
    Write(StopCmd + ComDevice::endStr,"Ascii");
}

/**
 * @brief 获取扫码结果
 * @return
 */
string DataManSR::GetBarCode()
{
    return Read("Ascii");
}

/**
 * @brief 自动扫码
 * @param times     超时时间
 * @return
 */
string DataManSR::AutoScan(int times)
{
    struct timeval tv_start,tv_current;
    int total_time;
    string barCodes;
    string mybarcodes;

    gettimeofday(&tv_start,0);
    StartScan();
    while(!_Stop)
    {
        barCodes.append(GetBarCode());
        gettimeofday(&tv_current,0);
        total_time = (tv_current.tv_sec - tv_start.tv_sec);
        total_time += (tv_current.tv_usec - tv_start.tv_usec) / 1000000;    //ms
        if((total_time > times)) _Stop = true;
    }
    StopScan();
    _Stop = false;
    mybarcodes = TrimTo(barCodes);      // 把"\r\n"变成","
    vector<string> bar;
    splittostring(mybarcodes,",",bar);  //去掉","
    return ModeFind_string(bar);
}

