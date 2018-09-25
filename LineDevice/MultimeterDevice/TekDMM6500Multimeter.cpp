#include "TekDMM6500Multimeter.h"
#include "main.h"
#define _DelayTime 50

TekDMM6500MultimeterTcp::TekDMM6500MultimeterTcp(int port ,string ip) : TcpDevice(port,ip)
{
    SysiBeep = ":SYSTem:BEEPer 500, 1";
    FuncVoltDc = ":SENS:FUNC \"VOLT:DC\"";
    FuncCurrDc = ":SENS:FUNC \"CURR:DC\"";
    VoltRang = ":SENS:VOLT:RANG ";
    CurrRang = ":SENS:CURR:RANG ";
    VoltNplc = ":SENS:VOLT:NPLc ";
    CurrNplc = ":SENS:CURR:NPLC ";
    m_Read = "READ?";
    SystErr = "SYST:ERR?";
    Idn = "*IDN?";
    Cls = "*CLS";
    Nplc1 = ":SENS:FRES:NPLC 1";
    endStr = "\n";

    MakeBuf100 = "TRAC:MAKE \"buf100\", 80";
    LoadBuf100 = "TRIGger:LOAD \"SimpleLoop\", 80, 0,\"buf100\"";
    InitRead = "INIT";
    WaitRead = "*WAI";
    GetBuf100 = "TRAC:DATA? 1, 80, \"buf100\"";
    ClearBuf100 = "TRACe:CLEar \"buf100\"";
}

TekDMM6500MultimeterTcp::TekDMM6500MultimeterTcp(int port,string ip,string name) : TcpDevice(port,ip,name)
{
    SysiBeep = ":SYSTem:BEEPer 500, 1";
    FuncVoltDc = ":SENS:FUNC \"VOLT:DC\"";
    FuncCurrDc = ":SENS:FUNC \"CURR:DC\"";
    VoltNplc = ":SENS:VOLT:NPLc ";
    CurrNplc = ":SENS:CURR:NPLC ";
    VoltRang = ":SENS:VOLT:RANG ";
    CurrRang = ":SENS:CURR:RANG ";
    m_Read = "READ?";
    SystErr = "SYST:ERR?";
    Idn = "*IDN?";
    Cls = "*CLS";
    Nplc1 = ":SENS:FRES:NPLC 1";
    endStr = "\n";

    MakeBuf100 = "TRAC:MAKE \"buf100\", 80";
    LoadBuf100 = "TRIGger:LOAD \"SimpleLoop\", 80, 0,\"buf100\"";
    InitRead = "INIT";
    WaitRead = "*WAI";
    GetBuf100 = "TRAC:DATA? 1, 80, \"buf100\"";
    ClearBuf100 = "TRACe:CLEar \"buf100\"";
}

bool TekDMM6500MultimeterTcp::CanAcess()
{
    string respones = WriteAndRead(Idn+endStr,"Ascii",_DelayTime);
//    cout<<"respones = "<<respones<<endl;
    if (respones.find("KEITHLEY") != std::string::npos)
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}

string TekDMM6500MultimeterTcp::GetValue()
{
    string result = WriteAndRead(m_Read+endStr,"Ascii",_DelayTime);
    char** str;
    if (!result.empty() && result.length() > 0)
    {
        string tmp = Trim(result);
        double num = strtod(tmp.data(),str);
        return DigitChange(num,10);
    }
    return "";
}

bool TekDMM6500MultimeterTcp::SendCommand(string command)
{
    if (command.empty()) return false;
    return Write(command,"Ascii");
}

string TekDMM6500MultimeterTcp::ReadData()
{
    return Read("Ascii");
}

string TekDMM6500MultimeterTcp::GetIdn()
{
    return WriteAndRead(Idn,"Ascii",_DelayTime);
}

string TekDMM6500MultimeterTcp::GetSystemError()
{
    return WriteAndRead(SystErr,"Ascii",_DelayTime);
}

bool TekDMM6500MultimeterTcp::Beep()
{
    return Write(SysiBeep,"Ascii");
}

bool TekDMM6500MultimeterTcp::CleraError()
{
    return Write(Cls,"Ascii");
}

bool TekDMM6500MultimeterTcp::MeasVolt(double rang,double nplc)
{
    bool ok = false;
    ok = Write(FuncVoltDc+endStr,"Ascii");
    if(rang)
        ok = Write(VoltRang+DoubleToString(rang)+endStr,"Ascii");
    if(nplc)
        ok = Write(VoltNplc+DoubleToString(nplc)+endStr,"Ascii");
    return ok;
}

bool TekDMM6500MultimeterTcp::MeasCurr(double rang,double nplc)
{
    bool ok = false;
    ok = Write(FuncCurrDc+endStr,"Ascii");
    if(rang)
        ok = Write(CurrRang+DoubleToString(rang)+endStr,"Ascii");
    if(nplc)
        ok = Write(CurrNplc+DoubleToString(nplc)+endStr,"Ascii");
    return ok;
}

bool TekDMM6500MultimeterTcp::SetWorkModeByBatch()
{
    if(!Write(Cls+endStr,"Ascii"))
        return false;
    if(!Write(MakeBuf100+endStr,"Ascii"))
        return false;

    if(!Write(LoadBuf100+endStr,"Ascii"))
        return false;
    sleep(1);
    return true;
}

string TekDMM6500MultimeterTcp::GetValueByBatch(int _coefficient)
{
    string result = "";
    if(!Write(InitRead+endStr,"Ascii"))
        return result;
    usleep(4500 * 1000);
    result = WriteAndRead(GetBuf100+endStr,"Ascii",100);
    _log.LOG_DEBUG("TekDMM6500Tcp 批量读取电流值 【%s】",result.data());

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
    Write(ClearBuf100+endStr,"Ascii");
    return returnRes;
}

bool TekDMM6500MultimeterTcp::InitDevice()
{
    bool bRet = true;
    //初始化万用表档位
    if(!SendCommand(m_gearPara))
    {
        bRet = false;
    }
    else
    {
        if(!SendCommand(m_rangePara1))
        {
            bRet = false;
        }
        else
        {
            if(!SetWorkModeByBatch())
            {
                bRet = false;
            }
        }
    }
    _log.LOG_DEBUG("AgilentMultimeterTcp 【%s】 InitDevice【%s】，gearPara=【%s】 rangePara1=【%s】 rangePara2=【%s】",Name.data(),bRet?"OK":"NG", m_gearPara.data(),m_rangePara1.data(),m_rangePara2.data());
    return bRet;
}

/**
 * @brief 将泰克万用表初始化的参数
 * @param gearPara   要初始化的万用表档位参数
 * @param rangePara1    要初始化的万用表量程参数1
 * @param rangePara2    要初始化的万用表量程参数2
 */
void TekDMM6500MultimeterTcp::InitPara(string gearPara,string rangePara1,string rangePara2)
{
    m_gearPara = gearPara;
    m_rangePara1 = rangePara1;
    m_rangePara2 = rangePara2;
}
