#include "AlarmDriver.h"
#include "LineDevice/DeviceThreads/ThreadFxPlc.h"
#include "main.h"

AlarmInfo comAi;        //设备通信
AlarmInfo plcAi;        //PLC报警点位
AlarmInfo devParaAi;    //设备参数报警
AlarmInfo devInitAi;      //设备初始化失败报警

/**
 * @brief AlarmDriver::AlarmDriver
 */
AlarmDriver::AlarmDriver()
{
    _stop = false;
    alarmFlag = true;
    m_plcCommERR = "";
    m_startAddress = 0;
    m_devFlag =true;
    m_testFlag = true;
    m_initFlag = true;
}

/**
 * @brief AlarmDriver::~AlarmDriver
 */
AlarmDriver::~AlarmDriver()
{

}

/**
 * @brief AlarmDriver::initSqlPara                  初始化数据库参数
 * @param ai
 */
void AlarmDriver::initSqlPara(AlarmInfo &ai)
{
    if(EnableSampleMode == 1)
    {
        ai.Dispatch_No = "sam" + dispatchNo;
    }
    else
    {
        ai.Dispatch_No = dispatchNo;
    }
    ai.Part_No = partNo;
    ai.Production_Line = gLine.Si.ProductionLine;
    ai.Work_Center_No = g_dbInfo.GetWorkCenterNo();
    ai.Alarm_Level = "2";
    ai.Enter_User = user;
//    cout<<"---- Work_Center_No = "<<ai.Work_Center_No<<endl;
//    cout<<"---- MCH_Code = "<<ai.MCH_Code<<endl;
//    cout<<"---- Alarm_Content = "<<ai.Alarm_Content<<endl;
}

/**
 * @brief AlarmDriver::saveAlarmInfoToSql
 * @param ai
 * @return
 */
bool AlarmDriver::saveAlarmInfoToSql(AlarmInfo &ai)
{
    initSqlPara(ai);//有可优化空间
    MYSQL* m_connection = new MYSQL;
    string sql;
    MySQLHelper mysql(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    if(m_connection != NULL)
    {
        if(!mysql.MySQL_Init(m_connection))
        {
            mysql.MySQL_Close(m_connection);
            delete m_connection;
            return false;
        }
        sql = "insert into alarm_info_temp(Dispatch_No,Part_No,Production_Line,Work_Center_No,MCH_Code,";
        sql = sql + "Alarm_Level,Alarm_Category,Alarm_Content,Alarm_Processing_Method,Enter_User) ";
        sql = sql + "values("+"'" + ai.Dispatch_No + "','" + ai.Part_No + "','";
        sql = sql + ai.Production_Line + "','" + ai.Work_Center_No + "','" ;
        sql = sql + ai.MCH_Code + "','" + ai.Alarm_Level+ "','" + ai.Alarm_Category+ "','";
        sql = sql + ai.Alarm_Content+ "','"+ ai.Alarm_Processing_Method+ "','"+ ai.Enter_User + "')";

        _log.LOG_DEBUG("AlarmDriver SaveProductInfo  插入数据库【%s】",sql.data());

        //执行sql语句，插入数据库
        bool result = mysql.InsertTable(m_connection,sql);
        if(!result)
        {
            _log.LOG_ERROR("AlarmDriver SaveProductInfo MySQL InsertTable Error");
            mysql.MySQL_Close(m_connection);
            delete m_connection;
            return false;
        }
        mysql.MySQL_Close(m_connection);
        delete m_connection;
        _log.LOG_DEBUG("AlarmDriver SaveProductInfo MySQL InsertTable finish !");
    }
    else
    {
        return false;
    }
    return true;
}

/**
 * @brief AlarmDriver::communicateAlarm                     设备通信报警
 * @param comAi
 */
void AlarmDriver::communicateAlarm(AlarmInfo &comAi)
{
    vector<string> tmp;
    int num=0;
    int starflag = TcpWorkThread::GetStartFlag();                   //用于控制LogicalDriver线程循环时是否操作gLine设备数据单元，为0，设备驱动未启动，不操作gLine设备数据结构体。为1，设备驱动启动，可以操作gLine设备数据结构体。
    if(starflag == 1)
    {
        for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++,num++)
        {
            DeviceInfo di = it->second;
            string connectStatus = m_db.Read_TagMValue(di.Alarm);
    //        cout<<di.Name<<"-------  communicateAlarm connectStatus = "<<connectStatus<<endl;
            comAi.MCH_Code = di.DeviceCode;

            if(!m_preConnectStatus.empty() && m_preConnectStatus.size() == _counter)
            {
    //            cout<<"name = "<<di.Name<< "---- num = "<<num<<" ---- connectStatus = "<<connectStatus<<" ---- m_preConnectStatus = "<<m_preConnectStatus[num]<<endl;
                if(connectStatus != m_preConnectStatus[num])
                {
                    if(m_preConnectStatus[num] != "0" && connectStatus == "0")
                    {
                        _log.LOG_DEBUG("AlarmDriver communicateAlarm 【%s】 通信异常【报警】",di.Name.data());
                        comAi.Alarm_Content = di.Name + "通信异常";
                        comAi.Alarm_Processing_Method = "1";
                        saveAlarmInfoToSql(comAi);
                        m_db.Write_TagMValue(m_plcCommERR, "1");
                    }
                    else if(m_preConnectStatus[num] == "0" && connectStatus == "1")
                    {
                        _log.LOG_DEBUG("AlarmDriver communicateAlarm 【%s】 通信恢复【正常】",di.Name.data());
                        comAi.Alarm_Content = di.Name + "通信恢复正常";
                        comAi.Alarm_Processing_Method = "0";
                        saveAlarmInfoToSql(comAi);
                        m_db.Write_TagMValue(m_plcCommOK, "1");
                    }
                }
    //            else
    //            {
    //                if(m_preConnectStatus[num] == "0" && connectStatus == "0")
    //                {
    //                    if(!di.communiAlarmFlag)
    //                    {
    //                        di.communiAlarmFlag = true;
    //                        comAi.Alarm_Content = di.Name + "通信异常";
    //                        comAi.Alarm_Processing_Method = "1";
    //                        saveAlarmInfoToSql(comAi);
    //                        m_db.Write_TagMValue(m_plcCommERR, "1");
    ////                        m_db.Write_TagMValue(gLine.Si.alarmStatus, "0");
    //                        _log.LOG_DEBUG("AlarmDriver communicateAlarm 【%s】 通信异常【报警】",di.Name.data());
    //                    }

    //                }
    //                else if(m_preConnectStatus[num] == "1" && connectStatus == "1")
    //                {
    //    //                    cout<<"通信一直正常，不插入数据库"<<endl;
    //                    m_db.Write_TagMValue(gLine.Si.alarmStatus, "1");
    //                }
    //            }
            }
            tmp.push_back(connectStatus);
    //        it->second = di;
        }
        _counter = num;
        if(!tmp.empty())
        {
            m_preConnectStatus.assign(tmp.begin(), tmp.end());
        }
    }
}

/**
 * @brief AlarmDriver::plcPointAlarm                PLC点位报警
 * @param plcAi
 */
void AlarmDriver::plcPointAlarm(AlarmInfo &plcAi)
{
    if(!ThreadFxPlc::m_alarmAddress.empty())
    {
        m_startAddress = ThreadFxPlc::m_alarmAddress[0];//获取报警点起始地址
    }

    vector<int> AlarmValue = ThreadFxPlc::getPlcAlarmValue();//获取报警点寄存器值

//    if(!m_prePLcAlarmValue.empty())
//    {
//        for(int i=0;i<m_prePLcAlarmValue.size();i++)
//            cout<<"---- m_prePLcAlarmValue = "<<m_prePLcAlarmValue.at(i)<<endl;
//    }

//    if(!AlarmValue.empty())
//    {
//        for(int i=0;i<AlarmValue.size();i++)
//            cout<<"---- AlarmValue = "<<AlarmValue.at(i)<<endl;
//    }
//    else
//    {
//        cout<<"---- AlarmValue size = "<<AlarmValue.size()<<endl;
//    }

    if(!PlcAlarmVec.empty() && !AlarmValue.empty() && m_startAddress!=0)
    {
        for(int i=0; i<PlcAlarmVec.size();i++)
        {
            int index = PlcAlarmVec[i].m_registerAddr - m_startAddress;
//                cout<<"AlarmDriver index = "<<index<<" m_registerAddr = "<<PlcAlarmVec[i].m_registerAddr<<" startAddress = "<<startAddress<<endl;
            if(AlarmValue.size() == m_prePLcAlarmValue.size())
            {
//                cout<<"index = "<<index<<" m_prePLcAlarmValue = "<<m_prePLcAlarmValue[index]<<" AlarmValue = "<<AlarmValue[index]<<endl;
                if(AlarmValue[index]!=m_prePLcAlarmValue[index])
                {
                    if(m_prePLcAlarmValue[index] == 1 && AlarmValue[index] == 0 )
                    {
//                        cout<<"***** address = "<<PlcAlarmVec[i].m_registerAddr<<" name = "<<PlcAlarmVec[i].m_pointName<<" 【报警解除】 "<<"index = "<<index<<" m_prePLcAlarmValue = "<<m_prePLcAlarmValue[index]<<" AlarmValue = "<<AlarmValue[index]<<endl;
                        _log.LOG_ERROR("AlarmDriver 【报警解除】 address = 【%d】 name = 【%s】 index = 【%d】 m_prePLcAlarmValue = 【%d】 AlarmValue = 【%d】",PlcAlarmVec[i].m_registerAddr,PlcAlarmVec[i].m_pointName.data(),index,m_prePLcAlarmValue[index],AlarmValue[index]);
                        m_db.Write_TagMValue(PlcAlarmVec[i].m_pointName, "0");//用于报警显示
                        plcAi.Alarm_Content = PlcAlarmVec[i].m_pointName;//plcAi用于填充报警结构，用于后续插入数据库数据
                        plcAi.Alarm_Processing_Method = "0";
                        saveAlarmInfoToSql(plcAi);
                    }
                    else if(m_prePLcAlarmValue[index] != 1 && AlarmValue[index] == 1)
                    {
//                        cout<<"----- 【开始报警】 "<<" address = "<<PlcAlarmVec[i].m_registerAddr<<" name = "<<PlcAlarmVec[i].m_pointName<<" index = "<<index<<" m_prePLcAlarmValue = "<<m_prePLcAlarmValue[index]<<" AlarmValue = "<<AlarmValue[index]<<endl;
                        _log.LOG_ERROR("AlarmDriver 【开始报警】 address = 【%d】 name = 【%s】 index = 【%d】 m_prePLcAlarmValue = 【%d】 AlarmValue = 【%d】",PlcAlarmVec[i].m_registerAddr,PlcAlarmVec[i].m_pointName.data(),index,m_prePLcAlarmValue[index],AlarmValue[index]);
                        m_db.Write_TagMValue(PlcAlarmVec[i].m_pointName, "1");
                        plcAi.Alarm_Content = PlcAlarmVec[i].m_pointName;
                        plcAi.Alarm_Processing_Method = "1";
                        saveAlarmInfoToSql(plcAi);
                    }
                }
                else
                {
                    if(m_prePLcAlarmValue[index] == 1 && AlarmValue[index] == 1)
                    {
//                        _log.LOG_ERROR("AlarmDriver 【仍然报警】 address = 【%d】 name = 【%s】 index = 【%d】 m_prePLcAlarmValue = 【%d】 AlarmValue = 【%d】",PlcAlarmVec[i].m_registerAddr,PlcAlarmVec[i].m_pointName.data(),index,m_prePLcAlarmValue[index],AlarmValue[index]);
                        m_db.Write_TagMValue(PlcAlarmVec[i].m_pointName, "1");
                    }
                    else if(m_prePLcAlarmValue[index] == 0 && AlarmValue[index] == 0)
                    {
                        m_db.Write_TagMValue(PlcAlarmVec[i].m_pointName, "0");
//                        cout<<"address = "<<PlcAlarmVec[i].m_registerAddr<<" name = "<<PlcAlarmVec[i].m_pointName<<" 没有报警"<<endl;
                    }
                }
            }
            else
            {
                break;
            }
        }
    }
    if(!AlarmValue.empty())
    {
        m_prePLcAlarmValue.assign(AlarmValue.begin(), AlarmValue.end());
    }
}


/**
 * @brief AlarmDriver::deviceParaAlarm                      设备参数报警
 * @param paraAi
 */
void AlarmDriver::deviceParaAlarm(AlarmInfo &paraAi)
{
    int starflag = TcpWorkThread::GetStartFlag();                   //用于控制AlarmDriver线程循环时是否操作gLine设备数据单元，为0，设备驱动未启动，不操作gLine设备数据结构体。为1，设备驱动启动，可以操作gLine设备数据结构体。
    if(starflag == 1)
    {
        for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++)
        {
            DeviceInfo di = it->second;
            paraAi.MCH_Code = di.DeviceCode;

            //加工参数、测试项参数不为空时，证明参数不全
            string devContent = m_db.Read_TagMValue(di.devParaAlarm);
            if(!devContent.empty())
            {
                if(!di.devParaFlag)
                {
                    di.devParaFlag = true;
                    paraAi.Alarm_Content = di.Name + "加工参数报警";
                    m_db.Write_TagMValue(m_plcCommERR, "1");
                    saveAlarmInfoToSql(paraAi);
                    _log.LOG_DEBUG("AlarmDriver 【%s】 加工参数【报警】",di.Name.data());
                    it->second = di;
                }
            }

            string testContent = m_db.Read_TagMValue(di.testParaAlarm);
            if(!testContent.empty())
            {
                if(!di.testParaFlag)
                {
                    di.testParaFlag = true;
                    paraAi.Alarm_Content = di.Name + "测试项参数报警";
                    m_db.Write_TagMValue(m_plcCommERR, "1");
                    saveAlarmInfoToSql(paraAi);
                    _log.LOG_DEBUG("AlarmDriver 【%s】 测试项参数【报警】",di.Name.data());
                    it->second = di;
                }
            }
        }
    }
}

/**
 * @brief AlarmDriver::deviceInitAlarm                  设备初始化失败报警（烧程工位、电流测试工位）
 * @param devAi
 */
void AlarmDriver::deviceInitAlarm(AlarmInfo &devAi)
{
    int starflag = TcpWorkThread::GetStartFlag();                   //用于控制AlarmDriver线程循环时是否操作gLine设备数据单元，为0，设备驱动未启动，不操作gLine设备数据结构体。为1，设备驱动启动，可以操作gLine设备数据结构体。
    if(starflag == 1)
    {
        for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++)
        {
            DeviceInfo di = it->second;
            devAi.MCH_Code = di.DeviceCode;

            //设备初始化失败（成功）
            string devInit = m_db.Read_TagMValue(di.devInitAlarm);

    //        cout<<"1-- name = "<<di.Name<<" ---- devInitFlag = "<<di.devInitFlag<<endl;
            if(!devInit.empty())
            {
                if(!di.devInitFlag)
                {
                    di.devInitFlag = true;
    //                cout<<"2-- name = "<<di.Name<<" ---- devInitFlag = "<<di.devInitFlag<<endl;
                    devAi.Alarm_Content = di.Name + "初始化失败";
                    m_db.Write_TagMValue(m_plcCommERR, "1");
                    saveAlarmInfoToSql(devAi);
                    _log.LOG_DEBUG("AlarmDriver 【%s】 设备初始化失败【报警】",di.Name.data());
                    it->second = di;
                }
            }
        }
    }
}


/**
 * @brief AlarmDriver::tcpServerAlarm           Tcp上传上位机报警（上位机界面显示）
 */
void AlarmDriver::tcpServerAlarm()
{
    //gLine.Si.alarmStatus值为“0”报警
    alarmFlag = true;
    if(m_db.Read_TagMValue("PLcAlarm") == "0")
    {
        m_db.Write_TagMValue(gLine.Si.alarmStatus, "0");
    }
    else
    {
        if(!m_deviceAlarmName.empty())
        {
            for(int i = 0;i < m_deviceAlarmName.size();i++)
            {
                if(m_db.Read_TagMValue(m_deviceAlarmName.at(i)) == "0")
                {
                    alarmFlag = false;
                    m_db.Write_TagMValue(gLine.Si.alarmStatus, "0");
                    break;
                }

                if(i == m_deviceAlarmName.size() - 1 && alarmFlag)
                {
                    m_db.Write_TagMValue(gLine.Si.alarmStatus, "1");
                }
             }
         }
    }
}


/**
 * @brief AlarmDriver::initDevicePara
 */
void AlarmDriver::initDevicePara()
{
    devParaAi.Alarm_Category = "设备参数报警";
    devParaAi.Alarm_Processing_Method = "1";
}


/**
 * @brief AlarmDriver::initCommPara
 */
void AlarmDriver::initCommPara()
{
    comAi.Alarm_Category = "通信报警";
}

/**
 * @brief AlarmDriver::initDevice
 */
void AlarmDriver::initDeviceError()
{
    devInitAi.Alarm_Category = "设备初始化失败报警";
    devInitAi.Alarm_Processing_Method = "1";

}


/**
 * @brief AlarmDriver::initPlcPara
 */
void AlarmDriver::initPlcPara()
{
    for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++)
    {
        DeviceInfo di = it->second;
        m_deviceAlarmName.push_back(di.Alarm);
        if(di.Name == "1#PLC")
        {
             plcAi.MCH_Code = di.DeviceCode;
             m_plcCommOK = di.communicateOk;           //PLC报警点位(MCD模式)通信恢复
             m_plcCommERR = di.communicateERR;         //PLC报警点位(MCD模式)通信异常
        }
    }
    plcAi.Alarm_Category = "机台报警";
    cout<<"---- m_plcCommOK = "<<m_plcCommOK<<" --- m_plcCommERR = "<<m_plcCommERR<<endl;
}




/**
 * @brief AlarmDriver::initPara
 */
void AlarmDriver::initPara()
{
    initCommPara();
    initPlcPara();
    initDevicePara();
    initDeviceError();
}

void AlarmDriver::threadprocess()
{
    _log.LOG_INFO("AlarmDriver 【开始加载设备报警线程】 ......");    
    initPara();

    while(1)
    {
        //设备通信报警
        communicateAlarm(comAi);
        //PLC点位报警
        plcPointAlarm(plcAi);
        //设备参数报警
        deviceParaAlarm(devParaAi);
        //设备初始化报警
        deviceInitAlarm(devInitAi);
        //上位机报警
        tcpServerAlarm();

        usleep(1000*1000);
    }
}

/**
 * @brief AlarmDriver::Start_Thread           启动逻辑驱动线程处理函数
 * @param arg                                   对象指针
 */
void *AlarmDriver::Start_Thread(void* arg)
{
    AlarmDriver *th = (AlarmDriver*)arg;
    th->threadprocess();
    return NULL;
}


/**
 * @brief AlarmDriver::Start                  启动逻辑驱动线程
 * @return
 */
bool AlarmDriver::Start()
{
    int ret = pthread_create(&pth,NULL,AlarmDriver::Start_Thread,this);
    if(ret != 0)
    {
        return false;
    }
        return true;
}


/**
 * @brief AlarmDriver::Stop                   驱动停止
 * @return
 */
bool AlarmDriver::Stop()
{
    pthread_detach(pth);
    pthread_cancel(pth);
    return true;
}


/**
 * @brief AlarmDriver::checkDevPara
 * @param paraAi
 * @param di
 */
void AlarmDriver::checkDevPara(AlarmInfo &paraAi, DeviceInfo &di)
{
    for(int i=0;i<di.parameterNoInfo.size();i++)
    {
//        string devName = di.Name + di.parameterNoInfo.at(i);
        string devContent = m_db.Read_TagMValue(di.devParaAlarm);
        if(!devContent.empty())
        {
            devParaAi.Alarm_Category = "加工参数报警";
            paraAi.Alarm_Content = di.Name + devContent;
            m_db.Write_TagMValue(m_plcCommERR, "1");
            saveAlarmInfoToSql(paraAi);
        }
    }
}

/**
 * @brief AlarmDriver::checkDevTestPara
 * @param paraAi
 * @param di
 */
void AlarmDriver::checkDevTestPara(AlarmInfo &paraAi, DeviceInfo &di)
{
    //循环遍历每个设备的测试项参数，包括（Judge_Enable、Standard_Value、Max_Value、Min_Value、Deviation、Min_PassRate、Max_Limit）
    for(int j=0;j<di.testItemCode.size();j++)
    {
        //标准值
        string StandardValue = m_db.Read_TagMValue(di.Name + di.testItemCode.at(j) + "SV");
        if(!StandardValue.empty())
        {
            devParaAi.Alarm_Category = "测试项参数报警";
            paraAi.Alarm_Content = di.Name + StandardValue;
            m_db.Write_TagMValue(m_plcCommERR, "1");
            saveAlarmInfoToSql(paraAi);
        }
        //最大值
        string MaxValue = m_db.Read_TagMValue(di.Name + di.testItemCode.at(j) + "MAV");
        if(!MaxValue.empty())
        {
            devParaAi.Alarm_Category = "测试项参数报警";
            paraAi.Alarm_Content = di.Name + MaxValue;
            m_db.Write_TagMValue(m_plcCommERR, "1");
            saveAlarmInfoToSql(paraAi);
        }
        //最小值
        string MinValue = m_db.Read_TagMValue(di.Name + di.testItemCode.at(j) + "MIV");
        if(!MinValue.empty())
        {
            devParaAi.Alarm_Category = "测试项参数报警";
            paraAi.Alarm_Content = di.Name + MinValue;
            m_db.Write_TagMValue(m_plcCommERR, "1");
            saveAlarmInfoToSql(paraAi);
        }
        //偏差范围
        string Deviation = m_db.Read_TagMValue(di.Name + di.testItemCode.at(j) + "DT");
        if(!Deviation.empty())
        {
            devParaAi.Alarm_Category = "测试项参数报警";
            paraAi.Alarm_Content = di.Name + Deviation;
            m_db.Write_TagMValue(m_plcCommERR, "1");
            saveAlarmInfoToSql(paraAi);
        }
        //最低合格率
        string MinPassRate = m_db.Read_TagMValue(di.Name + di.testItemCode.at(j) + "MP");
        if(!MinPassRate.empty())
        {
            devParaAi.Alarm_Category = "测试项参数报警";
            paraAi.Alarm_Content = di.Name + MinPassRate;
            m_db.Write_TagMValue(m_plcCommERR, "1");
            saveAlarmInfoToSql(paraAi);
        }
        //判定使能
        string JudgeEnable = m_db.Read_TagMValue(di.Name + di.testItemCode.at(j) + "JE");
        if(!JudgeEnable.empty())
        {
            devParaAi.Alarm_Category = "测试项参数报警";
            paraAi.Alarm_Content = di.Name + JudgeEnable;
            m_db.Write_TagMValue(m_plcCommERR, "1");
            saveAlarmInfoToSql(paraAi);
        }
        string AllItem = m_db.Read_TagMValue(di.Name + di.testItemCode.at(j) + "ALL");
        if(!AllItem.empty())
        {
            devParaAi.Alarm_Category = "测试项参数报警";
            paraAi.Alarm_Content = di.Name + AllItem;
            m_db.Write_TagMValue(m_plcCommERR, "1");
            saveAlarmInfoToSql(paraAi);
        }
    }
}
