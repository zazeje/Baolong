#ifndef I8O8CARD_H
#define I8O8CARD_H

#include "./LineDevice/Device/ComDevice.h"
#include "./LineDevice/Device/SerialPortParameters.h"
#include "./DataConvert/DataType.h"

class I808Card : public ComDevice
{

public:

    I808Card(SerialPortParameters *spp, string deviceName);
    I808Card(string portName, string deviceName);
    ~I808Card();

    //获取地址
    void GetCardAddress();
    //开继电器
    string OpenY(int position);
    //开继电器
    string OpenY(string position);
    //关继电器
    string CloseY(int position);
    //关继电器
    string CloseY(string position);
    //控制所有继电器
    void ControlAll(bool IsClose);
    //控制部分继电器
    void ControlSome(string positions, bool isClose);
    //读Y继电器状态
    string ReadYStatus();
    //读X继电器状态
    string ReadXStatus();
    //通信检测
    bool CanAcess();

private:

    string _controlCommand;
    string _readStateCommand;
    string cardAddress;
    //string responsePrefix;
    string getAddressCmd;

    //控制继电器
    string ControlY(int position, bool isClose);

};

#endif //I8O8CARD_H
