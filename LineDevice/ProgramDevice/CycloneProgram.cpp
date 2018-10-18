#include "CycloneProgram.h"
#include "./DataConvert/DataType.h"

string download = "0318413f";           //start program
string getwork= "03185f65";             //get workstation
string getver= "03010b24";              //get version
string clearer ="031866ca";             //clear error
string geterr = "03183366";             //get error
string getDFB2 = "08182E0000DFB204AA";  //get DFB2
string getD1F5 = "08182E0000D1F504B6";  //get D1F5
string getSapCount = "03181BBE";
string getIp = "03010607";

/**
 * @brief 通过串口参数以及设备名称构造
 * @param spp       包含串口参数的指针
 * @param name      设备名称
 */
CycloneProgram::CycloneProgram(SerialPortParameters *spp, string name):ComDevice(spp,name)
{
    d1f5Count=0;
    dfb2Count=0;
}

/**
 * @brief 测试函数
 */
void CycloneProgram::test()
{
    int ret;
    printf("--- ret = %d\n",ret);
    ret = IsInWorking();
    printf("=== ret = %d\n",ret);

    string buf;
    buf = WriteProgramAndGetId();
    printf("=== buf = %s \n",buf.data());
}

/**
 * @brief 发送并检查结果，会缩短等待返回数据的时间
 * @param command   指令内容
 * @param result    对比结果字符串
 * @param timeout   超时时间
 * @param crc8      校验码
 * @return
 */
bool CycloneProgram::SendAndCheck(string command, string result, unsigned long timeout,string crc8 = "")
{
    int ret = 0;
    int size = command.length();
    if(!crc8.empty())
    {
        command[size] = crc8[0];
        command[size+1] = crc8[1];
        command[size+2] = '\0';
        size = command.length();
    }
    char buff[size];
    memset(buff,0,sizeof(buff));
    int offset=0;
    struct timeval tv_start,tv_current;
    int total_time;
    unsigned char dec_cmd[size/2];
    memset(dec_cmd,0,sizeof(dec_cmd));
     if (timeout < 0)
         timeout = 5000000; //5000ms

    char* tmp_buff = (char *)malloc(size);

    int length = stringtodec(command,dec_cmd);
    if((ret = write(SerialPort_fd,dec_cmd,length)) < 0)
    {
        printf("write data error\n");
        return "";
    }
    gettimeofday(&tv_start,0);

    while(true)
    {
        usleep(50000);   //50ms

        gettimeofday(&tv_current,0);
        total_time = (tv_current.tv_sec - tv_start.tv_sec) * 1000000 ;
        total_time += (tv_current.tv_usec - tv_start.tv_usec);

        if((unsigned long)total_time > timeout)
        {
            printf("Time out \n");
            return false;
        }

        if((ret = read(SerialPort_fd,tmp_buff,1024))<=0)
        {
            printf("read data error\n");
            continue;
        }

        int i;
        for(i=0;i<ret;i++)
        {
            offset += sprintf(buff+offset,"%02X",tmp_buff[i]);
        }
        if(result.compare(buff)==0)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief 发送并检查结果
 * @param command   指令内容
 * @param result    对比结果字符串
 * @param timeout   超时时间
 * @return
 */
bool CycloneProgram::SendAndCheck_str(string command, string result,unsigned long timeout)
{
    if (timeout < 0) timeout = 5000;
    Write(command,"Hex");

    while (true)
    {
        usleep(50000);
        string hexString = Read("Hex");
        if (!hexString.empty())
        {
            return !StringToUpper(hexString).compare(result);
        }
    }
}

/**
 * @brief 获取Sap文件个数
 * @return
 */
int CycloneProgram::GetSapCount()
{
    int result = 0;
    string revice = WriteAndRead(getSapCount, "Hex",200);       //200ms
    if (revice.length() == 8 && revice.substr(0,2).compare("03") == 0 &&
            revice.substr(6,2).compare("EE"))
    {
        revice.substr(2,2);
        result = hextodec(revice.substr(2,2).data());
    }
    return result;
}

/**
 * @brief 获取设备IP
 * @return
 */
string CycloneProgram::GetDeviceIp()
{
    char result[16] = {0};
    string revice = WriteAndRead(getIp, "Hex",200);
    string m_buff;
    int IPdata[4] = {0};
    int j = 0;
    if (revice.length() == 10 && revice.substr(0,2).compare("04") == 0)
    {
        for(int i = 1;i < 5;i++)
        {
            m_buff = revice.substr(i*2, 2);
            //换算成IP格式
             IPdata[j++] = hextodec(m_buff);
        }
        sprintf(result,"%d.%d.%d.%d",IPdata[0],IPdata[1],IPdata[2],IPdata[3]);
    }
    string DeviceIP = result;
    return DeviceIP;
}

/**
 * @brief 添加Sap文件
 * @param 文件内容
 * @return
 */
bool CycloneProgram::AddSapFile(string fileContent)
{
    if (fileContent.empty())
        return 0;
    else
    {
        unsigned long timeout = 5000000;
        if (fileContent.empty()) return 0;
        fileContent = Trim(fileContent);
        if (fileContent.empty()) return 0;

        //添加sap文件：
        //步骤一（固定写法）
        if (!SendAndCheck("050223012229", "0101", timeout))
        {
            return false;
        }
        //步骤二（固定写法）
        if (!SendAndCheck("0502240101D6", "0101", timeout))
        {
            return false;
        }
        //步骤三（不固定，测试发现，添加第一个程序文件用"081806010000406A"以后都用"08180601000040FA"）
        if (!SendAndCheck("08180601000000406A", "01EE", timeout))
        {
            return false;
        }

        int mark = 0;
        int totalLen = 0;
        while (true)
        {
            totalLen = fileContent.length();

            if (totalLen <= 0) break;

            mark = (totalLen > 2048 ? 2048 : totalLen);
            string datas = fileContent.substr(0,mark);

            fileContent = fileContent.substr(mark);
            string cmdBuilder;

            cmdBuilder.append("1807");
            char byteArray[10] = {0};

            sprintf(byteArray,"%x",((datas.length() / 2) + 7));

            string lenStr = byteArray;//3个字节表示数据总长度

            lenStr = padZero(lenStr,6);//补零
            memset(byteArray,0,sizeof(byteArray));

            sprintf(byteArray,"%x",(datas.length() / 2));

            string coreDateLen = byteArray;
            coreDateLen = padZero(coreDateLen, 4);
            cmdBuilder.append(coreDateLen);
            cmdBuilder.append(datas);
            unsigned char array[10000]={0};
            int length=0;
            string crc8;
            length = stringtodec(cmdBuilder,array);
            crc8 = GetCrc8Check(0,7,array,length);
            cmdBuilder.insert(0, lenStr);
            if (!SendAndCheck(cmdBuilder, "03FFFFEE", timeout, crc8))
            {
                return false;
            }
        }
        //1
        if (!SendAndCheck("0502240103D8", "0101", timeout)) return false;
        //2
        if (!SendAndCheck("03180ED5", "03FFFFEE", timeout)) return false;
        //3
        if (!SendAndCheck("0502230100C7", "0101", timeout)) return false;//估计这是确认指令
        //4
        if (!SendAndCheck("050223002135", "0101", timeout)) return false;//添加过程结束
        //5
        if (!SendAndCheck("03180BCE", "030000EE", timeout)) return false;
        //6
        if (!SendAndCheck("03012CD1", "0400010000", timeout)) return false;
        //7
        if (!SendAndCheck("03181BBE", "030101EE", timeout)) return false;
        //8查错
        if (!SendAndCheck("03183366", "030000EE", timeout)) return false;
        //9查文件名称
        string revice = WriteAndRead("0418320127", "Hex",100);
        //10
        if (!SendAndCheck("04181E0175", "05F0110000EE", timeout))
        {
            return 0;
        }
        return SendAndCheck("03180CDB", "07FFFFF0120000EE", timeout);
    }
        return 0;
}

/**
 * @brief 通过文件路径添加Sap文件
 * @param 文件路径
 * @return
 */
bool CycloneProgram::AddSapFileByName(string filePath)
{
    FILE* fs = fopen(filePath.data(), "rb");
    if(fs == NULL)
    {
        perror("fopen:");
        return 0;
    }
    int res = 1;
    string sb;
    while(1)
    {
        char data[SIZE] = {0};
        res = fread(data,SIZE,sizeof(char),fs);
        sb.append(data);
        if(res < 0)
        {
            perror("fread error:\n");
            return 0;
        }
        else if(res == 0)
        {
            break;
        }
    }

    fclose(fs);
    return AddSapFile(sb);
}

/**
 * @brief 写入指定ID
 * @param model     模式
 * @param id        ID
 * @return
 */
bool CycloneProgram::SetDeviceId_str(string model, string id)
{
    string address;
    if(!model.compare("FS43X1"))
        address = "D1F5";
    else
        address = "DFB2";
    string command = "0B185D0000" + address + id;
    unsigned char array[7]={0};
    int length = 0;

    length =stringtodec(command,array);
    GetCrc8Check(0,7,array,length);
    return true;
}

/**
 * @brief 写入指定ID    id这里默认为1位  command数据长度为7位
 * @param model
 * @param id
 * @return
 */
bool CycloneProgram::SetDeviceId(string model, string id)
{
    char *address;
    string command = "0B185D0000";
    unsigned char array[7]={0};
    string hexstr;
    string Hex;
    int length = 0;

    address = (char *)malloc(10);
    if(model.compare("FS43X1")==0)
    {
        strcpy(address,"D1F5");
    }
    else
    {
        strcpy(address,"DFB2");
    }

    command.append(address);
    command.append(id);
    //第一字节不参与crc计算,去掉0b,保存到command
    string cmd = command.substr(2,command.length()-2);
    //将字符串转化成10进制数保存到arra数组中
    length =stringtodec(cmd,array);
    // crc8校验，返回16进制字符串
    hexstr = GetCrc8Check(0,7,array,length);
    cmd.append(hexstr);
    //16进制字符串转化成16进制字符数组
    stringtohex(cmd,Hex);

    if(!SendAndCheck(Hex,"01EE",1000000))
    {
        return false;
    }

    usleep(200000); //200ms

    return SendAndCheck("0502230100C7", "0101", 1000L);
}

/**
 * @brief 删除所有Sap文件
 * @return
 */
bool CycloneProgram::DeleteAllSap()
{
    long timeout = 2500000;     //2500ms
    if (!SendAndCheck("0818060100000000AD", "01EE", timeout))
    {
        return false;
    }
    if (!SendAndCheck("031808C7", "01EE", timeout))
    {
        return false;
    }
    return SendAndCheck("0502230100C7", "0101", timeout);
}

/**
 * @brief 清除错误信息
 * @return
 */
bool CycloneProgram::ClearError()
{
    int result;
    result = SendAndCheck(clearer,"030000EE",1000000);  //1000ms
    return result;
}

/**
 * @brief 烧程并写指定Id，必须是下面的顺序
 * @param model
 * @param id
 * @return
 */
bool CycloneProgram::WriteProgramAndSetId(string model, string id)
{
    bool isWorking = true;      //是否正在工作
    bool isTimeOut = false;     //是否超时
    int count = 0;              //检测是否正在工作的次数

     //1、先烧程
    if(!StartProgram())
        return false;
    //2、检查状态，等待烧程完成
    while (isWorking)
    {
        count++;
        isWorking = IsInWorking();
        printf("正在烧程中...\n");
        if (count > 10)
        {
            isTimeOut = true;
            isWorking = false;
        }
    }
    if(isTimeOut)
    {
         printf("烧程超时 !\n");
         return false;
    }
     printf("烧程结束！ \n");
     //3、写ID
     if (!SetDeviceId(model, id)) return false;       //xiaowei
     printf("写ID结束！ \n");
     usleep(100000);   //100ms
     //4 查错
     if (!SendAndCheck(geterr, "030000EE", 1000000))     //1000ms
     {
         printf("检查未通过！ \n");
         string lastError = GetLastError();
         printf("错误信息：%s \n",lastError.data());
         return false;
     }
     printf("检查通过，没有错误，烧程成功！\n");
     return true;//没有错误,烧程成功
}

/**
 * @brief CycloneProgram::GetIdOperator
 * @return
 */
string CycloneProgram::GetIdOperator()
{
    //经测试此处延迟最小70ms，设置为80ms
    int delay = 300;     //300ms
    string result;

    if(d1f5Count > dfb2Count)
    {
        result = GetId(delay,getD1F5);
        printf("getD1F5  result = %s\n",result.data());
        if(result.empty())
        {
            result = GetId(delay,getDFB2);
        }
    }
    else
    {
        result = GetId(delay,getDFB2);
        if(result.empty())
        {
            result = GetId(delay,getD1F5);
        }
    }

    return result;
}

/**
 * @brief 获取ID
 * @param delay     延时时间
 * @param addressString 字符串类型的地址
 * @return
 */
string CycloneProgram::GetId(int delay, string addressString)
{
    string result;
    string revice = WriteAndRead(addressString, "Hex",delay);
    if (revice.length() == 16 && revice.substr(0,2).compare("07") &&
            revice.substr(14,2).compare("EE"))
    {
        //substring(revice, result, 2, 8);
        result = revice.substr(2,8);
        //if (strstr(result,"FFFFFFFF") != NULL || strstr(result,"00000000") != NULL)
        if((!result.compare("FFFFFFFF"))&& (!result.compare("00000000")))
        {
            printf("取ID失败：%s\n",revice.data());
            //memset(result,0,sizeof(result));
            result="";
        }
        else
        {
            if(!addressString.compare("D1F5"))
                    d1f5Count++;
            else if (!addressString.compare("DFB2"))
                    dfb2Count++;
        }
    }
    else
        printf("取ID失败：%s\n",revice.data());

    return result;
}

/**
 * @brief 开始烧程
 * @return
 */
bool CycloneProgram::StartProgram()
{
    bool start;
    start = SendAndCheck_str(download,"01EE",1000000);  //1000ms
    return start;
}

/**
 * @brief 烧程后读ID
 * @return
 */
string CycloneProgram::WriteProgramAndGetId()
{
    bool isWorking = true;      //是否正在工作
    bool isTimeOut = false;     //是否超时
    int count = 0;              //检测是否正在工作的次数
    StartProgram();
    printf("烧程开始\n");
    while(isWorking)
    {
        count++;
        isWorking = IsInWorking();
        if(count % 3 == 0)
            printf("正在烧程中...");
        if (count > 30)
        {
            //此处设置烧成10秒超时，300ms检测一次
            isTimeOut = true;
            isWorking = false;
        }
    }
    if (isTimeOut)
    {
        printf("烧程超时");
        return GetIdOperator();
    }
    printf("烧程结束!\n");
    return  GetIdOperator();
}

/**
 * @brief 获取错误信息，没有错误信息返回空字符串
 * @return
 */
string CycloneProgram::GetLastError()
{
    //char *str;
    string str;
    str = WriteAndRead(geterr,"Hex",300);
    string error;
    error = str.substr(2,4);
    //if(error == "0000")
    if(!error.compare("0000"))
    {
        return "";
    }
    else
    {
        return error;
    }
}

/**
 * @brief 通信检测
 * @return
 */
bool CycloneProgram::CanAcess()
{
    string str;
    unsigned int j=0;
    m_communicateStatus = false;
    str = WriteAndRead(getver,"Hex",200);
    if((!str.empty())&&(str.length()>3))
    {
        while(j<str.length())
        {
            if((str[j] == '5')&&(str[j+1] == '0')&&(str[j+2] == '2')&&(str[j+3] == '6')&&(str[j+4] == '4')&&(str[j+5] == '5'))
            {
                m_communicateStatus = true;
            }
            j++;
        }
    }
    return m_communicateStatus;
}

/**
 * @brief 判断是否忙碌中
 * @return
 */
bool CycloneProgram::IsInWorking()
{
    string str;
    str = WriteAndRead(getwork,"Hex",300);
    if(!str.compare("030000EE"))
    {
        printf("Device Is In Working\n");
        return true;
    }    
    else if(!str.compare("030101EE"))
    {
        printf("Device Is Not In working\n");
        return false;
    }
    return false;
}


