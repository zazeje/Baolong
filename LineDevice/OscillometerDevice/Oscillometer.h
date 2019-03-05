#ifndef OSCILLOMETER_H
#define OSCILLOMETER_H


#include "./LineDevice/Device/TcpDevice.h"
#include "./DataConvert/DataType.h"
#include <stdlib.h>

class OscillometerTcp : public TcpDevice
{
    //示波器
    //作者 上海公司/张政
    //时间 2018-12-25
public:

    OscillometerTcp(int port , string ip);      //构造函数
    OscillometerTcp(int port , string ip, string name);

    bool CanAcess();
    bool ClearError();
    bool Capture();
    bool Reset();
    bool Init();
    bool Analyze();
    string ReadErrors();


    void initialize(void);          /* Initialize to known state. */
    void capture(void);             /* Capture the waveform. */
    void analyze(void);             /* Analyze the captured waveform. */

    void do_command(char *command);        /* Send command. */
    int do_command_ieeeblock(char *command); /* Command w/IEEE block. */
    string do_query_string(char *query);     /* Query for string. */
    string do_query_number(char *query);     /* Query for number. */
    void do_query_numbers(char *query);    /* Query for numbers. */
    int do_query_ieeeblock(char *query);   /* Query for IEEE block. */
    int do_query_ieeeblock_words(char *query);   /* Query for word data. */
    void check_instrument_errors();        /* Check for inst errors. */

private:

    string _idn;
    string _rst;
    string _clearError;
    string _AUToscale;

};

#endif // OSCILLOMETER_H
