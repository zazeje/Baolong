#include "MicrometerTcp.h"


/**
 * @brief MicrometerTcp::MicrometerTcp            集线器设备驱动
 * @param spp
 */
MicrometerTcp::MicrometerTcp(int port , string ip) : TcpDevice(port,ip)
{
    _getValue = "8003000000085A1D";
    _clearValue = "80060800AB566AB5";
    WriteAndRead(_clearValue, "Hex", 70);
}


/**
 * @brief MicrometerTcp::MicrometerTcp            集线器设备驱动
 * @param spp
 * @param name
 */
MicrometerTcp::MicrometerTcp(int port , string ip, string name) : TcpDevice(port,ip,name)
{
    _getValue = "8003000000085A1D";
    _clearValue = "80060800AB566AB5";
    WriteAndRead(_clearValue, "Hex", 70);
}


/**
 * @brief MicrometerTcp::~MicrometerTcp
 */
MicrometerTcp::~MicrometerTcp()
{

}


/**
 * @brief MicrometerTcp::CanAcess
 * @return
 */
bool MicrometerTcp::CanAcess()
{
    string result = WriteAndRead(_getValue, "Hex", 70);
    if(!result.empty())
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}




/**
 * @brief MicrometerTcp::GetValue              读集线器设备的值
 * @return
 */
vector<double> MicrometerTcp::GetValue()
{
    vector<double> resultVec;
    string result = WriteAndRead(_getValue, "Hex", 70);
//    cout<<"---result = "<<result<<" -- length = "<<result.length()<<endl;
    if(result.empty())
        return resultVec;
    if(result.length() == 42 && !result.substr(0,6).compare("800310"))
        result = result.substr(6,32);
//    cout<<"-- result = "<<result<<endl;
    if(result.length() == 32)
    {
        int sign1;
        if(!result.substr(0,2).compare("00"))
            sign1 = 1;
        else
            sign1 = -1;
        int value1 = hextodec(result.substr(4,4));
        double res1 = value1 * sign1 / 1000.0;
//        cout<<"-- res1 = "<<res1<<endl;
//        resultVec.push_back(DoubleToString(res1,"%.2f"));
        resultVec.push_back(res1);

        int sign2;
        if(!result.substr(8,2).compare("00"))
            sign2 = 1;
        else
            sign2 = -1;
        int value2 = hextodec(result.substr(12,4));
        double res2 = value2 * sign2 / 1000.0;
//        cout<<"--- res2 = "<<res2<<endl;
        resultVec.push_back(res2);

        int sign3;
        if(!result.substr(16,2).compare("00"))
            sign3 = 1;
        else
            sign3 = -1;
        int value3 = hextodec(result.substr(20,4));
        double res3 = value3 * sign3 / 1000.0;
        resultVec.push_back(res3);

        int sign4;
        if(!result.substr(24,2).compare("00"))
            sign4 = 1;
        else
            sign4 = -1;
        int value4 = hextodec(result.substr(28,4));
        double res4 = value4 * sign4 / 1000.0;

//        resultVec.push_back(DoubleToString(res2,"%.2f"));
        resultVec.push_back(res4);
        return resultVec;
    }
    return resultVec;
}


/**
 * @brief MicrometerTcp::ClearValue
 */
void MicrometerTcp::ClearValue()
{
    WriteAndRead(_clearValue, "Hex", 70);
}
