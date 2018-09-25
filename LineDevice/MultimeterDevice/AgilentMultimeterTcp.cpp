#include "AgilentMultimeterTcp.h"
#include "main.h"
#define _DelayTime 50


/**
 * @brief 用端口以及ip地址构造
 * @param port  端口号
 * @param ip    ip地址
 */
AgilentMultimeterTcp::AgilentMultimeterTcp(int port ,string ip) : TcpDevice(port,ip)
{
    DispOn = "DISP:ON\n";
    DispOff = "DISP:OFF\n";
    SysiBeep = "SYSI:BEEP\n";
    MeasVoltDc = "MEAS:VOLT:DC?\n";
    MeasCurrDc = "MEAS:CURR:DC?\n";
    _read = "READ?\n";
    SystErr = "SYST:ERR?\n";
    Idn = "*IDN?\n";
    Cls = "*CLS\n";
    _readByBatch = "READ:ARR?\n";
    endStr = "\n";
    BatchCount = "TRIG:COUNT 80";
    INIT = "INIT";
//    readBatch = "READ:ARR?\n";
//    readBatch = ":FETC:ARR?\n";
    Trg = "TRIG:TIM 0.05";
    SourBus = "TRIG:SOUR TIM";
    Tim = "TRIG:TIM 5E-2\n";
}

/**
 * @brief 使用端口号、ip地址以及设备名称进行构造
 * @param port  端口号
 * @param ip    ip地址
 * @param name  设备名称
 */
AgilentMultimeterTcp::AgilentMultimeterTcp(int port,string ip,string name) : TcpDevice(port,ip,name)
{
    DispOn = "DISP:ON\n";
    DispOff = "DISP:OFF\n";
    SysiBeep = "SYSI:BEEP\n";
    MeasVoltDc = "MEAS:VOLT:DC?\n";
    MeasCurrDc = "MEAS:CURR:DC?\n";
    _read = "READ?\n";
    SystErr = "SYST:ERR?\n";
    Idn = "*IDN?\n";
    Cls = "*CLS\n";
    _readByBatch = ":FETC:ARR?\n";
    endStr = "\n";
    BatchCount = "TRIG:COUNT 80";
    INIT = "INIT";
    Trg = "TRIG:TIM 0.05";
    SourBus = "TRIG:SOUR TIM";
    Tim = "TRIG:TIM 5E-2\n";
}

/**
 * @brief 通信测试
 * @return
 */
bool AgilentMultimeterTcp::CanAcess()
{
    string respones = WriteAndRead(Idn,"Ascii",_DelayTime);
//    cout<<"respones = "<<respones<<endl;
    if ( respones.find("agilent") != std::string::npos || respones.find("Agilent") != std::string::npos || respones.find("Keysight") != std::string::npos)
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}

/**
 * @brief 读万用表的值
 * @return
 */
string AgilentMultimeterTcp::GetValue()
{
    string result = WriteAndRead(_read,"Ascii",_DelayTime);
    char** str;
    if (!result.empty() && result.length() > 0)
    {
        string tmp = Trim(result);
        double num = strtod(tmp.data(),str);
        return DigitChange(num,20);
    }
    return "";
}

/**
 * @brief AgilentMultimeterTcp::GetValueByBatch         批量读取万用表的值
 * @return returnRes                                    returnRes为80个电流数据，每个数据之间用'/'分割
 */
string AgilentMultimeterTcp::GetValueByBatch(int _coefficient)
{
    SendCommand(INIT);
    usleep(4500 * 1000);
    string result = WriteAndRead(_readByBatch,"Ascii",100);
//   _log.LOG_DEBUG("AgilentMultimeterTcp 批量读取电流值 【%s】",result.data());
    string returnRes = "";
    vector<string> values;
    //将采集到的电流数据以','进行分割
    splittostring(result, ',', values);
    //将每一个以科学计数法表示的电流数据转换为小数，并以'/'为分隔进行拼接
    for(int i = 0;i < values.size();i++)
    {
        char** str;
        if (!values[i].empty() && values[i].length() > 0)
        {
            double num = strtod(Trim(values[i]).data(),str);
            string tmpString = DigitChange(num,10);
            double tmp = fabs(atof(tmpString.data()) * _coefficient);
            if(tmp > 10000)
                continue;
            returnRes.append(DoubleToString(tmp, "%.2f").append("/"));
        }
    }
//    SendCommand(":INP OFF");
    return returnRes;
}


/**
 * @brief AgilentMultimeterTcp::SetWorkModeByBatch          设置批量读取工作模式
 */
bool AgilentMultimeterTcp::SetWorkModeByBatch()
{
    if(!SendCommand(SourBus))
        return false;
    usleep(10 * 1000);
    if(!SendCommand(Trg))
        return false;
    usleep(10 * 1000);
    if(!SendCommand(BatchCount))
        return false;
    usleep(10 * 1000);
    return true;
}

/**
 * @brief AgilentMultimeterTcp::InitDevice
 */
bool AgilentMultimeterTcp::InitDevice()
{  
#ifdef _TruckLine
    if(!SendCommand(m_para1))
        return false;
    return true;
#else

    //初始化皮安计电流使能参数
    if(!SendCommand(m_para1))
        return false;
    if((workCenterNo.compare("3218") != 0) && (workCenterNo.compare("3309") != 0) && (workCenterNo.compare("20035") != 0))
    {
        //初始化皮安计输出参数
        if(!SendCommand(m_para2))
            return false;
        //设置批量读取工作模式
        if(!SetWorkModeByBatch())
            return false;
    }
    _log.LOG_DEBUG("AgilentMultimeterTcp 【%s】 InitDevice Success，currentEnablePara=【%s】 outputFormatPara=【%s】",Name.data(), m_para1.data(),m_para2.data());
    return true;
#endif
}


/**
 * @brief 将安捷伦表初始化的参数
 * @param m_para1   要初始化的电流使能参数
 * @param m_para2    要初始化的输出格式参数
 */
void AgilentMultimeterTcp::InitPara(string para1, string para2)
{
    m_para1 = para1;
    m_para2 = para2;
}


/**
 * @brief 发送指令
 * @param command   指令内容
 * @return
 */
bool AgilentMultimeterTcp::SendCommand(string command)
{
    if (command.empty()) return false;
    return Write(command + endStr, "Ascii");
}

/**
 * @brief 从万用表接收数据
 * @return
 */
string AgilentMultimeterTcp::ReadData()
{
    return Read("Ascii");
}

/**
 * @brief 读取IDN
 * @return
 */
string AgilentMultimeterTcp::GetIdn()
{
    return WriteAndRead(Idn,"Ascii",_DelayTime);
}

/**
 * @brief 设置万用表屏幕是否显示
 * @param display
 * @return
 */
bool AgilentMultimeterTcp::SetDisplay(bool display)
{
    return Write((display ? DispOn : DispOff),"Ascii");
}

/**
 * @brief 读取错误信息
 * @return
 */
string AgilentMultimeterTcp::GetSystemError()
{
    return WriteAndRead(SystErr,"Ascii",_DelayTime);
}

/**
 * @brief 写入报警请求
 * @return
 */
bool AgilentMultimeterTcp::Beep()
{
    return Write(SysiBeep,"Ascii");
}

/**
 * @brief 清除错误信息
 * @return
 */
bool AgilentMultimeterTcp::CleraError()
{
    return Write(Cls,"Ascii");
}
