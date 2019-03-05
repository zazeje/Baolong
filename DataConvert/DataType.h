#ifndef DATATYPE_H
#define DATATYPE_H

#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <QString>
#include <QList>
#include <math.h>
#include <iconv.h>
#include <algorithm>
#include <map>
#include <time.h>
#include <iostream>
#include <fstream>
// net add by wjl 2017.11.22
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include "../DataHelper/LogFile.h"
#include <QByteArray>
#include <QStringList>
#include <sstream>

using namespace std;

//16进制字符串转16进制字符数组
void stringtohex(string Buffer,string Hex);

//10进制转16进制
string dectoHex(int num);

//16进制转10进制（32位）
int hextodec(string s);

int hextodec(string s,int length);

//16进制转10进制（16位）
int hextodec16(string s);

//32位转16位整形
short int toInt16(int value);

vector<short int> toInt16Vec(vector<int> value);


//字符串转10进制
int stringtodec(string s, unsigned char array[]);

int stringtodec_char(string s, char array[]);

//字符串数组转double类型数组
void StringArrayToDoubleArray(vector<string> src, vector<double>& des);

//字符串转换成10进制double类型数
double StringToDouble(string src);

string GetCrc8Check(int initial, int generate, unsigned char arr[], int length);

string Trim(string m_str);

string padZero(string lenstr,int needTotalten);

/*Ascii字符串累和后转16进制，取末两位字符*/
string GetSumCheckByAsciiStr(string asciiString);

vector<int> HexStrToByteArr(string hexString);

void HexStrToByteArr(string hexString, unsigned char* byteArray);

string ByteArrToHexStr(unsigned char* byteArray,int length);

bool StringIsNullOrEmpty(char *);

void stringtohex_XX(string Buffer, string Hex[]);

/***将指定字符串转换为全小写的副本***/
string StringToLower(string s);

/***将指定字符串转换为全大写的副本***/
string StringToUpper(string s);

/***数字字符串小数位处理***/
string DigitChange(double num,int digit);

//add by vincent 2016-050-27
void split(const string& src, const string& separator, vector<double>& dest);

void split(const string& src, const string& separator, vector<int>& dest);

string ArrayJoint2String(const vector<string>& src ,const string& separator);

void splittostring(const string& src, const string& separator, vector<string>& dest);

void splittostring(const string& src, char separator, vector<string>& dest);

string TrimTo(string m_str);

string Replace(string src, string dst);

string ModeFind_string(vector<string> data);

//将int型转换成string类型
string IntToString(int num);

//将Double型转换成string类型
string DoubleToString(double num, string format = "%f");

//拆分好后将其放入链表中.根据输入的设备类型字符串，拆分成单独的，例如“PLC，扫码器”，拆分成“PLC”，“扫码器”；
QString splitstr(QString labelstr);

//去掉字符串中的序号(以**#开头)
string SubStrSerialNum(string);

//去掉字符串最后一位或两位数字
string SubStrLastNum(string str);

//将int数组转换成string类型
string intArrayToHexStr(int *decArray,int length);

//将vector类型的整形数组转化为string类型
string IntVecToStr(vector<int> src);

//将包含4个元素的int数组转换成
long IntToLong(int* b);

//将长整型数转换成包含4个元素的int数组
vector<int> longToInt(long number);

//十进制整型数转换为二进制字符串
string intToBinaryStr(int src);

//二进制字符串转换为十进制整型数
int binaryStrToInt(string src);

//二进制字符串转换为整型数组
vector<int> binaryStrToIntVec(string src);

//众数查找
int ModeFind(vector<int> data, int size);

string GetLocalTime();

string GetXorCheckByHexStr(string hexString);

string GetSumCheckByHexStr(string hexString);

//Lrc 校验
string GetLrcCheck(string hexString);

int code_convert(string from_charset, string to_charset, char* inbuf, size_t inlen, char* outbuf, size_t outlen);

int g2u(char* inbuf, size_t inlen, char* outbuf, size_t outlen);

int u2g(char* inbuf, size_t inlen, char* outbuf, size_t outlen);

template<typename T> void VectortoDouble(vector<T>vb, T buff[])
{
    unsigned int i;
    for(i = 0;i < vb.size();i++)
    {
        buff[i] = vb[i];
    }
}

template<typename T> T Average(vector<T> buff)
{
    T sum = 0;
    if(buff.size() > 0)
    {
        for(unsigned int i = 0;i < buff.size();i++)
        {
            sum = sum + buff[i];
        }
        sum = sum / buff.size();
    }
    return sum;
}

template<typename T> T Min(vector<T> buff)
{
    T min = 0;
    if(buff.size() > 0)
    {
        min = buff[0];
        for (unsigned int i = 1;i < buff.size();i++)
        {
          if(buff[i] < min)
          {
              min = buff[i];
          }
        }
    }
    return min;
}

template<typename T> T Max(vector<T> buff)
{
    T max = 0;
    if(buff.size() > 0)
    {
        max = buff[0];
        for (unsigned int i = 1;i < buff.size();i++)
        {
          if(max < buff[i])
          {
              max = buff[i];
          }
        }
    }
    return max;
}

//倒置字符串
string InsersionString(string str);

//获取IP地址函数 add by wjl
string getPBoxEthip(int nEth);

//设置IP地址函数 add by wjl
int setPBoxEthip(int nEth,string ip);

//判断字符串是否不为空并且为全数字字符函数 add by wjl
int isDigitStr(QString qstr);

//检查IP地址字符串是否合法函数 add by wjl,依赖isDigitStr函数;
int checkIpStr(QString qstr,string &outstr);

//16进行字符串转成2进行字符串
string HexToBin(const string &strHex);

//
string MemoryOutPut(const char * buf,size_t len);

#endif // DATATYPE_H


