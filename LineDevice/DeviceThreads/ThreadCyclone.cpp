#include "ThreadCyclone.h"
#include "main.h"


/**
 * @brief 构造函数
 * @param di
 */
ThreadCyclone::ThreadCyclone(DeviceInfo di): DeviceThread(di)
{
    //烧程器默认网口端口8738
    if (di.Port.empty()) _di.Port = "8738";
    _stopprocess = false;
    myDevice = new CycloneProUdp(atoi(_di.Port.data()),_di.Ip,_di.Name);
    repeatFlag = true;
    _workTimes = 0;
    m_displayValue = "";
    _counter = 0;
}

ThreadCyclone::~ThreadCyclone()
{
    if(myDevice != NULL)
    {
        delete myDevice;
        myDevice = NULL;
    }
}

/**
 * @brief 驱动线程处理函数
 */
void ThreadCyclone::threadprocess()
{
    _log.LOG_INFO("ThreadCyclone 【%s】线程驱动已启动......",_di.Name.data());

    //得到设备编号
    getDeviceNum();
    //向烧程器添加SAP烧程文件
#ifndef _Debug
    if(!Init())
    {
       return;
    }
#endif

    while (!_stopprocess)
    {
       usleep(100 * 1000);
       //设备不使能时,默认设备的判定结果为良品
       if(!getDeviceEnable())
           continue;
       //处理结束信号点
        processEndFlag();

       // 遍历采集点
        for(map<string, UnitInfo>::reverse_iterator it = _di.Units.rbegin();it != _di.Units.rend();it++)
        {
           map<string, Tag> tags = it->second.Tags;
           for(map<string, Tag>::reverse_iterator im = tags.rbegin();im != tags.rend();im++)
           {
               Tag tag = im->second;
               string name = tag.TagCode;
               tag.MemortValue = m_db.Read_TagMValue (tag.TagName);
               //处理烧程信号
               if(!name.compare("D3SP"))
               {
                   if (!tag.MemortValue.compare("1"))
                   {
                       _log.LOG_DEBUG("ThreadCyclone 【%s】 检测到烧程信号！",_di.Name.data());
                       m_db.Write_TagMValue(tag.TagName,"0");
                       string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                       _log.LOG_DEBUG("ThreadCyclone 【%s】 序列号为【%s】",_di.Name.data(),partSeqNo.data());
                       string partNoID = processCyclone(partSeqNo);

                       pi.testItemEigenValue = _num;

                       saveToSql(partNoID, partSeqNo,0);
                       SendCycloneInfoToPC(partNoID,partSeqNo);
                   }
               }
               //处理通信状态
               else if (!name.compare("CS"))
               {
                   if(_counter % 20 == 0)
                   {
                       CommunicateTest();
                       _counter = 1;
                   }
               }
            }
        }
        _counter++;
    }
    _log.LOG_INFO("ThreadCyclone 【%s】 线程驱动已停止....",_di.Name.data());
}

void ThreadCyclone::SendCycloneInfoToPC(string partNoId,string partSeqNo)
{
    string sId = _num + "$" + "ID";
    m_db.Write_TagMValue(sId, partNoId);
    string sSeqNo = _num + "$" + "SQ";
    m_db.Write_TagMValue(sSeqNo, partSeqNo);
    string sPos = _num + "$" + "PS";
    m_db.Write_TagMValue(sPos, _num);

    int pos=0;
    pos = atoi(_di.testStartPos.data());
    for(int i=pos;i<_di.testItemCode.size()+pos;i++)
    {
        string sCollectValue = _num + "$" + "CV" + IntToString(i);
        m_db.Write_TagMValue(sCollectValue, _num);
    }
}

/**
 * @brief ThreadCyclone::GetProgramAddress          获取烧程地址
 * @return
 */
string ThreadCyclone::GetProgramAddress()
{
    string data = "";
    for(int i=0; i<_di.parameterNo.size(); i++)
    {
        if(atoi((const char*)_di.parameterNo.at(i).data()) == CycloneAddress)
        {
            if(_di.parameter.size() > i)
                data = _di.parameter.at(i);
            else
                _log.LOG_ERROR("ThreadCyclone 【%s】 获取烧程地址失败",_di.Name.data());
        }
    }
    return data;
}

/**
 * @brief ThreadCyclone::AssignProgramId            分配烧程ID
 * @return
 */
string ThreadCyclone::AssignProgramId()
{
    string programId;
    string command;
    MYSQL_ROW row;
    MYSQL_RES* result;
    MYSQL* m_connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(m_connection);
    devPara = CyclonePath;
    string mode = getParameterNoInfo();
//    string mode = GetProductModel();
    if(!mode.empty())
    {
        command.append("CALL generate_id_by_model ('").append(mode).append("');");
//        command.append("CALL generate_id_by_model ('").append("QY1104").append("');");
        _log.LOG_DEBUG("ThreadCyclone AssignProgramId 查询命令command = 【%s】",command.data());
        result = helper.LoadTable(m_connection,command);
        if(result != NULL)
        {
            while(row = mysql_fetch_row(result))
            {
                if(row[0]!=NULL)
                    programId = row[0];
            }
            helper.MySQL_Release(result);
        }
        else
        {
            _log.LOG_ERROR("ThreadCyclone 【%s】 从服务器获取烧程ID失败",_di.Name.data());
        }

        _log.LOG_DEBUG("ThreadCyclone 【%s】 从服务器获取烧程ID为【%s】",_di.Name.data(),programId.data());
        helper.MySQL_Close(m_connection);
        delete m_connection;
        return programId;
    }
    else
    {
        m_db.Write_TagMValue(_di.devParaAlarm, "1");
        _log.LOG_ERROR("ThreadCyclone 【%s】 获取【加工参数】 烧程文件路径【失败】",_di.Name.data());
        m_db.Write_TagMValue(_di.IdFlag, "获取【加工参数】 烧程文件路径【失败】");
        return "";
    }
}


/**
 * @brief 驱动线程函数启动接口
 * @param arg
 * @return
 */
void *ThreadCyclone::Start_Thread(void* arg)
{
    ThreadCyclone *th = (ThreadCyclone*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief 启动驱动线程
 * @return
 */
bool ThreadCyclone::Start()
{
    int ret = pthread_create(&pth,NULL,ThreadCyclone::Start_Thread,this);
    if(ret != 0)
        return false;
    else
        return true;
}

/**
 * @brief 驱动线程关闭
 * @return
 */
bool ThreadCyclone::Stop()
{
    _log.LOG_DEBUG("ThreadCyclone 【%s】线程驱动已退出 ....",_di.Name.data());
    _stopprocess = true;
    pthread_detach(pth);
    pthread_cancel(pth);
    myDevice->Close();
    return true;
}

/**
 * @brief ThreadCyclone::initDevice                 添加烧程文件
 * @return
 */
bool ThreadCyclone::initDevice()
{
    myDevice->DeleteAllSap();
    if(gLocalPathSAP.empty())
    {
        return false;
    }
    else
    {
        return myDevice->AddSapFileByName(gLocalPathSAP);
    }
}

/**
 * @brief ThreadCyclone::getPlcAlarmPoint           获取PLC报警点
 * @return
 */
string ThreadCyclone::getPlcAlarmPoint()
{
    string plcAlarm;
    for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++)
    {
        DeviceInfo di = it->second;
        if(di.Name == "1#PLC")
             plcAlarm = di.communicateERR;           //PLC报警点位(MCD模式)
    }
    return plcAlarm;
}

/**
 * @brief ThreadCyclone::getParameterNoInfo
 * @return
 */
string ThreadCyclone::getParameterNoInfo()
{
    string data = "";
    for(int i = 0;i < _di.parameterNo.size();i++)
    {
        int type = atoi((const char*)_di.parameterNo.at(i).data());
        if(type == devPara)
        {
            if(_di.parameter.size() > i)
                data = _di.parameter.at(i);
            else
                _log.LOG_ERROR("ThreadCyclone 【%s】 获取加工参数失败",_di.Name.data());
        }
    }
//    if(data.empty())
//        _log.LOG_ERROR("ThreadCyclone 【%s】 获取加工参数【失败】 参数数组大小为：【%d】",_di.Name.data(),_di.parameterNo.size());
    return data;
}

/**
 * @brief ThreadCyclone::Init               向烧程器添加SAP烧程文件

 */
bool ThreadCyclone::Init()
{
    if(!initDevice())
    {
        //机台报警
        m_db.Write_TagMValue(_di.devInitAlarm, "1");
        //向上位机上传报警信息
        m_db.Write_TagMValue(gLine.Si.alarmStatus,"0");
        m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 更新SAP烧程文件失败!");
        //界面显示
        m_db.Write_TagMValue(_di.IdFlag, "更新SAP烧程文件【失败】");
        _log.LOG_ERROR("ThreadCyclone 【%s】 更新SAP烧程文件【失败】",_di.Name.data());
        return false;
    }
    else
    {
        _log.LOG_DEBUG("ThreadCyclone 【%s】 更新SAP烧程文件【%s】成功",_di.Name.data(),gFullPathSAP.data());
        char m_buff[100] = {0};
        memset(m_buff,0,100);
        sprintf(m_buff,"更新【%s】OK",gFullPathSAP.data());
        string buff = m_buff;
        m_db.Write_TagMValue(_di.IdFlag, buff);
        return true;
    }
}

/**
 * @brief ThreadCyclone::processEndFlag             处理结束信号
 */
void ThreadCyclone::processEndFlag()
{
    _endFlag = m_db.Read_TagMValue(_di.EndFlag);
    //开始信号为"1"时，清空遗留信息
    if((!_endFlag.empty()) && (!_endFlag.compare("1")))
    {
         usleep(500*1000);
         _log.LOG_DEBUG("ThreadCyclone 【%s】检测到结束采集信号，清空序列号，烧程ID，复位结束采集信号点",_di.Name.data());
         m_db.Write_TagMValue(_di.EndFlag,"0");
         char m_buff[100] = {0};
         memset(m_buff,0,100);
         sprintf(m_buff,"当前烧程文件【%s】",gFullPathSAP.data());
         string buff = m_buff;
         m_db.Write_TagMValue(_di.IdFlag, buff);
//         m_db.Write_TagMValue(_di.IdFlag,"");
         m_db.Write_TagMValue(_di.SnFlag,"");
         m_db.Write_TagMValue(_di.JudgeResult, "");
         m_displayValue.clear();
         pi.Clear();
         _workTimes = 0;
    }
}

/**
 * @brief ThreadCyclone::processCyclone         开始处理烧程信号
 * @param partSeqNo
 */
string ThreadCyclone::processCyclone(string partSeqNo)
{
    string partNoID;
    if(!partSeqNo.empty())
    {
        string address = getCycloneaddress();
        if(!address.empty())
        {
            partNoID = startToCyclone(address);
        }
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadCyclone 【%s】 获取序列号为【空】，不进行测试，直接判定为【不良品】",_di.Name.data());
        m_db.Write_TagMValue(_di.IdFlag,"扫码失败，不进行测试，直接判定为【不良品】");
    }
    return partNoID;
}

/**
 * @brief ThreadCyclone::getCycloneaddress              获取烧程地址
 * @return
 */
string ThreadCyclone::getCycloneaddress()
{
    devPara = CycloneAddress;
    string address = getParameterNoInfo();

    if(address.empty())
    {
        m_db.Write_TagMValue(_di.devParaAlarm, "1");
        _log.LOG_ERROR("ThreadCyclone 【%s】 获取【加工参数】烧程地址【失败】",_di.Name.data());
    }
    else
    {
        _log.LOG_DEBUG("ThreadCyclone 【%s】 获取烧程地址【%s】",_di.Name.data(),address.data());
    }
    return address;
}

/**
 * @brief ThreadCyclone::startToCyclone         开始烧程
 * @param address
 * @return
 */
string ThreadCyclone::startToCyclone(string address)
{
    //从数据库申请烧程ID
    string programId = AssignProgramId();
    pi.testItemCollectValue = myDevice->m_NameSAP;  //保存烧程文件名,做版本追溯
    if(programId.empty())
    {
        _log.LOG_ERROR("ThreadCyclone 【%s】 数据库取ID【失败】",_di.Name.data());
        m_db.Write_TagMValue(_num + "$" + "NT","数据库取ID失败");
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        m_db.Write_TagMValue(_di.IdFlag, "数据库取ID【失败】");
        pi.testItemEigenValue = "NG";
    }
    else
    {
        //烧程写ID，并判断是否成功
        _log.LOG_DEBUG("ThreadCyclone 【%s】 烧程写ID为【%s】",_di.Name.data(),programId.data());
#ifndef _Debug
        bool programResult = myDevice->WriteProgramAndSetId(address,programId);
#else
        bool programResult = true;
#endif
        if(programResult)
        {
            _log.LOG_DEBUG("ThreadCyclone 【%s】 烧程【成功】",_di.Name.data());
            pi.testItemJudgeResult = 1;
            m_db.Write_TagMValue(_di.JudgeResult, "1");
            //界面显示
            m_db.Write_TagMValue(_di.IdFlag, "烧程ID为：【" + programId + "】");
        }
        else
        {
            _log.LOG_DEBUG("ThreadCyclone 【%s】 第二次烧程烧程写ID为【%s】",_di.Name.data(),programId.data());
            bool programResult2 = myDevice->WriteProgramAndSetId(address,programId);
            if(programResult2){
                _log.LOG_DEBUG("ThreadCyclone 【%s】 第二次烧程烧程【成功】",_di.Name.data());
                pi.testItemJudgeResult = 1;
                m_db.Write_TagMValue(_di.JudgeResult, "1");
                //界面显示
                m_db.Write_TagMValue(_di.IdFlag, "烧程ID为：【" + programId + "】");
            }else{
                _log.LOG_ERROR("ThreadCyclone 【%s】 烧程【失败】",_di.Name.data());
    //          programId.clear();
                myDevice->Reset();
                pi.testItemJudgeResult = 0;
                m_db.Write_TagMValue(_di.JudgeResult, "0");
                //界面显示
                m_db.Write_TagMValue(_di.IdFlag, "烧程失败");
            }
        }
        return programId;
    }
}

void ThreadCyclone::CommunicateTest()
{
    _connectstatus = myDevice->CanAcess() ? "1" : "0";
    m_db.Write_TagMValue(_di.Alarm, _connectstatus);
    if(_connectstatus == "0")
    {
        m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
        _log.LOG_ERROR("ThreadCyclone 【%s】 通信检测【失败】",_di.Name.data());
    }
    else
    {
        m_db.Write_TagMValue(_num + "$" + "NT","");
//        _log.LOG_DEBUG("ThreadCyclone 【%s】 通信检测【成功】",_di.Name.data());
    }
}

