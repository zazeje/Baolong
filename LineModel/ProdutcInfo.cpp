#include "ProductInfo.h"

ProductInfo::ProductInfo()
{
    AvgValue.clear();
    MinValue.clear();
    MaxValue.clear();
    BadValue = 0.0;
    EigenValue.clear();
    PassRate = 0.0;
    HightCount = 0;
    LowCount = 0;
    PassCount = 0;
    ValueCount = 0;
    MinPassRate = 0.0;
    JudgeResult = 0;
    ErrorCode.clear();

    ProductId.clear();
    ProductSerialNo.clear();
    CraftName.clear();
    CraftId.clear();
    CollectValue.clear();
    CreatUser.clear();
    CreatTime = 0;
    ModifyUser.clear();
    ModifyTime = 0;

    BillNo.clear();
    DeviceId.clear();
    LineId = 0;
    ProductModel.clear();
    ProductCount = 0;

    ProductName.clear();
    LastCraft.clear();

    version = 0;

    LightAndRain_ch1_ADC.clear();
    LightAndRain_ch2_ADC.clear();
    LightAndRain_Light_On.clear();
    LightAndRain_TwiLight_On.clear();
    LightAndRain_FW.clear();
    LightAndRain_IR.clear();
    LightAndRain_Rain.clear();
}

void ProductInfo::Clear()
{
    AvgValue.clear();
    MinValue.clear();
    MaxValue.clear();
    BadValue = 0.0;
    EigenValue.clear();
    PassRate = 0.0;
    HightCount = 0;
    LowCount = 0;
    PassCount = 0;
    ValueCount = 0;
    MinPassRate = 0.0;
    JudgeResult = 0;
    ErrorCode.clear();

    ProductId.clear();
    ProductSerialNo.clear();
    CraftName.clear();
    CraftId.clear();
    CollectValue.clear();
    CreatUser.clear();
    CreatTime = 0;
    ModifyUser.clear();
    ModifyTime = 0;

    BillNo.clear();
    DeviceId.clear();
    LineId = 0;
    ProductModel.clear();
    ProductCount = 0;

    ProductName.clear();
    LastCraft.clear();

    version = 0;

    LightAndRain_ch1_ADC.clear();
    LightAndRain_ch2_ADC.clear();
    LightAndRain_Light_On.clear();
    LightAndRain_TwiLight_On.clear();
    LightAndRain_FW.clear();
    LightAndRain_IR.clear();
    LightAndRain_Rain.clear();
}
