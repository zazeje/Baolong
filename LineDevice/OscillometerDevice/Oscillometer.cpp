#include "Oscillometer.h"


#define SICL_ADDRESS      "lan,4880;hislip[141.121.237.226]:hislip0"
#define TIMEOUT           15000
#define IEEEBLOCK_SPACE   5000000

char str_result[256] = {0};     /* Result from do_query_string(). */
unsigned char ieeeblock_data[IEEEBLOCK_SPACE];   /* Result from
                                   do_query_ieeeblock(). */
signed short ieeeblock_data_words[IEEEBLOCK_SPACE];   /* Result from
                                   do_query_ieeeblock_words(). */
double dbl_results[10];         /* Result from do_query_numbers(). */

OscillometerTcp::OscillometerTcp(int port , string ip) : TcpDevice(port, ip)
{
    _idn = "*IDN?\n";
    _rst = "*RST\n";
    _clearError = "*CLS\n";
    _AUToscale = "AUToscale\n";
}

OscillometerTcp::OscillometerTcp(int port , string ip, string name) : TcpDevice(port, ip, name)
{
    _idn = "*IDN?\n";
    _rst = "*RST\n";
    _clearError = "*CLS\n";
    _AUToscale = "AUToscale\n";
}

bool OscillometerTcp::CanAcess()
{
    string result = WriteAndRead(_idn ,"Ascii",500);
//    cout<<"result = "<<result<<endl;
    if(!result.empty())
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}

bool OscillometerTcp::ClearError()
{
    return Write(_clearError,"Ascii");
}

bool OscillometerTcp::Reset()
{
    return Write(_rst,"Ascii");
}


bool OscillometerTcp::Init()
{
    if(ClearError())
        if(Reset())
            return true;
        else
            return false;
    else
        return false;
}

bool OscillometerTcp::Capture()
{
    string queryResult = "";
    /* Set probe attenuation factor. */
    do_command(":CHANnel1:PROBe 1.0");
    queryResult = do_query_string(":CHANnel1:PROBe?");
    cout<<"Channel 1 probe attenuation factor: "<< queryResult<<endl;

    /* Use auto-scale to automatically configure oscilloscope.
     * ------------------------------------------------------------- */
    do_command(":AUToscale");

    /* Set trigger mode. */
    do_command(":TRIGger:MODE EDGE");
    queryResult = do_query_string(":TRIGger:MODE?");
    cout<<"Trigger mode: "<< queryResult<<endl;

    /* Set EDGE trigger parameters. */
    do_command(":TRIGger:EDGE:SOURCe CHANnel1");
    queryResult = do_query_string(":TRIGger:EDGE:SOURce?");
    cout<<"Trigger edge source: "<<queryResult<<endl;

    do_command(":TRIGger:LEVel CHANnel1,-2E-3");
    queryResult = do_query_string(":TRIGger:LEVel? CHANnel1");
    cout<<"Trigger level, channel 1: "<<queryResult<<endl;

    do_command(":TRIGger:EDGE:SLOPe POSitive");
    queryResult = do_query_string(":TRIGger:EDGE:SLOPe?");
    cout<<"Trigger edge slope: "<< queryResult<<endl;

    /* Change settings with individual commands:
     * ------------------------------------------------------------- */

    /* Set vertical scale and offset. */
    do_command(":CHANnel1:SCALe 0.1");
    queryResult = do_query_string(":CHANnel1:SCALe?");
    cout<<"Channel 1 vertical scale: "<< queryResult<<endl;

    do_command(":CHANnel1:OFFSet 0.0");
    queryResult = do_query_string(":CHANnel1:OFFSet?");
    cout<<"Channel 1 offset: "<< queryResult<<endl;

    /* Set horizontal scale and position. */
    do_command(":TIMebase:SCALe 0.0002");
    queryResult = do_query_string(":TIMebase:SCALe?");
    cout<<"Timebase scale: "<< queryResult<<endl;

    do_command(":TIMebase:POSition 0.0");
    queryResult = do_query_string(":TIMebase:POSition?");
    cout<<"Timebase position: "<< queryResult<<endl;

    /* Set the acquisition mode. */
    do_command(":ACQuire:MODE RTIMe");
    queryResult = do_query_string(":ACQuire:MODE?");
    cout<<"Acquire mode: "<< queryResult<<endl;


    /* Set the desired number of waveform points,
     * and capture an acquisition. */
    do_command(":ACQuire:POINts 32000");
    do_command(":DIGitize");
}

bool OscillometerTcp::Analyze()
{
    double x_increment;
    double x_origin;
    double y_increment;
    double y_origin;

    string queryResult = "";

    /* Make measurements.
     * ------------------------------------------------------------- */
    do_command(":MEASure:SOURce CHANnel1");
    queryResult = do_query_string(":MEASure:SOURce?");
    cout<<"Measure source: "<<queryResult<<endl;

    do_command(":MEASure:FREQuency");
    queryResult = do_query_number(":MEASure:FREQuency?");
    cout<<"Frequency: "<<StringToDouble(queryResult) / 1000<< "kHz"<<endl;

    do_command(":MEASure:VAMPlitude");
    queryResult = do_query_number(":MEASure:VAMPlitude?");
    cout<<"Vertical amplitude: "<<StringToDouble(queryResult)<<"V"<<endl;

    /* Get the waveform type. */
    queryResult = do_query_string(":WAVeform:TYPE?");
    cout<<"Waveform type: "<<queryResult<<endl;

    /* Get the number of waveform points. */
    queryResult = do_query_string(":WAVeform:POINts?");
    cout<<"Waveform points: "<<queryResult<<endl;

    /* Set the waveform source. */
    do_command(":WAVeform:SOURce CHANnel1");
    queryResult = do_query_string(":WAVeform:SOURce?");
    cout<<"Waveform source: "<<queryResult<<endl;

    /* Choose the format of the data returned: */
    do_command(":WAVeform:FORMat WORD");
    queryResult = do_query_string(":WAVeform:FORMat?");
    cout<<"Waveform format: "<<queryResult<<endl;

    /* Display the waveform settings: */
    queryResult = do_query_number(":WAVeform:XINCrement?");
    x_increment = StringToDouble(queryResult);
    cout<<"Waveform X increment: "<< x_increment<<endl;

    queryResult = do_query_number(":WAVeform:XORigin?");
    x_origin = StringToDouble(queryResult);
    cout<<"Waveform X origin: "<<x_origin<<endl;

    queryResult = do_query_number(":WAVeform:YINCrement?");
    y_increment = StringToDouble(queryResult);
    cout<<"Waveform Y increment: "<<y_increment<<endl;

    queryResult = do_query_number(":WAVeform:YORigin?");
    y_origin = StringToDouble(queryResult);
    cout<<"Waveform Y origin: "<<y_origin<<endl;
}


void OscillometerTcp::do_command(char *command)
{
  char message[80];

  strcpy(message, command);
  strcat(message, "\n");
  Write(message,"Ascii");

  ReadErrors();
}

string OscillometerTcp::do_query_string(char *query)
{
  char message[80];

  strcpy(message, query);
  strcat(message, "\n");
  string ret = WriteAndRead(message,"Ascii");
  ReadErrors();
  return ret;
}

string OscillometerTcp::do_query_number(char *query)
{
  char message[80];

  strcpy(message, query);
  strcat(message, "\n");
  string ret = WriteAndRead(message,"Ascii");
  ReadErrors();
  return ret;
}

string OscillometerTcp::ReadErrors()
{
    string ret = WriteAndRead(":SYST:ERR?\n", "Ascii");
    if(ret.find("No error") != string::npos){
        return "";
    }

    if( ret.length() > 0){
        return ret;
    }else{
        cout<<"read err fail.............."<<endl;
        return "";
    }
}
