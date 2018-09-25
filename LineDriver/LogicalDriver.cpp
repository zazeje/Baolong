#include "LogicalDriver.h"
#include "main.h"

//extern LineInfo gLine;

/**
 * @brief LogicalDriver::LogicalDriver          构造函数
 */
LogicalDriver::LogicalDriver()
{
    judgeFlag = false;
}

/**
 * @brief LogicalDriver::~LogicalDriver         析构函数
 */
LogicalDriver::~LogicalDriver()
{
}

string LogicalDriver::getPlcJR()
{
    string plcJR;
    for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++)
    {
        DeviceInfo di = it->second;
        if(di.Name == "1#PLC")
        {
            plcJR = di.JudgeResult;
        }
    }
    return plcJR;
}


void LogicalDriver::threadprocess()
{
    bool flag = true;
    string testEnd;                     //测试结束点名（xml中配置）
    string plcJR = getPlcJR();          //PLC的良品信号点（MCD）
    int starflag = 0;                   //用于控制LogicalDriver线程循环时是否操作gLine设备数据单元，为0，设备驱动未启动，不操作gLine设备数据结构体。为1，设备驱动启动，可以操作gLine设备数据结构体。

    while(flag)
    {
        usleep(50 * 1000);
        starflag = TcpWorkThread::GetStartFlag();
        if(starflag == 1)
        {
            for(int i=0; i<atoi(gLine.Si.endNum.data()); i++)
            {
                testEnd = gLine.Si.Code + "$END" + IntToString(i+1);
                if(m_db.Read_TagMValue(testEnd) == "1")
                {
                    int jrDev=0;            //参与判定设备个数（且判定值为1）
                    int nojrDev=0;          //不参与判定设备个数
                    _log.LOG_DEBUG("LogicalDriver 检测到 【测试结束信号END%d】 开始进行判定",i+1);
                    //复位测试结束点
                    m_db.Write_TagMValue(testEnd, "");
                    judgeFlag = false;
                    for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++)
                    {
                        DeviceInfo di = it->second;
    //                    _log.LOG_DEBUG("LogicalDriver name=【%s】 testItemCode size=【%d】 ",di.Name.data(),di.testItemCode.size());
    //                    _log.LOG_DEBUG("di.Name.substr=【%s】 IntToString(i+1)=【%s】",di.Name.substr(0,1).data(),IntToString(i+1).data());
                        if(di.testItemCode.size() > 0 && di.Name.substr(0,1) == IntToString(i+1))     //扫码器、PLC不参与判定
                        {
                            _log.LOG_DEBUG("LogicalDriver 【%s】设备 【%s】判定值为【%s】",di.Name.data(),di.JudgeResult.data(),m_db.Read_TagMValue(di.JudgeResult).data());
                            if(m_db.Read_TagMValue(di.JudgeResult) != "1")
                            {
                                judgeFlag = false;
                                _log.LOG_DEBUG("LogicalDriver 【%s】设备 【%s】检测为【不良品】，跳出循环",di.Name.data(),di.JudgeResult.data());
                                break;
                            }
                            else
                            {
                                jrDev++;        //参与判定设备的JR值为“1”时
                                _log.LOG_DEBUG("LogicalDriver 【%s】设备 检测为【良品】",di.Name.data());
                            }
                        }
                        else
                        {
                            nojrDev++;
                            _log.LOG_DEBUG("LogicalDriver 不参与判定设备 name=【%s】",di.Name.data());
                        }
                    }
                    _log.LOG_DEBUG("LogicalDriver 总设备个数Dev=【%d】 不参与判定个数nojrDev=【%d】 良品设备个数【%d】",g_deviceNum,nojrDev,jrDev);

                    if((jrDev == g_deviceNum - nojrDev))
                    {
                        if(jrDev == 0)      //当前工位没有参与判定的设备
                            judgeFlag = false;
                        else
                            judgeFlag = true;
                    }


                    if(judgeFlag)
                    {
                        //display.cpp 显示界面
                        m_db.Write_TagMValue(gLine.Si.JudgeResult, "1");
                        //plc 良品判定
                        m_db.Write_TagMValue(plcJR + IntToString(i+1), "1");      //判定结果
                        _log.LOG_DEBUG("LogicalDriver 【PLC】设备【%s】 写入判定结果：【良品】",(plcJR + IntToString(i+1)).data());
                        m_db.Write_TagMValue(IntToString(i+1) + "$" + "MG", "良品");
                    }
                    else
                    {
                        m_db.Write_TagMValue(gLine.Si.JudgeResult, "0");
                        m_db.Write_TagMValue(plcJR + IntToString(i+1), "0");      //判定结果
                        _log.LOG_DEBUG("LogicalDriver 【PLC】设备【%s】 写入判定结果：【不良品】",(plcJR + IntToString(i+1)).data());
                        m_db.Write_TagMValue(IntToString(i+1) + "$" + "MG", "不良品");
                    }
                }
            }
        }
    }
}


/**
 * @brief LogicalDriver::Start_Thread           启动逻辑驱动线程处理函数
 * @param arg                                   对象指针
 */
void *LogicalDriver::Start_Thread(void* arg)
{
    LogicalDriver *th = (LogicalDriver*)arg;
    th->threadprocess();
    return NULL;
}


/**
 * @brief LogicalDriver::Start                  启动逻辑驱动线程
 * @return
 */
bool LogicalDriver::Start()
{
    int ret = pthread_create(&pth,NULL,LogicalDriver::Start_Thread,this);
    if(ret != 0)
    {
        return false;
    }
        return true;
}


/**
 * @brief LogicalDriver::Stop                   驱动停止
 * @return
 */
bool LogicalDriver::Stop()
{
    pthread_detach(pth);
    pthread_cancel(pth);
    return true;
}


/**
 * @brief LogicalDriver::SaveProductInfo        向数据库上传信息
 * @return                                      存储成功返回true，否则返回false
 */
bool LogicalDriver::SaveProductInfo(PartTestItemInfo &pi)
{
#if 0
    string sql;

    pi.dispatchNo = dispatchNo;
    pi.partNo = partNo;
    pi.operationNo = operationNo;
    pi.enterUser = user;

    MYSQL* m_connection = new MYSQL;
    MySQLHelper mysql(HOSTIP,USER,PASSWD,PBOX);
    if(m_connection != NULL)
    {
        if(!mysql.MySQL_Init(m_connection))
        {
            mysql.MySQL_Close(m_connection);
            delete m_connection;
            return false;
        }
    #if 1
        sql = "insert into part_test_item_info_temp(Dispatch_No,Part_No,Part_No_Id,Part_Seq_No,Operation_No,";
        sql = sql + "MCH_Code,Test_Item_Code,Test_Item_Collect_Value,Test_Item_Eigen_Value,Test_Item_Judge_Result,Enter_User) ";
        sql = sql + "values("+"'" + (pi.dispatchNo) + "','" + (pi.partNo) + "','";
        sql = sql + (pi.partNoId) + "','" + pi.partSeqNo + "','" ;
        sql = sql + pi.operationNo + "','" + pi.machCode+ "','" + pi.testItemCode+ "','";
        sql = sql + pi.testItemCollectValue+ "','"+ pi.testItemEigenValue+ "','"+ IntToString(pi.testItemJudgeResult) + "','" + pi.enterUser+"')";
    //        sql = sql + pi.enterDate+"'" ;
    #endif
       printf("%s %s \n",_di.Name.data(), sql.data());
        m_log.WriteToLogFile("LineAuto DeviceThread SaveProductInfo sql = ",sql);
    #if 1
        //执行sql语句，插入数据库
        bool result = mysql.InsertTable(m_connection,sql);
        if(!result)
        {
            m_log.WriteToLogFile("LineAuto DeviceThread SaveProductInfo ","MySQL InsertTable Error");
            mysql.MySQL_Close(m_connection);
            delete m_connection;
            return false;
        }
        mysql.MySQL_Close(m_connection);
        delete m_connection;
        m_log.WriteToLogFile("LineAuto DeviceThread SaveProductInfo ","MySQL InsertTable finish");
    #endif
    }
    else
    {
        cout<<"MYSQL (m_connection) 初始化失败 ....."<<endl;
        return false;
    }

    return true;
#endif
}

