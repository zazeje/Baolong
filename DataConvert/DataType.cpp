#include "DataType.h"

static int LogNum = 0;

//将int型转换成string类型
string IntToString(int num)
{
    string buff = "";
    char m_buff[100] = {0};
    sprintf(m_buff,"%d",num);
    buff = m_buff;
    return buff;
}

/**
 * <summary>将int数组转换成string类型
 * <param>第一个参数:int数组 第二个参数:数组长度
 * <return>string类型的16进制字符串
 * */
string intArrayToHexStr(int *decArray,int length)
{
    unsigned char array[1024] = {0};
    for(int i = 0;i < length;i++)
    {
        array[i] = decArray[i];
    }
    return ByteArrToHexStr(array,length);
}

string IntVecToStr(vector<int> src)
{
    char buffer[1024] = {0};
    if(!src.empty())
    {
        for(int i = 0,j = 0;i < src.size();i++,j++)
        {
            if(src[i] != 0)
            {
                buffer[j] = src[i];
            }
            else
            {
                j -= 1;
                continue;
            }
        }
    }
    return buffer;
}

/**
 * <summary>将包含4个元素的int数组转换成长整型数
 * <param>第一个参数:int数组
 * <return>长整型数
 * */
long IntToLong(int* b)
{
    long s0 = b[0] << 24;
    long s1 = b[1] << 16;
    long s2 = b[2] << 8;
    long s3 = b[3];
    long s = s0 | s1 | s2 | s3;
    return s;
}

/**
 * <summary>将长整型数转换成包含4个元素的int数组
 * <param>第一个参数:长整型数
 * <return>int数组
 * */
vector<int> longToInt(long number)
{
    long temp = number;
    long num = temp;
    vector<int> b(4,0);
    for (int i = 3; i >= 0; i--)
    {
        b[i] = temp & 0xFF;
        temp = temp >> 8;
        //将最低位保存在最低位
    }
        temp = temp >> 8;// 向右移8位
        if(num & 0x80000000)
            b[0] = b[0] - 256;
        if(num & 0x800000)
            b[1] = b[1] - 256;
        if(num & 0x8000)
            b[2] = b[2] - 256;
        if(num & 0x80)
            b[3] = b[3] - 256;
    return b;
}

/**
 * <summary> 十进制整型数转换为二进制字符串
 * <param> 第一个参数:进行转换的整型数
 * <return> 二进制字符串
 * */
string intToBinaryStr(int src)
{
    string desc = "";
    for(int i = 0;src != 0;i++)
    {
        desc.append(src % 2 ? "1" : "0");
        src = src / 2;
    }
    reverse(desc.begin(),desc.end());
    return desc;
}

/**
 * <summary> 二进制字符串转换为十进制整型数
 * <param> 第一个参数:进行转换的字符串
 * <return> 十进制整型数
 * */
int binaryStrToInt(string src)
{
    int desc = 0;
    int flag = 0;
    for(string::reverse_iterator i = src.rbegin();i != src.rend();i++)
    {
        int tmp = 0;
        (*i) == '1' ? tmp = pow(2,flag) : tmp = 0;
        desc = desc + tmp;
        flag++;
    }
    return desc;
}

/**
 * @brief 二进制字符串转换为整型数组
 * @param 进行转换的字符串
 * @return
 */
vector<int> binaryStrToIntVec(string src)
{
    vector<int> dst;
    if(!src.empty())
    {
        for(int i = 0;i < src.length();i++)
        {
            if(src[i] == '1')
                dst.push_back(1);
            else
                dst.push_back(0);
        }
    }
    return dst;
}

/**
 * @brief 众数查找(整型数)
 * @param 整型数组
 * @param 数组包含元素个数
 * @return 返回出现次数最多的整数
 */
int ModeFind(vector<int> data,int size)
{
    map<int,int>result;
    for(int i = 0;i < size;i++)
    {
        map<int,int>::iterator im = result.find(data[i]);
        if(im == result.end())
        {
            result.insert(pair<int,int>(data[i],0));
        }
        else if(im != result.end())
        {
            im->second++;
        }
    }
    map<int,int>tmp = result;
    map<int,int>::iterator is;
    for(is = result.begin();is != result.end();is++)
    {
        map<int,int>::iterator it;
        for(it = tmp.begin();it != tmp.end();it++)
        {
            if(is->first == it->first)
            {
                continue;
            }
            else if(is->second < it->second)
            {
                result.erase(is);
                break;
            }
            else if(is->second >= it->second)
            {
                if(result.find(it->first) != result.end())
                    result.erase(result.find(it->first));
                else
                    break;
            }
        }
    }
    return result.begin()->first;
}

/**
 * @brief 众数查找(字符串)
 * @param 字符串数组
 * @return 返回出现次数最多的字符串
 */
string ModeFind_string(vector<string>data)
{
    map<string,int>result;

    for(unsigned int i = 0;i < data.size();i++)
    {
        map<string,int>::iterator im = result.find(data[i]);

        if(data[i].data() != NULL && im == result.end())
        {
            result.insert(pair<string,int>(data[i],0));
        }
        else if(data[i].data() != NULL && im != result.end())
        {
            im->second++;
        }
    }
    map<string,int>tmp = result;
    map<string,int>::iterator is;
    for(is = result.begin();is != result.end();is++)
    {
        map<string,int>::iterator it;
        for(it = tmp.begin();it != tmp.end();it++)
        {
            if(is->first == it->first)
            {
                continue;
            }
            else if(is->second < it->second)
            {
                result.erase(is);
                break;
            }
            else if(is->second >= it->second)
            {
                if(result.find(it->first) != result.end())
                    result.erase(result.find(it->first));
                else
                    break;
            }
        }
    }
    return result.begin()->first;
}

/**
 * @brief 浮点数转字符串
 * @param num
 * @param 格式类型
 * @return
 */
string DoubleToString(double num,string format)
{
    string buff = "";
    char m_buff[100] = {0};
    sprintf(m_buff,format.data(),num);
    buff = m_buff;
    return buff;
}

/**
 * @brief 按指定字符（串）去分割源字符串
 * @param 源字符串
 * @param 指定字符
 * @param 将分割后的内容转化为double类型数组
 */
void split(const string& src, const string& separator, vector<double>& dest)
{
    string str = src;
    string substring;
    string::size_type start = 0, index;
    do
    {
        index = str.find_first_of(separator,start);
        if (index != string::npos)
        {
            substring = str.substr(start,index-start);
            dest.push_back(atof(substring.data()));
            start = str.find_first_not_of(separator,index);
            if (start == string::npos) return;
        }
    }while(index != string::npos);
    substring = str.substr(start);
    dest.push_back(atof(substring.data()));
}

/**
 * @brief 按指定字符（串）去分割源字符串
 * @param 源字符串
 * @param 指定字符
 * @param 将分割后的内容转化为int类型数组
 */
void split(const string& src, const string& separator, vector<int>& dest)
{
    string str = src;
    string substring;
    string::size_type start = 0, index;
    do
    {
        index = str.find_first_of(separator,start);
        if (index != string::npos)
        {
            substring = str.substr(start,index-start);
            dest.push_back(atoi(substring.data()));
            start = str.find_first_not_of(separator,index);
            if (start == string::npos) return;
        }
    }while(index != string::npos);
    substring = str.substr(start);
    dest.push_back(atoi(substring.data()));
}

/**
 * @brief 按指定字符把字符串数组拼接成单个字符串
 * @param 字符串数组
 * @param 分隔字符
 * @return  按格式拼接好的字符串
 */
string ArrayJoint2String(const vector<string>& src ,const string& separator)
{
    string data = "";
    if(!src.empty())
    {
        for(int i = 0;i < src.size();i++)
        {
            data.append(src[i]);
            if(i != src.size() - 1)
                data.append(separator);
        }
    }
    return data;
}

/**
 * @brief 按指定字符（串）去分割源字符串
 * @param 源字符串
 * @param 指定字符
 * @param 将分割后的内容转化为string类型数组
 */
void splittostring(const string& src, const string& separator, vector<string>& dest)
{
    string str = src;
    string substring;
    string::size_type start = 0, index;

    do
    {
        index = str.find_first_of(separator,start);
        if (index != string::npos)
        {
            substring = str.substr(start,index-start);
            dest.push_back((substring.data()));
            start = str.find_first_not_of(separator,index);
            if (start == string::npos) return;
        }
    }while(index != string::npos);

    substring = str.substr(start);
    dest.push_back((substring.data()));
}

/**
 * @brief 按指定字符去分割源字符串
 * @param 源字符串
 * @param 指定字符
 * @param 将分割后的内容转化为string类型数组
 */
void splittostring(const string& src, char separator, vector<string>& dest)
{
    int index = 0;
    if(!src.empty())
    {
        for(int i = 0;i < src.length();i++)
        {
            if(src.c_str()[i] == separator)
            {
                dest.push_back(src.substr(index,i - index));
                index = i + 1;
                continue;
            }
            if(i == src.length() - 1)
                dest.push_back(src.substr(index,i - index + 1));
        }
    }
}

/**
 * @brief 16进制字符串转比特数组
 */
vector<int> HexStrToByteArr(string hexString)
{
    vector<int> byteArray;
     int len = hexString.length() / 2;
     int j = 0;
     for (int i = 0; i < len; i++)
     {
         int sum = hextodec((&(hexString[j])),1)*16 + hextodec((&(hexString[++j])),1);
         byteArray.push_back(sum);
         j += 1;
     }
     return byteArray;
}

/**
 * @brief 16进制字符串转比特数组
 * @param
 * @param 接收数组
 */
void HexStrToByteArr(string hexString,unsigned char* byteArray)
{
     int len = hexString.length() / 2;
     int j = 0;
     for (int i = 0; i < len; i++)
     {
         byteArray[i] = hextodec((&(hexString[j])),1)*16 + hextodec((&(hexString[++j])),1);
         j += 1;
     }
}

/**
 * @brief 比特数组转16进制字符串
 * @param 比特数组
 * @param 数组长度
 * @return 转化后的字符串
 */
string ByteArrToHexStr(unsigned char* byteArray,int length)
{
     string hexString;
     char swap[2] = {0};
     if (byteArray != NULL)
     {
         for (int i = 0; i < length; i++)
         {
             memset(swap,0,sizeof(swap));
             sprintf(swap,"%02X",byteArray[i]);
             hexString.append(swap);
         }
     }
     return hexString;
}

/**
 * @brief stringtohex
 * @param Buffer
 * @param Hex
 */
void stringtohex(string Buffer,string Hex)
{
    int i,j,xh;
    for(j=i=0;Buffer[i];i++)
    {
        Hex[j]=((xh=Buffer[i++])>'9' ? xh>='a' ? xh-0x57 : xh-0x37 :  xh-'0')<<4;
        Hex[j++] |= (xh=Buffer[i])>'9' ? xh>='a' ? xh-0x57 : xh-0x37 :  xh-'0';
    }
}

/**
 * @brief 16进制数(字符串)转10进制整型数
 * @param s
 * @param length
 * @return
 */
int hextodec(string s,int length)
{
    int i = 0,t = 0;
    long sum = 0;
    if(length > 1)
    {
        for(i=0;i < length;i++)
        {
            if(s[i]<='9')
                t=s[i]-'0';
            else
            {
                if('A' <= s[i] && s[i] <= 'Z')
                        t=s[i]-'A'+10;
                else if('a' <= s[i] && s[i] <= 'z')
                        t = s[i] - 'a' + 10;
            }
            sum=sum*16+t;
        }
    }
    else
    {
        if(s[0] <= '9')
            t = s[0] - '0';
        else
        {
            if('A' <= s[0] && s[0] <= 'Z')
                    t=s[0]-'A'+10;
            else if('a' <= s[0] && s[0] <= 'z')
                    t = s[0] - 'a' + 10;
        }
        sum = t;
    }
        return sum;
}

/**
 * @brief dectoHex 10进制整数转16进制
 * @param num
 * @return
 */
string dectoHex(int num)
{
    string buff;
    char tmp[1024] = {0};
    sprintf(tmp, "%02X", num);
    buff = tmp;
    return buff;
}


/**
 * @brief 16进制数(字符串)转10进制整型数（32位）
 * @param s
 * @return
 */
int hextodec(string s)
{
    int i,t=0;
    long sum=0;
    for(i=0;s[i];i++)
    {
        if(s[i] <= '9' && s[i] >= '0')
            t=s[i]-'0';
        else
        {
            if('A' <= s[i] && s[i] <= 'Z')
                    t=s[i]-'A'+10;
            else if('a' <= s[i] && s[i] <= 'z')
                    t = s[i] - 'a' + 10;
        }
        sum=sum*16+t;
    }
    return sum;
}

/**
 * @brief hextodec16    16进制数(字符串)转10进制整型数(16位)
 * @param s
 * @return
 */
int hextodec16(string s)
{
    int i;
    short int t=0;
    short int sum=0;
    for(i=0;s[i];i++)
    {
        if(s[i] <= '9' && s[i] >= '0')
            t=s[i]-'0';
        else
        {
            if('A' <= s[i] && s[i] <= 'Z')
                    t=s[i]-'A'+10;
            else if('a' <= s[i] && s[i] <= 'z')
                    t = s[i] - 'a' + 10;
        }
        sum=sum*16+t;
    }
    return sum;
}

short int toInt16(int value)
{
    short int sum = 0;
    sum = value;
    return sum;
}

vector<short int> toInt16Vec(vector<int> value)
{
    vector<short int> sum(value.size());
    for(int i=0; i<value.size();i++)
    {
        sum[i] = value[i];
    }
    return sum;
}

/**
 * @brief 16进制字符串转10进制字符数组
 * @param s
 * @param array
 * @return 返回数组长度
 */
int stringtodec(string s, unsigned char array[])     //length为array[]数组的长度
{
    unsigned int i = 0;
    int length = 0;
    for(i = 0;i < s.length() / 2;i++)
    {
        array[i] = hextodec(s.substr(i*2,2));
    }
    length = i;
    return length;
}

#if 0
int stringtodec_char(string s,char array[])     //length为array[]数组的长度
{
    unsigned int i = 0;
    int length=0;
    for(i=0;i<s.length()/2;i++)
    {
        array[i] = hextodec(s.substr(i*2,2));
    }
    length = i;
    return length;
}
#endif

/**
 * @brief 数字字符串(包含小数点)转double类型数
 * @param 源字符串
 * @return double类型数
 */
double StringToDouble(string src)
{
    return atof(src.data());
}

/**
 * @brief 字符串数组转double类型数组
 * @param src
 * @param des
 */
void StringArrayToDoubleArray(vector<string> src,vector<double>& des)
{
    for(unsigned int i = 0;i < src.size();i++)
    {
        string str = src[i];
        for(unsigned int t = 0;t < str.length();t++)
        {
            if(isdigit(str[t]))
                continue;
            else
                str.erase(t,1);
        }
    }
    for(unsigned int i = 0;i < src.size();i++)
    {
        des.push_back(atof(src[i].data()));
    }
}

/**
 * @brief Crc8校验
 * @param initial
 * @param generate
 * @param arr
 * @param length
 * @return
 */
string GetCrc8Check(int initial, int generate, unsigned char arr[], int  length)
{
    int crc8 = initial & 0xFF;
    int poly = generate & 0xFF;
    char *hexstring;

    hexstring = (char *)malloc(sizeof(char));
    //array的数据长度为7位 4(command) + 2(address) +1(id)
    for (int i = 0; i < length; i++)
    {
        crc8 ^= (arr[i] & 0xFF);
        for (int j = 0; j < 8; j++)
        {
            if ((crc8 & 0x80) != 0)
            {
                crc8 = ((crc8 << 1) & 0xFF) ^ poly;
            }
            else
            {
                crc8 = ((crc8 << 1) & 0xFF);
            }
        }
    }
    sprintf(hexstring,"%02X",crc8);
    string HexString = hexstring;
    return HexString;
}

/**
 * @brief 将字符串中的换行符，空格去掉
 * @param 需要进行处理的字符串
 * @return 处理好的字符串
 */
string Trim(string m_str)
{
    for(unsigned int i = 0;i < m_str.length();++i)
    {
        if((m_str[i] == '\r') || (m_str[i] == '\n'))
        {
            m_str = m_str.substr(0,i);      // 去除结果字符串中的换行符且把后面的内容舍弃掉
            break;
        }
        else if( m_str[i] == ' ')
        {
             m_str.erase(i,1);     // 空格为1个字符
             i -= 1;
        }
    }
    return m_str;
}

/**
 * @brief 将字符串中的换行符替换为逗号
 * @param 需要进行处理的字符串
 * @return 处理后的字符串
 */
string TrimTo(string m_str)
{
    for(unsigned int i = 0;i < m_str.length();++i)
    {
        if((m_str[i] == '\r') || (m_str[i] == '\n'))
        {
            m_str.replace(i,1,",");      // "\r\n"为2个字符
        }

    }
    return m_str;
}



string Replace(string src, string dst)
{
    for(int i=0;i<src.size();i++)
    {
        if(src[i ] == '\r' )                 //换行符\r\n，占两个字符
        {
            src.replace(i,2,dst);
        }
        else if(src[i] == ' ')
        {
            src.replace(i,1,dst);
        }
    }
    return src;
}


/**
 * @brief 字符串补零
 * @param 源字符串
 * @param 需要补零后的字符串长度
 * @return 补零后的字符串
 */
string padZero(string lenstr, int needTotalten)
{
    int padLen = needTotalten - lenstr.length();
    string sb = "";
    for (int i = 0; i < padLen; i++)
    {
        sb.append("0");
    }
    return sb + lenstr;

}

/**
 * @brief ascii码字符串和校验
 * @param asciiString
 * @return
 */
string GetSumCheckByAsciiStr(string asciiString)
{
    int sum = 0;
    string sumHexString;
    for (unsigned int i = 0; i < asciiString.length(); i += 1)
    {
        sum = sum + asciiString.data()[i];
    }
    char m_sum[10];
    sprintf(m_sum,"%X",sum);
    sumHexString = m_sum;
    return sumHexString.substr(sumHexString.length()- 2);
}

/**
 * @brief 字符串转全小写
 */
string StringToLower(string s)
{
    for(unsigned int i = 0;i < s.length();i++)
     {
         if('A' <= s[i] && s[i] <= 'Z')
         {
             s[i] = s[i] + 32;
         }
         else
             continue;
     }
    return s;
}

/**
 * @brief 字符串转全大写
 */
string StringToUpper(string s)
{
    for(unsigned int i = 0;i < s.length();i++)
     {
         if('a' <= s[i] && s[i] <= 'z')
         {
             s[i] = s[i] - 32;
         }
         else
             continue;
     }
    return s;
}

/**
 * @brief 浮点数小数位处理
 * @param num
 * @param 需要保留的小数位数
 * @return 返回处理后的数字字符串
 */
string DigitChange(double num,int digit)
{
    string str;
    string format;
    char tmp[100] = {0};
    format = "%." + IntToString(digit) + "f";
    memset(tmp,0,100);
    sprintf((char*)tmp, format.data(), num);
    str = tmp;
    return str;
}

/**
 * @brief 字符串拆分
 * @param labelstr
 * @return
 */
QString splitstr(QString labelstr)
{
    string data = labelstr.toStdString();
    QString Data;
    for(unsigned int i = 0;i < data.length();i++)
    {
        if(data[i] == ' ' && data[i+1] == ' ')
        {
            Data = QString::fromStdString(data.substr(0,i));
            break;
        }
    }
    return Data;
}

/**
 * @brief 去掉字符串中最后两位数字
 */
string SubStrLastNum(string str)
{
    string name;
    if(str.substr(str.length() - 2,2)[0] < '0' || str.substr(str.length() - 2,2)[0] > '9')
    {
        if(str.substr(str.length() - 2,2)[1] < '0' || str.substr(str.length() - 2,2)[1] > '9')
        {
            name = str;
        }
        else
            name = str.substr(0,str.length() - 1);
    }
    else
        name = str.substr(0,str.length() - 2);

    return name;
}

/**
 * @brief 去掉字符串中的序号(以**#开头)
 */
string SubStrSerialNum(string str)
{

    if(str.find("#") != string::npos)
    {
        return str.substr(str.find("#") + 1);
    }
}

/**
 * @brief xor校验
 * @param 校验前字符串
 * @return 校验后的字符串
 */
string GetXorCheckByHexStr(string hexString)
{
    int Xor = 0, temp = 0;
    string hexStr = "";
    for (unsigned int i = 0; i < hexString.length(); i += 2)
    {
        temp = hextodec(hexString.substr(i,2));
        Xor = Xor ^ temp;
    }
    char tmp[256] = {0};
    sprintf(tmp,"%X",Xor);
    hexStr = tmp;
    if (hexStr.length() == 1)  hexStr = "0" + hexStr;
    if (hexStr.length() > 2) hexStr = hexStr.substr(hexStr.length() - 2);
    return hexStr;
}

/**
 * @brief GetSumCheckByHexStr               十六进制字符串转10进制累和后转16进制，取末两位字符
 * @param hexString                         十六进制字符串
 * @return
 */
string GetSumCheckByHexStr(string hexString)
{
    int sum = 0;
    string sumHexString = "";
    for (int i = 0; i <= hexString.length() - 2; i += 2)
    {
        sum = sum + hextodec(hexString.substr(i,2));
    }
    char buff[10] = {0};
    sprintf(buff,"%X",sum);
    sumHexString = buff;
    return sumHexString.substr(sumHexString.length() - 2);
}

/**
 * @brief GetLrcCheck                       Lrc校验
 * @param hexString
 * @return
 */
string GetLrcCheck(string hexString)
{
    int temp = 0;
    string hexStr = "";
    for (int i = 0; i < hexString.length(); i += 2)
    {
        temp = temp + hextodec(hexString.substr(i, 2));
    }
    temp = ~temp + 1;
    char tmp[10] = {0};
    sprintf(tmp,"%X",temp);
    hexStr = tmp;
    if (hexStr.length() == 1) hexStr = "0" + hexStr;
    if (hexStr.length() > 2) hexStr = hexStr.substr(hexStr.length() - 2);
    return hexStr;
}

/**
 * @brief InsersionString 倒置字符串
 * @param str
 * @return  10101000
 *          00010101
 */
string InsersionString(string str)
{
    string values;
    string tmp;

    for(int i =0; i< str.length(); i++)
    {
        tmp = str.data()[str.length() -i - 1];
        values.append(tmp);
    }
    return values;
}


int code_convert(string from_charset, string to_charset,
        char* inbuf, size_t inlen, char* outbuf, size_t outlen)
{
    iconv_t cd;
    char **pin = &inbuf;
    char **pout = &outbuf;

    cd = iconv_open(to_charset.data(), from_charset.data());
    if (cd<0)
    {
        perror("iconv_open:");
        return -1;
    }

    memset(outbuf,0,outlen);
    if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
    {
        printf("errno=%d\n", errno);
        perror("iconv failed:\n");
        iconv_close(cd);
        return -1;
    }

    iconv_close(cd);
    return 0;
}
/**
 * @brief 将gb2312编码转utf8编码
 * @param inbuf
 * @param inlen
 * @param outbuf
 * @param outlen
 * @return
 */
int g2u(char* inbuf, size_t inlen, char* outbuf, size_t outlen)
{
    return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
}

/**
 * @brief 将utf8编码转gb2312编码
 * @param inbuf
 * @param inlen
 * @param outbuf
 * @param outlen
 * @return
 */
 int u2g(char* inbuf, size_t inlen, char* outbuf, size_t outlen)
{
    return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);
}

 /**
  * @brief getPBoxEthip
  * @param nEth,指定网卡好，如eth0,对应参数0;
  * @return
  * add by wjl 2017/11/22
  */
 string getPBoxEthip(int nEth)
 {
     int sock_get_ip;
     char ipaddr[50];
     struct   sockaddr_in *sin;
     struct   ifreq ifr_ip;
     memset(&ifr_ip, 0, sizeof(ifr_ip));
     if(nEth == 0)
     {
         strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);
     }
     else if(nEth == 1)
     {
         strncpy(ifr_ip.ifr_name, "eth1", sizeof(ifr_ip.ifr_name) - 1);
     }
     else
     {
         _log.LOG_ERROR("nEth参数错误，指定的要获取IP地址的网卡：【%d】不存在！",nEth);
//         cout<<"no eth:"<<nEth<<endl;
         return "";
     }
     if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)
     {
//         cout<<"socket0 create failse...GetLocalIp!"<<endl;
         _log.LOG_ERROR("创建获取IP地址的socket失败！");
         return "";
     }


     if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )
     {
         close( sock_get_ip );
         _log.LOG_ERROR("调用驱动函数要获取网卡：【%d】的IP地址的不成功！",nEth);
         return "";
     }
     sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
     strcpy(ipaddr,inet_ntoa(sin->sin_addr));
     close( sock_get_ip );
//     _log.LOG_INFO("获取到网卡：【%d】的IP地址：【%s】！",nEth,ipaddr);
//     cout<<"local ip:"<<ipaddr<<endl;

     return string( ipaddr );
 }

 /**
  * @brief setPBoxEthip
  * @param nEth,指定网卡号，如eth0,对应参数0;
  * @param ip,指定网卡要设置的IP地址，如192.168.20.120;
  * 调用本参数前先使用checkIpStr函数校验ip字符串是否合法，合法则才能调用本函数
  * @return 0：设置成功，返回非0设置失败。
  * add by wjl 2017/11/24
  */
 int setPBoxEthip(int nEth,string ip)
 {
     int sock_set_ip,ret;
     struct   sockaddr_in sin;
     struct   ifreq ifr_ip;
     memset(&ifr_ip, 0, sizeof(ifr_ip));
     if(nEth == 0)
     {
         strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);
     }
     else if(nEth == 1)
     {
         strncpy(ifr_ip.ifr_name, "eth1", sizeof(ifr_ip.ifr_name) - 1);
     }
     else
     {
         _log.LOG_ERROR("nEth参数错误，指定的要获取IP地址的网卡：【%d】不存在！",nEth);
//         cout<<"no eth:"<<nEth<<endl;
         return -3;
     }

     memset(&sin, 0, sizeof(sin));
     sin.sin_family = AF_INET;
     inet_pton(AF_INET, ip.data(), &(sin.sin_addr));
     memcpy(&ifr_ip.ifr_addr, &sin, sizeof(struct sockaddr));

     if ((sock_set_ip = socket(AF_INET, SOCK_STREAM, 0)) == -1)
     {
//         socket(AF_INET, SOCK_DGRAM, 0);
//         cout<<"socket0 create failse...GetLocalIp!"<<endl;
         _log.LOG_ERROR("创建获取IP地址的socket失败！");
         return -2;
     }

//     errno = saved_errno;
     ret = ioctl(sock_set_ip, SIOCSIFADDR, &ifr_ip);
     if(ret < 0 )
     {
//         if(errno == 19)
//         {
//             fprintf(stderr, "Interface %s : No such device.\n", dev);
//         }
//         else if(errno == 99)
//         {
//             fprintf(stderr, "Interface %s : No IPv4 address assigned.\n", dev);
//         }
         close(sock_set_ip);
         _log.LOG_ERROR("调用驱动函数要设置网卡：【%d】的IP地址的不成功！",nEth);
         return -1;
     }
     close(sock_set_ip);
//     _log.LOG_INFO("设置IP地址：【%s】到网卡：【%d】成功！",ipaddr,nEth);
     return 0;
 }
#if 1
 /**
  * @brief isDigitStr
  * @param src,指定要判断的QString类型字符串;
  * @return 是纯数字返回1，非纯数字返回-1,为空返回0;
  * add by wjl 2017/11/22
  */
 int isDigitStr(QString qstr)
 {
     if(qstr.size() == 0)
     {
         return 0;
     }
     QByteArray ba = qstr.toLatin1();
     const char *s = ba.data();
     while(*s && *s>='0' && *s<='9') s++;

     if (*s)
     {
         return -1;
     }
     else
     {
         return 1;
     }
 }
#endif
 /**
  * @brief checkIpStr
  * @param 【in】 qinstr,指定要判断的QString类型字符串;
  * @param 【out】 outstr,当IP字符串合法时，输出去掉多余前缀0的string类型字符串;譬如192.168.001.012转换为192.168.1.12;
  * @return 合法C类地址返回1，并且在string字符串输出参数中返回去除0前缀的IP地址;
  * 数据格式错误返回-1,IP地址范围（C类地址：192.0.0.0-223.255.255.255）不合法返回0，返回空字串;
  * add by wjl 2017/11/22
  */
 int checkIpStr(QString qstr,string &outstr)
 {
     outstr = "";
     QStringList iplist = qstr.split(".", QString::SkipEmptyParts);
     if(iplist.size() != 4)
     {
         return -1;
     }
     foreach(QString s, iplist)
     {
         if(isDigitStr(s) <= 0)
         {
             return -1;
         }
         if(s.size() > 3)
         {
             return -1;
         }
     }
     bool ok = false;
     int ntmp = iplist[0].toInt(&ok,10);
     if(!ok)
     {
        return -1;
     }
     else
     {
//        if((ntmp < 0) || (ntmp > 255))
//        {
//            return -1;
//        }
//        else
//        {
//            if((ntmp < 192) || (ntmp > 223))
//            {
//                return 0;
//            }
//        }
         if(ntmp != 192)
         {
             return -1;
         }
        stringstream ss;
        ss<<ntmp;
        outstr = ss.str();
        outstr += ".";
     }
     ntmp = iplist[1].toInt(&ok,10);
     if(!ok)
     {
        outstr = "";
        return -1;
     }
     else
     {
         if(ntmp != 168)
         {
             outstr = "";
             return -1;
         }
         stringstream ss;
         ss<<ntmp;
         outstr += ss.str();
         outstr += ".";
     }
     ntmp = iplist[2].toInt(&ok,10);
     if(!ok)
     {
        outstr = "";
        return -1;
     }
     else
     {
         if((ntmp < 0) || (ntmp > 255))
         {
             outstr = "";
             return -1;
         }
         stringstream ss;
         ss<<ntmp;
         outstr += ss.str();
         outstr += ".";
     }
     ntmp = iplist[3].toInt(&ok,10);
     if(!ok)
     {
        outstr = "";
        return -1;
     }
     else
     {
         if((ntmp <= 0) || (ntmp >= 255))
         {
             outstr = "";
             return -1;
         }
         stringstream ss;
         ss<<ntmp;
         outstr += ss.str();
     }
     return 1;
 }

 string HexToBin(const string &strHex)
 {
     if (strHex.size() % 2 != 0)
     {
         return "";
     }

     string strBin = "";
     for (size_t i = 0; i < strHex.size(); i++)
     {
         string temp = "";
         switch (strHex[i]){
         case '0':
             temp = "0000";
             break;
         case '1':
             temp = "0001";
             break;
         case '2':
             temp = "0010";
             break;
         case '3':
             temp = "0011";
             break;
         case '4':
             temp = "0100";
             break;
         case '5':
             temp = "0101";
             break;
         case '6':
             temp = "0110";
             break;
         case '7':
             temp = "0111";
             break;
         case '8':
             temp = "1000";
             break;
         case '9':
             temp = "1001";
             break;
         case 'a': case 'A':
             temp = "1010";
             break;
         case 'b': case 'B':
             temp = "1011";
             break;
         case 'c': case 'C':
             temp = "1100";
             break;
         case 'd': case 'D':
             temp = "1101";
             break;
         case 'e': case 'E':
             temp = "1110";
             break;
         case 'f': case 'F':
             temp = "1111";
             break;
         }

         strBin += temp;
//         if(i>0 && i % 2 == 1)
//             strBin += " ";
     }

     return strBin;
 }

string MemoryOutPut(const char* buf, size_t len)
 {
    string ret = "";
    char a[3];
    a[0] = 0;
    a[1] = 0;
    a[2] = 0;
     for(size_t i=0; i<len; ++i)
     {
         unsigned char c = buf[i]; // must use unsigned char to print >128 value
         if( c< 16)
         {
             sprintf(a,"0%x", c);
         }
         else
         {
             sprintf(a,"%x", c);
         }
         ret += string((const char *)a);
     }
    return ret;
}
