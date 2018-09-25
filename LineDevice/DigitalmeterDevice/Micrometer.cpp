#include "Micrometer.h"


/**
 * @brief Micrometer::Micrometer            集线器设备驱动
 * @param spp
 */
Micrometer::Micrometer(SerialPortParameters *spp) : ComDevice(spp)
{
    _getValue = "8003000000085A1D";
    _clearValue = "80060800AB566AB5";
    WriteAndRead(_clearValue, "Hex", 70);
}


/**
 * @brief Micrometer::Micrometer            集线器设备驱动
 * @param spp
 * @param name
 */
Micrometer::Micrometer(SerialPortParameters *spp, string name) : ComDevice(spp,name)
{
    _getValue = "8003000000085A1D";
    _clearValue = "80060800AB566AB5";
    WriteAndRead(_clearValue, "Hex", 70);
}


/**
 * @brief Micrometer::~Micrometer
 */
Micrometer::~Micrometer()
{

}


/**
 * @brief Micrometer::CanAcess
 * @return
 */
bool Micrometer::CanAcess()
{
    string result = WriteAndRead(_getValue, "Hex", 70);
    if(!result.empty())
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}




/**
 * @brief Micrometer::GetValue              读集线器设备的值
 * @return
 */
vector<double> Micrometer::GetValue()
{
    vector<double> resultVec;
    string result = WriteAndRead(_getValue, "Hex", 70);
//    cout<<"---result = "<<result<<" -- length = "<<result.length()<<endl;
    if(result.empty())
        return resultVec;
    if(result.length() == 42 && !result.substr(0,6).compare("800310"))
        result = result.substr(6,16);
//    cout<<"-- result = "<<result<<endl;
    if(result.length() == 16)
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

//        resultVec.push_back(DoubleToString(res2,"%.2f"));
        resultVec.push_back(res2);
        return resultVec;
    }
    return resultVec;
}


/**
 * @brief Micrometer::ClearValue
 */
void Micrometer::ClearValue()
{
    WriteAndRead(_clearValue, "Hex", 70);
}
