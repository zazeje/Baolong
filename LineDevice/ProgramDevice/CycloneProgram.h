#ifndef CYCLONEPROGRAM_H
#define CYCLONEPROGRAM_H

#include "../Device/ComDevice.h"
#include "../Device/SerialPortParameters.h"
#include "./DataConvert/DataType.h"


//#define SIZE 1000*1024      //1M


class CycloneProgram : public ComDevice
{

public:

    CycloneProgram(SerialPortParameters *spp, string name);

    bool SendAndCheck(string command, string result, unsigned long timeout,string crc8);
    bool SendAndCheck_str(string command, string result, unsigned long timeout);
    bool CanAcess();
    bool IsInWorking();
    bool WriteProgramAndSetId(string model, string id);
    bool StartProgram();
    bool ClearError();
    bool DeleteAllSap();
    bool SetDeviceId(string model, string id);
    bool SetDeviceId_str(string model, string id);
    bool AddSapFile(string fileContent);
    bool AddSapFileByName(string filePath);
    int GetSapCount();

    void test();

    string GetLastError();
    string WriteProgramAndGetId();
    string GetId(int delay, string addressString);
    string GetIdOperator();      //modify by vincent
    string GetDeviceIp();

private:

    int d1f5Count;
    int dfb2Count;

};

#endif // CYCLONEPROGRAM_H
