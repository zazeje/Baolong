#include "SimulatorTcp.h"

SimulatorTcp::SimulatorTcp(int port , string ip) : TcpDevice(port,ip)
{
}

SimulatorTcp::SimulatorTcp(int port , string ip, string name) : TcpDevice(port,ip,name)
{
}

SimulatorTcp::~SimulatorTcp()
{

}

bool SimulatorTcp::CanAcess()
{
    string result = WriteAndRead("10010A0000000000001B1003", "Hex", 70);
    if(!result.empty())
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}

bool SimulatorTcp::SendSimulateData()
{
    string result = WriteAndRead(CreateCommand(),"Hex",100);
    if(result.length() == 16 && result.substr(6,2).compare("01") == 0){
        return true;
    }else
        return false;
}

string SimulatorTcp::CreateCommand()
{
    string create = "100134";
    yxms.compare("1")?create+="01":create+="00";
    double ftemp = atof(sjkz.data());
    int ftemp100 = ftemp * 100;
    if(ftemp100 % 50 != 0)
        return "";
    unsigned short i16 = ftemp/0.5;
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    i16 = atoi(dk.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    ftemp = atof(mk.data());
    unsigned int i32 = 1/ftemp * 4294967296;
    create+= MemoryOutPut((const char*)&i32,sizeof(i32));
    i16 = atoi(mbjl1.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    i16 = atoi(mbjl2.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    i16 = atoi(mbjl3.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    i16 = atoi(mbjl4.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    short i16s;
    i16s = atoi(mbsd1.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    i16s = atoi(mbsd2.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    i16s = atoi(mbsd3.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    i16s = atoi(mbsd4.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    i16 = atoi(jg.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    char mbkg = 0;
    if(mbkg1.compare("1") == 0)
        mbkg = mbkg|0x01;
    if(mbkg2.compare("1") == 0)
        mbkg = mbkg|0x02;
    if(mbkg3.compare("1") == 0)
        mbkg = mbkg|0x04;
    if(mbkg4.compare("1") == 0)
        mbkg = mbkg|0x08;
    create += MemoryOutPut(&mbkg,sizeof(mbkg));
    create += "00";
    i32 = atoi(zxpl.data());
    create += MemoryOutPut((const char*)&i32,sizeof(i32));
    i16 = atoi(jlbc.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    i16s = atoi(plbc1.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    i16s = atoi(plbc2.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    i16s = atoi(plbc3.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    i16s = atoi(plbc4.data());
    create+= MemoryOutPut((const char*)&i16,sizeof(i16));
    create += "00";

    int temp = 0;
    string hexStr = "";
    for (int i = 0; i < create.length(); i += 2)
    {
        temp = temp + hextodec(create.substr(i, 2));
    }
    char tmp[10] = {0};
    sprintf(tmp,"%X",temp);
    hexStr = tmp;
    if (hexStr.length() == 1)
        hexStr = "0" + hexStr;
    if (hexStr.length() > 2)
        hexStr = hexStr.substr(hexStr.length() - 2);

    create += hexStr;
    create += "1003";

    return create;
}

bool SimulatorTcp::setPara(vector<string> Para)
{
    if(Para.size() != 23)
        return false;
    yxms = Para[0];
    sjkz = Para[1];
    dk = Para[2];
    mk = Para[3];
    mbjl1 = Para[4];
    mbjl2 = Para[5];
    mbjl3 = Para[6];
    mbjl4 = Para[7];
    mbsd1 = Para[8];
    mbsd2 = Para[9];
    mbsd3 = Para[10];
    mbsd4 = Para[11];
    mbkg1 = Para[12];
    mbkg2 = Para[13];
    mbkg3 = Para[14];
    mbkg4 = Para[15];
    jg = Para[16];
    zxpl = Para[17];
    jlbc = Para[18];
    plbc1 = Para[19];
    plbc2 = Para[20];
    plbc3 = Para[21];
    plbc4 = Para[22];
    return true;
}
