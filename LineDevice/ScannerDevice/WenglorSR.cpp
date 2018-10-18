#include "WenglorSR.h"

/**
 * @brief 无参构造
 */
WenglorSR::WenglorSR():Command("<?>")
{
}

/**
 * @brief 通过串口参数、设备名称执行构造
 * @param spp
 * @param name
 */
WenglorSR::WenglorSR(SerialPortParameters *spp, string name) : ComDevice(spp,name), Command("<?>")
{
    _stop=false;
}

/**
 * @brief 通信检测
 * @return
 */
bool WenglorSR::CanAcess()
{
    string result = ComDevice::WriteAndRead(Command,"Ascii",200);
    if(!result.empty() && result.length()>0)
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}

/**
 * @brief 自动扫码
 * @param times     超时时间
 * @return
 */
string WenglorSR::AutoScan(int times)
{
    string codes;
    struct timeval tv_start,tv_current;
    int total_time;

    gettimeofday(&tv_start,0);
    ComDevice::Read("Ascii");

    while(!_stop)
    {
        usleep(1000*1000);
        codes = ComDevice::Read("Ascii");
        if(!codes.empty())
        {
            codes = TrimTo(codes);      // 把"\r\n"变成","
            vector<string> bar;
            splittostring(codes,",",bar);  //去掉","
            if(bar.size() > 3)
            {
                string barCode = ModeFind_string(bar);
                gettimeofday(&tv_current,0);
                total_time = (tv_current.tv_sec - tv_start.tv_sec);
                total_time += (tv_current.tv_usec - tv_start.tv_usec) / 1000000;    //ms
                if(total_time > times || !barCode.empty())
                {
                    return barCode;
                }
            }
        }
    }
    return "";
}
