#include "CycloneProUdp.h"

#define SIZE 1024*1024

string _getversion="0003C14E010B";
string _download[2]={"0003AE8A181C01","0003AE8B1841"};
string _isworking="000326C9185F";
string _error="000326CA1833";
string _address_DFB2="DFB2";
string _address_D1F5="D1F5";

int interval = 60*1000;

/**
 * @brief 无参构造
 */
CycloneProUdp::CycloneProUdp()
{
    _commandNo = 600;
    _d1f5Count = 0;
    _dfb2Count = 0;
    m_isProgramming = false;
}

/**
 * @brief 通过端口号、ip地址执行构造
 * @param port  端口号
 * @param ip    IP地址
 */
CycloneProUdp::CycloneProUdp(int port, string ip):UdpDevice(port,ip)
{
    _commandNo = 600;
    _d1f5Count = 0;
    _dfb2Count = 0;
    _Port = port;
    _Ip = ip;
    m_isProgramming = false;
}

/**
 * @brief 通过端口号、ip地址以及设备名称执行构造
 * @param port  端口号
 * @param ip    ip地址
 * @param name  设备名称
 */
CycloneProUdp::CycloneProUdp(int port, string ip, string name):UdpDevice(port,ip,name)
{
    _commandNo = 600;
    _d1f5Count = 0;
    _dfb2Count = 0;
    _Port = port;
    _Ip = ip;
    _Name = name;
    m_isProgramming = false;
}

/**
 * @brief 测试函数
 */
void CycloneProUdp::test()
{

    int ret;
    string buf;
//    ret = StartProgram();
//    ret = CanAcess();
//    ret = IsInWorking();
//    buf = GetError();
    //buf = GetId();
    buf = WriteProgramAndGetId();
    printf("buf %s \n",buf.data());
    //ret = WriteId("D1F5","AAAAFFFF");
    //ret = WriteProgramAndSetId("D1F5","AAAAFFFF");
    //printf("buf = %s \n",buf.data());
//    printf("ret = %d\n",ret);
    //WriteProgramAndGetId();
}

/**
 * @brief 烧程并写指定Id
 * @param address   地址
 * @param id        指定ID
 * @return
 */
bool CycloneProUdp::WriteProgramAndSetId(string address, string id)
{
//    _commandNo = hextodec("AE8A", 4);

    m_isProgramming = true;      //是否正在工作
    bool isTimeOut = false;     //是否超时
    int count = 0;              //检测是否正在工作的次数

    //1、先烧程
    if (!StartProgram()) return false;
    //2、检查状态，等待烧程完成
    while (m_isProgramming)
    {
        //UDP协议通信时，同一条指令连续发送2次以上烧程器无法正确响应，此处检查工作状态后执行一次查错避免此情况发生
        count++;
        m_isProgramming = IsInWorking();
        if (!m_isProgramming) break;
       // cout<<_Name<<" 正在烧程中... "<<endl;
        _log.LOG_DEBUG("CycloneProUdp::WriteProgramAndSetId 【%s】设备,正在烧程中...【%d】",Name.data(),count);
        sleep(1);
        if (count > 12)
        {
            //此处设置烧成10秒超时，1000ms检测一次
            isTimeOut = true;
            m_isProgramming = false;
        }
    };
    if (isTimeOut)
    {
        //printf("烧程超时\n");
        _log.LOG_DEBUG("CycloneProUdp::WriteProgramAndSetId 【%s】设备,烧程超时",Name.data());
        return false;
    }    
    //cout<<_Name<<" 烧程结束 !"<<endl;
    _log.LOG_DEBUG("CycloneProUdp::WriteProgramAndSetId 【%s】设备,烧程结束 !",Name.data());
    //3、写ID
    if(!id.empty())
        if (!WriteId(address, id)) return false;
    //cout<<_Name<<" 写ID结束！"<<endl;
    _log.LOG_DEBUG("CycloneProUdp::WriteProgramAndSetId 【%s】设备,写ID结束！",Name.data());

    //4、查错
    usleep(100*1000);
    //查错
    string error = GetError();
    if (!error.empty())
    {        
        //cout<<"检查未通过,错误信息:"<<error<<endl;
        _log.LOG_DEBUG("CycloneProUdp::WriteProgramAndSetId 【%s】设备,检查未通过,错误信息:【%s】",Name.data(),error.data());
        return false;
    }    
    //cout<<_Name<<" 烧程写ID成功，ID : 【"<<id<<"】"<<endl;
    _log.LOG_DEBUG("CycloneProUdp::WriteProgramAndSetId 【%s】设备,烧程写ID成功【%s】: ",Name.data(),id.data());
    m_isProgramming = false;
    return true;    //没有错误,烧程成功
}

/**
 * @brief 烧程后读ID
 * @return
 */
string CycloneProUdp::WriteProgramAndGetId()
{
    _commandNo = hextodec("AE8A", 4);

    string id;
    m_isProgramming = true;      //是否正在工作
    bool isTimeOut = false;     //是否超时
    int count = 0;              //检测是否正在工作的次数

    StartProgram();
    printf("烧程开始：\n");
    while (m_isProgramming)
    {
        //UDP协议通信时，同一条指令连续发送2次以上烧程器无法正确响应，此处检查工作状态后执行一次查错避免此情况发生
        count++;
        m_isProgramming = IsInWorking();
        if (!m_isProgramming) break;
        printf("正在烧程中...");
        sleep(1); //延迟1s
        if (count > 10)
        {
            //此处设置烧成10秒超时，1000ms检测一次
            isTimeOut = true;
            m_isProgramming = false;
        }
    };
    if (isTimeOut)
    {
        printf("烧程超时\n");
        return "";
    }
    printf("烧程结束\n");
    id = GetId();

    string error = GetError();
    if (!error.empty())
    {
        printf("检查未通过,错误信息：%s\n",error.data());
        return "";
    }
    printf("烧程ID: %s,无错误信息\n",id.data());
    m_isProgramming = false;
    return id;
}

/**
 * @brief 写入指定的ID
 * @param address   地址
 * @param id        指定ID
 * @return
 */
bool CycloneProUdp::WriteId(string address, string id)
{
//    D2线修改为"185D0000" + address + id
//    vector<string> commands = !address.compare("D1F5") ? GetCommand("185D0000D1F5" + id) : GetCommand("185D0000DFB2" + id);
    vector<string> commands = GetCommand("185D0000" + address + id);
    string result = SendAndCheck(commands[0], commands[1], 1000L);

    if (result.substr(0,8).compare(commands[2]))
    {
        cout<<_Name<<" 烧程ID 【"<<id<<"】，写ID指令失败 ！！！"<<endl;
        return false;
    }
    //此处延迟必须添加，否则回读错误
    usleep(200*1000);   //延迟100ms
    string readBack = GetId(address);
    cout<<_Name<<" 烧程ID :"<<id<<" ,回读ID :"<<readBack<<endl;
    _log.LOG_DEBUG("CycloneProUdp WriteId 【%s】设备 烧程ID为【%s】,回读ID为【%s】",Name.data(),id.data(),readBack.data());
    if (!readBack.compare(id)) return true;
    return false;
}

/**
 * @brief 根据地址读ID
 * @param addressString 地址
 * @return
 */
string CycloneProUdp::GetId(string addressString)
{
    string id;
//    D2线修改为"182E0000" + addressString + "04"
//    vector<string> commands = addressString.compare("D1F5") ? GetCommand("182E0000DFB204") : GetCommand("182E0000D1F504");
    vector<string> commands = GetCommand("182E0000" + addressString + "04");
    string result = SendAndCheck(commands[0], commands[1], 1000);
    _log.LOG_DEBUG("CycloneProUdp 【%s】 GetID result: %s",Name.data(),result.data());
    if (result.length() == 26 && !result.substr(0,4).compare("0003") && !result.substr(22,4).compare("00EE"))
    {
        id = result.substr(12, 8);
        if (!id.compare("FFFFFFFF") || !id.compare("00000000"))
        {
            id = "";            
            cout<<"取ID失败："<<result<<endl;
        }
//        else
//        {
//            if (!addressString.compare("D1F5"))
//                _d1f5Count++;
//            else if (!addressString.compare("DFB2"))
//                _dfb2Count++;
//        }
    }
    else
    {
        cout<<_Name<<"读地址【"<<addressString<<"】取ID回码错误：【command[0]:"<<commands[0]<<"】,【command[1]:"<<commands[1]<<"】,【result:"<<result<<"】"<<endl;
    }
    return id;
}

/**
 * @brief 取得芯片ID，自动选择一个地址找ID
 * @return
 */
string CycloneProUdp::GetId()
{
    string id;
    if (_dfb2Count >= _d1f5Count)
    {
        id = GetId(_address_DFB2);
        if (id.empty()) id = GetId(_address_D1F5);
    }
    else
    {
        id = GetId(_address_D1F5);
        if (id.empty()) id = GetId(_address_DFB2);
    }
    return id;
}

/**
 * @brief 获取错误信息
 * @return
 */
string CycloneProUdp::GetError()
{
    vector<string> commands = GetCommand("1833");
    string result = SendAndCheck(commands[0], commands[1], 1000L);
    if (result.length() == 18)
    {
        string error = result.substr(12, 4);
        return error.compare("0000") ? error : "";
    }
    return "";
}

/**
 * @brief 是否忙碌状态
 * @return
 */
bool CycloneProUdp::IsInWorking()
{
    vector<string> commands = GetCommand("185F");
    string result = SendAndCheck(commands[0], commands[1], 1000L);
    string cmd2 = commands[2] + "AAAA0000EE";
    _log.LOG_DEBUG("CycloneProUdp 【%s】烧程过程中，查询是否在忙碌　result: %s cmd2: %s",Name.data(),result.data(),cmd2.data());
    if (!result.compare(commands[2] + "AAAA0000EE")) return true;
    return false;
}

/**
 * @brief 通信测试
 * @return
 */
bool CycloneProUdp::CanAcess()
{
    if(!m_isProgramming)
    {
        int j = 0;
        vector<string> commands = GetCommand("010B");
        string result = SendAndCheck(commands[0],commands[1], 1000L);
//        cout<<"result = "<<result<<endl;
        if((!result.empty()) && (result.length()>3))
        {
            while(j < result.length())
            {
                if((result[j] == '5')&&(result[j+1] == '0')&&(result[j+2] == '2')&&(result[j+3] == '6')&&(result[j+4] == '4')&&(result[j+5] == '5'))   //(P&E)
                {
                    return true;
                }
                j++;
            }
        }
        return false;
    }
    else
        return true;
}

/**
 * @brief 开始烧程
 * @return
 */
bool CycloneProUdp::StartProgram()
{
    cout << " StartProgram " << endl;
    vector<string> commands = GetCommand("181C01");
    string result = SendAndCheck(commands[0], commands[1], 1000L);
    if (result.compare(commands[2] + "AAAAEE")) return false;

    commands = GetCommand("1841");
    result = SendAndCheck(commands[0], commands[1], 1000L);
    if (result.compare(commands[2] + "AAAAEE")) return false;    
    cout<<_Name<<" 开始烧程指令成功！"<<endl;
    return true;
}

/**
 * @brief 发送指令并检查返回结果
 * @param command   指令内容
 * @param result    对比结果字符串
 * @param timeout   超时时间
 * @return
 */
string CycloneProUdp::SendAndCheck(string command, string result, long timeout)
{
     string tmp_buff;
     string m_buff[10];
     struct timeval tv_start,tv_current;
     int total_time = 0;
     int recvCount=0;
     bool tryRecv=true;

      if (timeout < 0)
          timeout = 1000; //1000ms
      if((Write(command,"Hex"))<0)
      {
          printf("Write Data error\n");
          return "";
      }
      gettimeofday(&tv_start,0);
      while(tryRecv)
      {
          gettimeofday(&tv_current,0);
          total_time = (tv_current.tv_sec - tv_start.tv_sec) * 1000 ;
          total_time += (tv_current.tv_usec - tv_start.tv_usec)/1000;
//          cout<<"--- SendAndCheck  total_time = "<<total_time<<endl;

          if(total_time > timeout)
          {
              printf("Time out \n");
              tryRecv=false;
          }

          tmp_buff = Read("Hex");
//          cout<<" --- SendAndCheck tmp_buff = "<<tmp_buff<<endl;
          if((tmp_buff.empty())||(tmp_buff.length()== 0))
              continue;

          m_buff[recvCount] = tmp_buff;
          recvCount++;
//          cout<<"-- SendAndCheck recvCount = "<<recvCount<<endl;

          if(recvCount>1)
              tryRecv=false;
      }

      if((strcmp(m_buff[0].data(),result.data())==0)&&(!m_buff[0].empty()))
      {
          if(!m_buff[1].empty())
          {
              return m_buff[1];
          }
          else{
              _log.LOG_DEBUG("CycloneProUdp 【%s】 m_buff[0] == result %s m_buff[1] is empty.",Name.data(),m_buff[0].data());
              return "";
          }
      }
      else
      {          
          _log.LOG_DEBUG("CycloneProUdp 【%s】 m_buff[0] : %s != result : %s",Name.data(),m_buff[0].data(),result.data());
          printf("Send Check Error !\n");
          return "";
      }
}

/**
 * @brief 根据指令内容取完整指令及指令校验序号,返回字符串数组，元素1：完整指令帧，元素2：返回帧1帧头，元素3：返回帧2帧头
 * @param commandContent    指令内容
 * @return  字符串数组，元素1：完整指令帧，元素2：返回帧1帧头，元素3：返回帧2帧头
 */
vector<string> CycloneProUdp::GetCommand(string commandContent)
{
//    cout << "commandcontent = " << commandContent << endl;
    if (_commandNo >= 65530)
        _commandNo = hextodec("A1A2", 4);
    _commandNo++;
    vector<string> commands ;
    char cmdNo[5] = {0};
    sprintf(cmdNo,"%04X",_commandNo);
    string str = cmdNo;
    commands.push_back("0003" + str + commandContent);
    commands.push_back("0004" + str);
    commands.push_back("0003" + str);
    return commands;
}

/**
 * @brief 删除所有程序文件
 * @return
 */
bool CycloneProUdp::DeleteAllSap()
{
    vector<string> commands = GetCommand("18060100000000");
    string result = SendAndCheck(commands[0], commands[1], 2500);
    if (result.compare(commands[2] + "AAAAEE")) return false;

    commands = GetCommand("180D");
    result = SendAndCheck(commands[0], commands[1], 2500);
    if (result.compare(commands[2] + "AAAAFFFFEE"))
    {
        _log.LOG_ERROR("CycloneProUdp 【%s】设备 Sap文件清除【失败】",Name.data());
        return false;
    }
    _log.LOG_DEBUG("CycloneProUdp 【%s】设备 Sap文件清除成功!",Name.data());
    return true;
}

/**
 * @brief 添加Sap文件
 * @param filePath  文件路径
 * @return
 */
bool CycloneProUdp::AddSapFileByName(string filePath)
{
    _log.LOG_DEBUG("CycloneProUdp 【%s】设备,开始添加SAP文件，文件路径为【%s】",Name.data(),filePath.data());

    m_NameSAP = "";
    QStringList str = QString(filePath.data()).split("/");
    if (str.size() > 0)
    m_NameSAP = str[str.size() -1].toStdString();
    mycout(m_NameSAP);

    FILE* fs = fopen(filePath.data(), "rb");
    if(fs == NULL)
    {
        perror("fopen error:\n");
        _log.LOG_ERROR("CycloneProUdp 【%s】设备,AddSapFileByName 文件打开【失败】",Name.data());
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
            _log.LOG_ERROR("CycloneProUdp 【%s】设备,AddSapFileByName 文件读取【失败】",Name.data());
            perror("fread error:\n");
            return 0;
        }
        else if(res == 0)
        {
            break;
        }
    }
    fclose(fs);

    return AddSapFile(sb, 3000);
}

/**
 * @brief 添加Sap文件，添加后烧程器中仅存在一个SAP文件
 * @param fileContent   文件内容
 * @return
 */
bool CycloneProUdp::AddSapFile(string fileContent, long timeout)
{
    vector<string> commands;
    string result = "";
    if (fileContent.empty())
    {
        _log.LOG_ERROR("CycloneProUdp 【%s】设备,AddSapFile 文件内容【为空】",Name.data());
        return false;
    }
    fileContent = Replace(fileContent,"");
    if (fileContent.empty())
    {
        _log.LOG_ERROR("CycloneProUdp Replace 【%s】设备,AddSapFile 文件内容【为空】",Name.data());
        return false;
    }

    //添加sap文件：
    //步骤一（固定写法）
    commands = GetCommand("012053657276657200");
    result = SendAndCheck(commands[0], commands[1], timeout);
    if (result.substr(0,8).compare(commands[2])) return false;
//        cout<<"步骤一（固定写法）"<<endl;

    //步骤二（固定写法）
    commands = GetCommand("010B");
    result = SendAndCheck(commands[0], commands[1], timeout);
    if (result.substr(0,8).compare(commands[2])) return false;
//        cout<<"步骤二（固定写法）"<<endl;

    //步骤三
    //commands = GetCommand("18090100000000");
    commands = GetCommand("18060100000080");//大文件代码样本 wuchao 20180730
    result = SendAndCheck(commands[0], commands[1], timeout);
    if (result.substr(0,8).compare(commands[2])) return false;
//        cout<<"步骤三"<<endl;

    //步骤四(查错)
    commands = GetCommand("1833");
    result = SendAndCheck(commands[0], commands[1], timeout);
    if (result.compare(commands[2] + "AAAA0000EE")) return false;
//        cout<<"步骤四(查错)"<<endl;

    //_commandNo = Convert.ToInt32("B420", 16);
    //步骤五（写入文件内容）
    int mark = 0;
    int totalLen = 0;
    while (true)
    {
        totalLen = fileContent.length();
        if (totalLen <= 0) break;
        mark = (totalLen > 2048 ? 2048 : totalLen);
        string datas = fileContent.substr(0, mark);
        fileContent = fileContent.substr(mark);

        char cmd[10] = {0};
        sprintf(cmd,"1807%4X",(datas.length() / 2));
        string command = cmd;
        commands = GetCommand(command + datas);
        result = SendAndCheck(commands[0], commands[1], 5000);
//        cout  << "result = " << result << endl;
        if (result.compare(commands[2] + "AAAAFFFFEE")) return false;
    }
//        cout<<" 步骤五（写入文件内容）"<<endl;


    //步骤六（查错）
    commands = GetCommand("1833");
    result = SendAndCheck(commands[0], commands[1], timeout);
//        cout<<" 步骤六（查错） result = "<<result<<endl;
//        cout<<" 步骤六（查错） commands[2] = "<<commands[2]<<endl;
    if (result.compare(commands[2] + "AAAA0000EE")) return false;
//        cout<<"步骤六（查错）"<<endl;

    //步骤七
    commands = GetCommand("180E");
    result = SendAndCheck(commands[0], commands[1], timeout);
//        cout<<" 步骤七  result = "<<result<<endl;
//        cout<<" 步骤七  commands[2] = "<<commands[2]<<endl;
    if (result.compare(commands[2] + "AAAAFFFFEE")) return false;
//        cout<<"步骤七"<<endl;

    //步骤八
    commands = GetCommand("181B");
    result = SendAndCheck(commands[0], commands[1], timeout);
//        cout<<" 步骤八  result = "<<result<<endl;
//        cout<<" 步骤八  commands[2] = "<<commands[2]<<endl;
    if (result.compare(commands[2] + "AAAA0101EE")) return false;
//        cout<<" 步骤八"<<endl;
    //步骤九
    commands = GetCommand("18000041");
    result = SendAndCheck(commands[0], commands[1], timeout);
    if (result.compare(commands[2] + "AAAAEE")) return false;
//        cout<<" AddSapFile 9999"<<endl;
    _log.LOG_DEBUG("CycloneProUdp 【%s】设备,添加SAP文件成功",Name.data());
    return true;
}

/**
 * @brief 重启烧程器，返回帧只有一帧
 * @return
 */
bool CycloneProUdp::Reset()
{
    vector<string> commands = GetCommand("0203");
    string result = WriteAndRead(commands[0], "Hex", 100);
    if (!result.compare(commands[1])) return true;
    return false;
}
