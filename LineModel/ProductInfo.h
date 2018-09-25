#ifndef PRODUCTINFO_H
#define PRODUCTINFO_H

#include <string>

using namespace std;

class ProductInfo
{
public:
    ProductInfo();
    void Clear();
    string AvgValue;
    string MinValue;
    string MaxValue;
    double BadValue;
    string EigenValue;
    double PassRate;
    int HightCount;
    int LowCount;
    int PassCount;
    int ValueCount;
    double MinPassRate;
    int JudgeResult;
    string ErrorCode;

    string ProductId;
    string ProductSerialNo;
    string CraftName;
    string CraftId;
    string CollectValue;
    string CreatUser;
    int CreatTime;
    string ModifyUser;
    int ModifyTime;

    string BillNo;
    string DeviceId;
    int LineId;
    string ProductModel;
    int ProductCount;

    string ProductName;
    string LastCraft;

    int version;

    string LightAndRain_ch1_ADC;
    string LightAndRain_ch2_ADC;
    string LightAndRain_Light_On;
    string LightAndRain_TwiLight_On;
    string LightAndRain_FW;
    string LightAndRain_IR;
    string LightAndRain_Rain;
};
#endif // PRODUCTINFO_H
