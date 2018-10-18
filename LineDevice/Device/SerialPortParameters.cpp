#include "SerialPortParameters.h"

SerialPortParameters:: SerialPortParameters()
{}


SerialPortParameters:: SerialPortParameters(string portName)
{
    _portName = StringToUpper(portName);
    _baudRate = 9600;
    _dataBits = 8;
    _stopBits = 0;
    _parity = 0;
}

SerialPortParameters::SerialPortParameters(string portName, int baudRate, int dataBits, int stopBits, int parity)
{
    _portName = StringToUpper(portName);
    _baudRate = baudRate;
    _dataBits = dataBits;
    _stopBits = stopBits;
    _parity = parity;
}

SerialPortParameters::SerialPortParameters(string portName, string baudRate, string dataBits, string stopBits, string parity)
{
    _portName = StringToUpper(portName);
    _baudRate = atoi(baudRate.data());
    _dataBits = atoi(dataBits.data());
    _stopBits = atoi(stopBits.data());
    _parity = atoi(parity.data());
}

/**
 * @brief 通过DeviceInfo获取spp
 * @param di
 * @return
 */
SerialPortParameters* SerialPortParameters::GetSpp(DeviceInfo di)
{
    SerialPortParameters *spp = new SerialPortParameters(di.Port, di.BaudRate, di.DataBits, di.StopBits, di.Parity);
    return spp;
}
