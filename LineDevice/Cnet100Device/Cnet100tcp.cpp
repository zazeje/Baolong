#include "Cnet100tcp.h"

Cnet100Tcp::Cnet100Tcp(int port, string ip) : TcpDevice(port,ip)
{

}

Cnet100Tcp::Cnet100Tcp(int port , string ip, string name) : TcpDevice(port,ip,name)
{

}

Cnet100Tcp::~Cnet100Tcp()
{

}

bool Cnet100Tcp::setWorkMode24G1T1R(workmode mode)
{
    int trycount = 0;
    string value,m;
    if(mode == staticMode)
        m = "01";
    else if(mode == dynamicMode)
        m = "00";
    value = "0800000333" + m + "00000000000000";
    Write(value,"Hex");
    string result = "";
    while(1){
        result = Read("Hex");
        //cout<<"zz "<<result<<" "<<result.length() << " "<< result.substr(0,10)<< " " << result.substr(14,2)<< endl;
        if(result.length() == 52 && result.substr(0,10).compare("0800000322") == 0 && result.substr(14,2).compare(m) == 0){
            return true;
        }else{
            trycount ++;
            if(trycount > 20)
                return false;
        }
        usleep(100);
        //cout<<"zz write "<< Write(value,"Hex")<<" "<<value<<endl;
    }
}

string Cnet100Tcp::getMessage()
{
    return Read("Hex");
}

bool Cnet100Tcp::CanAcess()
{
    return true;
}

vector<float> Cnet100Tcp::analysis77G500(const string &str)
{
    string ChirpNumber = str.substr(40,8);
    string OutMode  = str.substr(55,1);
    string SNR = str.substr(49,11);
    string Range = str.substr(60,12);
    string Velocity = str.substr(72,14);
    string Angle = str.substr(86,12);
    string TRK = str.substr(98,3);
    vector<float> ret;
    ret.push_back(float(binaryStrToInt(ChirpNumber)));
    ret.push_back(float(binaryStrToInt(OutMode)));
    ret.push_back(float(binaryStrToInt(SNR))/10);
    ret.push_back(float(binaryStrToInt(Range))/10);
    ret.push_back(float(binaryStrToInt(Velocity))/100 - 81.92);
    ret.push_back(float(binaryStrToInt(Angle))/10 - 180);
    ret.push_back(float(binaryStrToInt(TRK)));
    cout<<float(binaryStrToInt(ChirpNumber))<<" "<<float(binaryStrToInt(Range))/10<<" sudu : "<<float(binaryStrToInt(Velocity))/100 - 81.92<<endl;
    return ret;
}

vector<unsigned int> Cnet100Tcp::analysis24G1T1R323(const string &str)
{
    string TargetVelocity = str.substr(40,32);
    string TargetDirection  = str.substr(72,8);
    string TargetExistance = str.substr(80,8);
    vector<unsigned int> ret;
    ret.push_back(binaryStrToInt(TargetVelocity));
    ret.push_back(binaryStrToInt(TargetDirection));
    ret.push_back(binaryStrToInt(TargetExistance));
    return ret;
}
