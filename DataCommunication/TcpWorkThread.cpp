#include "TcpWorkThread.h"
#include "main.h"

LogicalDriver* TcpWorkThread::m_logicalDriver = NULL;
AlarmDriver* TcpWorkThread::m_alarmDriver = NULL;
DeviceDriver* TcpWorkThread::m_deviceDriver = NULL;
int TcpWorkThread::m_startFlag = 0;             //用于控制LogicalDriver线程循环时是否操作gLine设备数据单元，为0，设备驱动未启动，不操作gLine设备数据结构体。为1，设备驱动启动，可以操作gLine设备数据结构体。
string TcpWorkThread::m_manualdispatchNoNotify = "";
string TcpWorkThread::m_manualpartNoNotify = "";
string TcpWorkThread::m_manualprojectNoNotify = "";
bool TcpWorkThread::m_driverStartFlag = false;
bool TcpWorkThread::m_driverStopOkFlag = true;
bool TcpWorkThread::m_ShowDeviceListFlag = false;

/**
 * @brief TcpWorkThread::TcpWorkThread 构造函数
 */
TcpWorkThread::TcpWorkThread()
{
    m_WorkModeChangedFlag = false;           //工作模式改变标志，为true表示工作模式改变，网络模式与本地模式切换算工作模式改变，本地启停不算工作模式改变。
    m_manualStartNotifyFlag = false;         //本地模式手动启动或停止通知标志，通知手动启动为true，否则为false。
    m_lastWorkModeNotify = 0;              //用来记录上一个循环周期的PBOX的工作模式。
    m_startCmdIndexshadow = -1;
    m_WorkModeFlag = NetworkMode;          //NetworkMode表示使用网络模式，使用上位机启动生产；LocalMode表示使用本地模式，手动启动生产。
    m_WorkCenterNoshadow = "";
}

/**
 * @brief TcpWorkThread::Start 初始化工作线程并启动
 * @return 成功返true,失败返false
 */
bool TcpWorkThread::Start()
{
    WorkThreadInit();
    if(pthread_create(&m_workpth,&m_workpth_attr,TcpWorkThread::Work_Thread,(void *)this) == -1)
    {
        _log.LOG_DEBUG("TcpWorkThread::Work_Thread error");
        return false;
    }
    return true;
}

/**
 * @brief TcpWorkThread::WorkThreadInit 初始化工作线程属性
 */
void TcpWorkThread::WorkThreadInit()
{
    pthread_attr_init(&m_workpth_attr);
    pthread_attr_setdetachstate(&m_workpth_attr,PTHREAD_CREATE_DETACHED);
}

/**
 * @brief TcpWorkThread::Work_Thread
 * @param arg,TcpServer对象的指针
 */
void* TcpWorkThread::Work_Thread(void *arg)
{
    TcpWorkThread *th = (TcpWorkThread*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief TcpWorkThread::Work_Thread
 * @param arg,TcpServer对象的指针
 */
void TcpWorkThread::threadprocess()
{
    m_WorkModeChangedFlag = false;           //工作模式改变标志，为true表示工作模式改变，网络模式与本地模式切换算工作模式改变，本地启停不算工作模式改变。
    m_manualStartNotifyFlag = false;         //本地模式手动启动或停止通知标志，通知手动启动为true，否则为false。
    m_lastWorkModeNotify = 0;              //上一个循环周期的PBOX的工作模式。
    m_startCmdIndexshadow = -1;
    WorkModeNotify = 0;
    WorkModeNotifyshadow = 0;                   //WorkModeNotifyshadow:WorkModeNotify的影子变量，用线程间WorkModeNotify值的同步交换。
    m_WorkModeFlag = NetworkMode;                    //NetworkMode表示使用网络模式，使用上位机启动生产；LocalMode表示使用本地模式，手动启动生产。
    int laststartCmdIndex = -1;

    m_currstartCmdinfo.init();
    m_prevstartCmdinfo.init();
    m_WorkCenterNoshadow = g_dbInfo.GetWorkCenterNo();

    while(exitFlag == 0)
    {
        pthread_mutex_lock(&mutexDeviceDriverCond);
        if((WorkModeNotifyshadow == WorkModeNotify) && (m_startCmdIndexshadow == startCmdIndex))//启停模式和启动参数都没变，进入等待状态
        {
            pthread_cond_wait(&cond,&mutexDeviceDriverCond);
        }
        pthread_mutex_unlock(&mutexDeviceDriverCond);
        WorkModeNotifyshadow = WorkModeNotify;                                                //获取工作模式通知码并缓存到WorkModeNotifyshadow，用于本线程处理
        checkWorkModeState();

        m_startCmdIndexshadow = startCmdIndex;
        if(m_WorkModeFlag == 0)
        {
            if((m_WorkModeChangedFlag) || (m_startCmdIndexshadow != laststartCmdIndex))
            {//工作模式被改变过
                if(m_startCmdIndexshadow != -1)
                {//接收过来自网络模式下的start指令，按最新的启动参数启动设备驱动
                    DealWithNetworkStartStop(byNetwork);
                }
            }
            else
            {//模式没有改变，启动参数也没有改变，不做后续处理
                continue;
            }
        }
        else
        {
            DealWithLocalStartStop(byLocal);
        }
        if(m_WorkModeChangedFlag)
        {
            m_WorkModeChangedFlag = false;
        }
        if(m_startCmdIndexshadow != laststartCmdIndex)
        {
            if(!(startCmdInfoBuff[m_startCmdIndexshadow].partNo.empty()))
            {//网络的当前件号等参数改变后将参数保存，用于下次本地启动后初始化本地启动的相关参数的输入框
                saveLineInfotoFile();
            }
            laststartCmdIndex = m_startCmdIndexshadow;
        }
    }
    _log.LOG_DEBUG("Work_Thread StartThread 已退出！");
}

/**
 * @brief TcpWorkThread::DealWithNetworkStartStop 处理网络模式的启停
 * @param startsourceindex,用于在日志打印时分析启动设备驱动的来源
 */
void TcpWorkThread::DealWithNetworkStartStop(enum DriverSource sourceindex)
{
    m_currstartCmdinfo.setField(startCmdInfoBuff[m_startCmdIndexshadow].projectNo,startCmdInfoBuff[m_startCmdIndexshadow].dispatchNo,startCmdInfoBuff[m_startCmdIndexshadow].partNo,startCmdInfoBuff[m_startCmdIndexshadow].user,startCmdInfoBuff[m_startCmdIndexshadow].operationNo);
    if((m_prevstartCmdinfo.partNo == m_currstartCmdinfo.partNo) && (!(m_currstartCmdinfo.partNo.empty())))
    {//件号、工作中心未改变，并且件号不为空则不做后续处理
        if(m_prevstartCmdinfo.dispatchNo.compare(m_currstartCmdinfo.dispatchNo))
        {
            _log.LOG_INFO("DealWithNetworkStartStop，件号未变，派工单切换，旧dispatchNo【%s】，新dispatchNo【%s】",m_prevstartCmdinfo.dispatchNo.data(),m_currstartCmdinfo.dispatchNo.data());
            dispatchNo = m_currstartCmdinfo.dispatchNo;
            m_prevstartCmdinfo.dispatchNo = m_currstartCmdinfo.dispatchNo;
            onlydispatchChanged = 1;
        }
        return;
    }
    if(m_deviceDriver != NULL)
    {//启动参数改变，但当前驱动未停，先停设备驱动
        StopDeviceDriver(sourceindex,WorkModeNotifyshadow,partNo);
    }

    m_prevstartCmdinfo.setField(m_currstartCmdinfo.projectNo,m_currstartCmdinfo.dispatchNo,m_currstartCmdinfo.partNo);//设置projectNo，dispatchNo，partNo
    if(!(m_currstartCmdinfo.partNo.empty()))
    {//件号不为空，加载新参数，启动新参数下的设备驱动
        setLineInfoField(m_currstartCmdinfo.projectNo,m_currstartCmdinfo.dispatchNo,m_currstartCmdinfo.partNo,m_currstartCmdinfo.user,m_currstartCmdinfo.operationNo);
        StartDeviceDriver(sourceindex,WorkModeNotifyshadow,partNo,dispatchNo,projectNo,user,operationNo);
    }
}
/**
 * @brief TcpWorkThread::DealWithLocalStartStop 处理本地模式的启停
 * @param Localsource
 */
void TcpWorkThread::DealWithLocalStartStop(enum DriverSource sourceindex)
{
    if(m_manualStartNotifyFlag)
    {//本地模式的启动状态下执行
        pthread_rwlock_rdlock(&rwlockPartNoDispatchNo);
        m_currstartCmdinfo.setField(m_manualprojectNoNotify,m_manualdispatchNoNotify,m_manualpartNoNotify);
        pthread_rwlock_unlock(&rwlockPartNoDispatchNo);

        if(m_prevstartCmdinfo.partNo == m_currstartCmdinfo.partNo)
        {
            return;
        }
        m_prevstartCmdinfo.setField(m_currstartCmdinfo.projectNo,m_currstartCmdinfo.dispatchNo,m_currstartCmdinfo.partNo);//设置projectNo，dispatchNo，partNo

        setLineInfoField(m_currstartCmdinfo.projectNo,m_currstartCmdinfo.dispatchNo,m_currstartCmdinfo.partNo,g_locallineInfo.m_localUser,g_locallineInfo.m_localOperationNo);
        StartDeviceDriver(sourceindex,WorkModeNotifyshadow,partNo,dispatchNo,projectNo,user,operationNo);
    }
    else
    {//本地模式的停止状态下执行
        if(m_deviceDriver != NULL)
        {
            StopDeviceDriver(sourceindex,WorkModeNotifyshadow,partNo);
        }
    }
}

/**
 * @brief DeviceDriver启动函数
 * @param startsourceindex,用于在日志打印时分析启动设备驱动的来源
 */
void TcpWorkThread::StartDeviceDriver(enum DriverSource startsourceindex,int workmodenotifyshadow,string partno,string dispatchno,string projectno,string user,string operationno)
{
    DeviceThread::SetProductModel(projectno);
    DeviceThread::SetPartNo(partno);
    ParseLineInfo parse;
    parse.parseLineInformation(partno,m_WorkCenterNoshadow);
    if(startsourceindex == byLocal)//本地启动时工序号必须从数据库取正确的工序号
    {//如果工序号不正确将导致网络自动化生产时前工位判定不正确。
        parse.getOperationNoInfo(partNo,workCenterNo);
        if(!g_locallineInfo.m_localOperationNo.compare(""))
        {
            _log.LOG_ERROR("getOperationNoInfo  取工序号失败");
            gLine.Si.Name = "本地启动，查询数据库中工站对应工序号失败，请检查";
            operationNo = "";
        }
        else
        {
            operationNo = g_locallineInfo.m_localOperationNo;
        }
    }
    m_ShowDeviceListFlag = true;
    _log.LOG_INFO("Starting Source【%d】，WorkModeNotify【%d】，partNo【%s】，dispatchNo【%s】，projectNo【%s】，user【%s】，operationNo【%s】,productnum【%d】",
            startsourceindex,workmodenotifyshadow,partno.data(),dispatchno.data(),projectno.data(),user.data(),operationno.data(),productnum);
    m_startFlag = 1;
    fetchSapFile();
    if(m_logicalDriver == NULL)
    {
        m_logicalDriver = new LogicalDriver();
        m_logicalDriver->Start();
    }
    if(m_alarmDriver == NULL)
    {
        m_alarmDriver = new AlarmDriver();
        m_alarmDriver->Start();
    }
    if(m_deviceDriver == NULL)
    {
        m_deviceDriver = new DeviceDriver(gLine);
        if((startsourceindex == byLocal) && (!operationNo.compare("")))
        {//必须不启动，如果以错误的工序号上传了生产数据将导致自动流线生产时前工位检测不良，错误数据多时短期不好清理
            _log.LOG_ERROR("getOperationNoInfo  本地启动从数据库取工站工序号失败，驱动不启动");
        }
        else
        {
            m_deviceDriver->Start();
        }
    }
    m_driverStartFlag = true;
    usleep(400 * 1000);
}

/**
 * @brief DeviceDriver停止函数
 * @param startsourceindex,用于在日志打印时分析要停止设备驱动的起源
 */
void TcpWorkThread::StopDeviceDriver(enum DriverSource stopsourceindex,int workmodenotifyshadow,string partno)
{
    m_startFlag = 0;
    _log.LOG_INFO("Stopping Source【%d】，WorkModeNotify【%d】，partNo【%s】",stopsourceindex,workmodenotifyshadow,partno.data());
    m_ShowDeviceListFlag = false;
    m_driverStopOkFlag = false;
    m_driverStartFlag = false;
    m_deviceDriver->Stop();
    usleep(5000 * 1000);
    delete m_deviceDriver;
    usleep(1000 * 1000);
    m_deviceDriver = NULL;
    m_driverStopOkFlag = true;
    if(stopsourceindex == byLocal)
    {
        m_currstartCmdinfo.setField("","","");//清空projectNo，dispatchNo，partNo
        m_prevstartCmdinfo.setField("","","");//清空projectNo，dispatchNo，partNo
    }
}

/**
 * @brief TcpWorkThread::checkWorkModeState 解析工作模式，本地的启停通知
 */
void TcpWorkThread::checkWorkModeState()
{
    //工作模式有变化
    if(WorkModeNotifyshadow != m_lastWorkModeNotify)
    {
        //比较通知码是否改变，无改变不执行，通过通知码解析出要执行的工作模式以及手动模式下的是启动还是停止
        if(WorkModeNotifyshadow == 0)
        {
            m_WorkModeFlag = NetworkMode;           //网络模式,本地==》网络模式
            if(m_lastWorkModeNotify != 0)
            {
                m_WorkModeChangedFlag = true;
            }
            m_manualStartNotifyFlag = false;
        }
        else
        {
            m_WorkModeFlag = LocalMode;           //本地模式,网络模式==》本地
            if(m_lastWorkModeNotify == 0)
            {
                m_WorkModeChangedFlag = true;
            }
            if(WorkModeNotifyshadow == 1)
            {
                m_manualStartNotifyFlag = false;      //本地模式停止状态
            }
            else
            {
                m_manualStartNotifyFlag = true;       //本地模式启动状态
            }
        }
        m_lastWorkModeNotify = WorkModeNotifyshadow;
    }
    return;
}

/**
 * @brief TcpWorkThread::saveLineInfotoFile 保存最后一次的网络启动参数到本地文件
 */
void TcpWorkThread::saveLineInfotoFile()
{
    if((g_locallineInfo.m_localPartNo != startCmdInfoBuff[m_startCmdIndexshadow].partNo) || (g_locallineInfo.m_localDispatchNo != startCmdInfoBuff[m_startCmdIndexshadow].dispatchNo)
        || (g_locallineInfo.m_localProjectNo != startCmdInfoBuff[m_startCmdIndexshadow].projectNo))
    {
        g_locallineInfo.m_localPartNo = startCmdInfoBuff[m_startCmdIndexshadow].partNo;
        g_locallineInfo.m_localDispatchNo = startCmdInfoBuff[m_startCmdIndexshadow].dispatchNo;
        g_locallineInfo.m_localProjectNo = startCmdInfoBuff[m_startCmdIndexshadow].projectNo;
        g_locallineInfo.SaveLineInfo();
    }
}

/**
 * @brief TcpWorkThread::setLineInfoField 保存启动信息中的件号、派工单号、型号、用户名、操作号到全局变量。
 * @param projectno 型号
 * @param dispatchno 派工单号
 * @param partno 件号
 * @param username 用户名
 * @param operationno 操作号
 */
void TcpWorkThread::setLineInfoField(string projectno,string dispatchno,string partno,string username,string operationno)
{
    operationNo = operationno;
    projectNo = projectno;
    partNo = partno;
    user = username;
    dispatchNo = dispatchno;
}

/**
 * @brief TcpWorkThread::GetStartFlag
 * @return 返回设备驱动是否启动的bool标志
 */
bool TcpWorkThread::GetStartFlag()
{
    return m_startFlag;
}

/**
 * @brief TcpWorkThread::fetchSapFile 获取烧程文件
 */
void TcpWorkThread::fetchSapFile()
{
    string cmd;
    string path = "";
    gFullPathSAP = "";
    gLocalPathSAP = "";

    int nRet = GetSapFilePath(gLine,path);
    if(nRet == 0)
    {
        cmd.clear();
        cmd.append("rm ./SapFile/* -rf");
        system(cmd.data());
        cmd.clear();
        cmd.append("mount -t cifs -o username=administrator,password=123,iocharset=utf8,sec=ntlm //192.168.127.100");
        cmd.append("/SAP").append(" /tmp");
        system(cmd.data());
        _log.LOG_ERROR("mount 文件目录为【%s】",cmd.data());

        sleep(1);

        string dirPath = "/tmp/" + path;
        if(access(dirPath.data(),F_OK) == -1)
        {
            nRet = -3;
            _log.LOG_ERROR("SAP文件目录不存在! 文件目录为【%s】",dirPath.data());
        }
        else
        {
            //数据库中SAP文件路径格式
            //取到的SAP文件存放在./SapFile/文件夹下，以PBOX.SAP命名
            cmd.clear();
            cmd.append("cp ").append(dirPath).append("/* ").append("./SapFile/");
            _log.LOG_DEBUG("TcpWorkThread 拷贝SAP文件【%s】",cmd.data());
            system(cmd.data());
            DIR *dir;
            struct dirent *ptr;
            if ((dir=opendir("./SapFile/")) == NULL)
            {
                nRet = -4;
                _log.LOG_ERROR("./SapFile/目录下不存在!");
            }
            else
            {
                int sapfilenum = 0;
                string filename;
                while ((ptr=readdir(dir)) != NULL)
                {
                    if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)
                    {
                        continue;
                    }
                    else if(ptr->d_type == 8)    //file
                    {
                        sapfilenum++;
                        if(sapfilenum == 1)
                        {
                            filename = ptr->d_name;
                        }
                        _log.LOG_DEBUG("文件【%d】:【%s】",sapfilenum,(dirPath +"/" + filename).data());
                    }
                    else if(ptr->d_type == 10)    //link file
                    {
                        continue;
                    }
                    else if(ptr->d_type == 4)    //dir
                    {
                        continue;
                    }
                }
                closedir(dir);
                if(sapfilenum == 1)
                {
                    gFullPathSAP = dirPath + "/" + filename;
                    gLocalPathSAP = "./SapFile/" + filename;
                }
                else
                {
                    nRet = -5;
                    _log.LOG_ERROR("./SapFile/目录下烧程文件不存在或文件过多不明确!");
                }
            }
        }
    }
}

/**
 * @brief TcpWorkThread::GetSapFilePath 获取烧程文件路径
 * @param li,产线设备信息
 * @return path,烧程文件的路径.
 */
int TcpWorkThread::GetSapFilePath(LineInfo li,string &path)
{
    path = "";
    int nRet = 1;
    for(map<string, DeviceInfo>::iterator iterdi = li.Si.Dis.begin();iterdi != li.Si.Dis.end();iterdi++)
    {
        DeviceInfo di = iterdi->second;
        if(!di.machDriverNo.compare("2"))       //DeviceId = 2代表烧程器
        {
            if(di.parameter.size() > 0)
            {
                path = di.parameter.at(0);      //每个烧程器的烧程文件都一样
                if(path.empty())
                {
                    nRet = -1;
                    _log.LOG_ERROR("TcpWorkThread 【%s】 SAP文件路径获取【失败】,参数数组为空",di.Name.data());
                }
                else
                {
                    nRet = 0;
                    _log.LOG_DEBUG("TcpWorkThread  【%s】 数据库存储的Sap文件路径为【%s】",di.Name.data(),path.data());
                }
            }
            else
            {
                nRet = -2;
                _log.LOG_ERROR("TcpWorkThread 【%s】 SAP文件路径获取【失败】,参数为空",di.Name.data());
            }
            break;
        }
    }
    return nRet;
}
