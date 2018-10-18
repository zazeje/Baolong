#include "main.h"
#include "DataHelper/LocalLineInfo.h"

string mainVersion = "V1.0.56";
string subVersion = ".20180912";
LineInfo gLine;
string dispatchNo;                          //派工单号
string partNo;                              //件号
string operationNo;                         //工序号
string user;                                //用户名
string currentTime;                         //系统时间
string productNumber;                       //产品个数
string projectNo;                           //产品型号
string workCenterNo;
string currEditPartNo;

string gFullPathSAP;           //Sap文件在文件服务器的文件路径
string gLocalPathSAP;           //Sap文件在PBOX上的文件路径

int lastSocket = 0;                         //上位机最新连接的socket网口
int productnum = 0;                         //当前机台能一次同时测试的产品数量
int sockAry[ArrayBuffLen] = {0};
StartCmdInfo startCmdInfoBuff[ArrayBuffLen];
int sockIndex = 0;
int startCmdIndex = -1;
int exitFlag = 0;                           //通知daemonthread退出的标志
int onlydispatchChanged = 0;                //件号未切换（不需要重启设备驱动），但派工单号被切换，用于通知界面单独刷新派工单号。
string addressPC;                           //上位机IP地址
int    WorkModeNotify = 0;                  //WorkModeNotify:为0表示通知使用网络模式，使用上位机启动生产；
                                            //为1表示通知使用本地模式，当前状态为停止生产。
                                            //为2表示通知使用本地模式，当前状态为启动生产。
int    WorkModeNotifyshadow = 0;            //WorkModeNotifyshadow:用于WorkModeNotify的值在线程间交换变量，0表示当前切换到了网络模式；
                                            //为1表示当前切换到了本地模式，当前状态为停止生产状态。
                                            //为2表示当前切换到了本地模式，当前状态为启动生产状态。
int EnableSampleMode = 0;                   //EnableSampleMode:为0表示非样件模式；为1表示样件模式
pthread_rwlock_t rwlockPartNoDispatchNo;//件号与派工单号读写锁，用于在本地生产启动界面线程与开启本地生产线程中同步件号与派工单号字符串；
pthread_rwlock_t rwlockpcStartCmdParameter;//上位机start命令参数同步的读写锁。
pthread_mutex_t mutexPcConnectSocket;//用于上位机连接TCP服务器的Socket同步的互斥锁。
pthread_mutex_t mutexDeviceDriverCond = PTHREAD_MUTEX_INITIALIZER;//用于设备驱动运行条件判断互斥锁。
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
TcpServer *server = NULL;
TcpWorkThread *workthread = NULL;

LogHelper m_log;
LogFile _log;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(!_log.LogInit())
        return 0;

    if(!g_dbInfo.Init())
        return 0;
#if 1
    if(!initCsv())
        return 0;

    if(!initSignal())
        return 0;

    if(!initWorkThread())
        return 0;

    if(!initSocketThread())
        return 0;
#endif
//    localTest();

//    devTest();

    _log.LOG_DEBUG("*****************版本号为：【%s】*****************",mainVersion.data());

    mainWidget w;
    w.show();
    w.showFullScreen();

    return a.exec();
}


void localTest()
{
    dispatchNo="361709080018";
    partNo = "96216708";
    productNumber = "2";
    user="admin";

    ParseLineInfo parse;
    gLine = parse.parseLineInformation(partNo,g_dbInfo.GetWorkCenterNo());

    _log.LOG_DEBUG("【%s】工位开始运行 .... ",gLine.Si.Name.data());

    DeviceDriver* dd;
    dd = new DeviceDriver(gLine);
    dd->Start();

    LogicalDriver *ld;
    ld = new LogicalDriver();
    ld->Start();

    display::SetUpdateFlag(true);

    AlarmDriver *ad;
    ad = new AlarmDriver();
    ad->Start();
}


bool initCsv()
{
    ParseCsv parseCsv;
    return parseCsv.GetPlcVec(parseCsv.ReadCsvFile("./PlcPointInfo.csv"));
}

bool initSignal()
{
    int res;
    sigset_t signal_mask;
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGPIPE);
    res = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
    if(res !=0)
    {
        _log.LOG_DEBUG("main initSignal error");
        return false;
    }
    return true;
}

/**
 * @brief initWorkThread 读取本地上一次的生产启动参数，初始化件号派工单号的读写锁，启动命令的读写锁，创建workthread对象并启动
 * @return 成功返true,失败返false
 */
bool initWorkThread()
{
    g_locallineInfo.LoadLineInfo();
    pthread_rwlock_init(&rwlockPartNoDispatchNo,NULL);
    pthread_rwlock_init(&rwlockpcStartCmdParameter,NULL);
    workthread = new TcpWorkThread();
    if(!(workthread->Start()))
    {
        _log.LOG_DEBUG("main::WorkThread start error");
        return false;
    }
    return true;
}

/**
 * @brief initSocketThread 创建TcpServer对象并初始化，启动SocketRecvSend线程和SocketAccept线程
 * @return 成功返true,失败返false
 */
bool initSocketThread()
{
    if(!(initTcpServer()))
    {
        return false;
    }
    if(!(server->StartSocketRecvSend()))
    {
        return false;
    }
    if(!(server->StartAccept()))
    {
        return false;
    }
    return true;
}

/**
 * @brief initTcpServer 初始化socket连接交换的互斥锁和启动命令参数的缓冲区，获取IP地址，创建TcpServer对象。
 * @return 成功返true,失败返false
 */
bool initTcpServer()
{
    pthread_mutex_init(&mutexPcConnectSocket,NULL);
    initStartCmdBuffer();
    string ip = getPBoxEthip(1);
    server = new TcpServer(5000,ip);
    if(server == NULL)
    {
        _log.LOG_DEBUG("main::server new error");
        return false;
    }
    return true;
}

/**
 * @brief initStartCmdBuffer 初始化启动命令参数的缓冲区
 */
void initStartCmdBuffer()
{
    for(int i = 0;i < ArrayBuffLen;i++)
    {
        startCmdInfoBuff[i].init();
    }
}

void initWatchDog()
{
    pthread_t watchDogPth;
    if(pthread_create(&watchDogPth,NULL,StartWatchDog,NULL) == -1)
    {
        _log.LOG_DEBUG("Main initWatchDog error");
    }
}

void *StartWatchDog(void* arg)
{
    WatchDog* m_dogHelper = WatchDog::instance();
    m_dogHelper->init(10,2);
    m_dogHelper->run();
}



void devTest()
{

#if 0
    SerialPortParameters* spp = new SerialPortParameters("COM1",9600,8,1,0);
    Micrometer* mydevice = new Micrometer(spp);

    while(1)
    {
        usleep(500 * 1000);
        mydevice->WriteAscii("hello world");
    }
#endif

#if 0
    char data[50] = {'O','K','\n','Q','Y','1','1','0','4','\n','0','A','4','5','6','7','8','9','\n','2','0','1','7','0','6','3','0','\n'};
    vector<string> vec;
    splittostring(data,'\n',vec);
    cout << "vec.size = " << vec.size() << endl;
    for(int i = 0;i < vec.size();i++)
    {
        cout << vec[i] << " ";
    }
    cout << endl;
#endif
#if 0

    MarkViewCheck* m_device = new MarkViewCheck(9600,"192.168.10.13");
    while(1)
    {
//        string res = "";
        vector<string> data;
        data = m_device->GetCheckResult();
//        splittostring(res,'\r',data);
//        for(int i = 0;i < data.size();i++)
//        {
//            printf("data[%d] = %s\n",i,data[i].data());
//        }
//        cout << "GetCheckResult  res = " << res << endl;
        sleep(3);
    }


#endif



#if 0
    //校验位为Odd           PLC5U
    SerialPortParameters *spp = new SerialPortParameters("COM2",9600,7,1,1);
    Fx5U *mydevice = new Fx5U(spp,1,"PLC");
     while(1)
     {
//         string res = mydevice->ReadBits(110,800,"M");
//         cout<<"----- ReadBits res = "<<res<<endl;
         sleep(1);
         vector<int> res = mydevice->ReadWord(1060,3);
         for(int i = 0;i < res.size();i++)
         {
             printf("read word res[%d] = %d\n",i,res[i]);
         }
     }
#if 0
    string res = mydevice->ReadBitBy232(100,10);
    cout<<"#### res = "<<res<<endl;

    mydevice->WriteBitBy232(100,"1111111111");

    res = mydevice->ReadBitBy232(100,10);
    cout<<"#### res1 = "<<res<<endl;

    mydevice->WriteBitBy232(100,"0000000000");

    res = mydevice->ReadBitBy232(100,10);
    cout<<"#### res2 = "<<res<<endl;
#endif

#if 0
    vector<int> res;
//    vector<int> res = mydevice->ReadWordBy232(100,10,FOUR);
//    for(int i=0;i<res.size();i++)
//    {
//        cout<<"###1 res = "<<res[i]<<endl;
//    }

    vector<int> value(10,100000);
    bool flag = mydevice->WriteWordBy232(100,value,FOUR);
    cout<<"-- flag = "<<flag<<endl;

    res = mydevice->ReadWordBy232(100,10,FOUR);
    for(int i=0;i<res.size();i++)
    {
        cout<<"###2 res = "<<res[i]<<endl;
    }
//    vector<int> value1(10,100);
//    mydevice->WriteWordBy232(100,value1,4);

//    res = mydevice->ReadWordBy232(100,10,4);
//    for(int i=0;i<res.size();i++)
//    {
//        cout<<"###3 res = "<<res[i]<<endl;
//    }
#endif

#endif


#if 0
/********************************RS232 BIT WORD 测试*********************************/
    //校验位为Odd
    SerialPortParameters *spp = new SerialPortParameters("COM3",9600,7,1,1);
    FxPlc *mydevice = new FxPlc(spp,"PLC",dedicatedProtocol);
    bool flag;

    while(1)
    {
        flag = mydevice->CanAcess();
//        cout<<"----- CanAcess flag = "<<flag<<endl;
        _log.LOG_DEBUG("----- CanAcess flag = %d",flag);
        sleep(1);
    }


#if 0
    flag = mydevice->WriteBitM(5,"1111111111");
    cout<<"------ flag = "<<flag<<endl;
    sleep(1);
    string str = mydevice->ReadBitM(5,10);
    cout << "---- str = "<<str<<endl;
    cout << "---- length = "<<str.length()<<endl;
#endif

#if 0
    vector<int> value(10,1);            //初始化10个1
    flag = mydevice->WriteWord(10,value);
    cout<<"------ flag = "<<flag<<endl;
    sleep(1);
    vector<int> str = mydevice->ReadWord(10,20);
    for(int i=0;i<str.size();i++)
    {
        printf("----- str[%d] = %d ----\n",i,str[i]);
    }

#endif
/********************************RS232 BIT WORD 测试*********************************/
#endif


#if 0
    AgilentMultimeterTcp *mydevice1 = new AgilentMultimeterTcp(5025,"192.168.10.12");
    AgilentMultimeterTcp *mydevice2 = new AgilentMultimeterTcp(5025,"192.168.10.13");
    AgilentMultimeterTcp *mydevice3 = new AgilentMultimeterTcp(5025,"192.168.10.14");
    AgilentMultimeterTcp *mydevice4 = new AgilentMultimeterTcp(5025,"192.168.10.15");

#if 0
    //批量读取
    mydevice1->SendCommand(":FORM:ELEM:SENS CURR\n");
    mydevice1->SendCommand(":SENS:CURR:RANG:UPP 2E-5\n");
    sleep(2);
    mydevice1->SetWorkModeByBatch();
    sleep(2);
    _log.LOG_DEBUG("--- GetValueByBatch --- ");
    while(1)
    {
        string value = mydevice1->GetValueByBatch();
//        string value = mydevice1->GetValue();
        _log.LOG_DEBUG("value = %s",value.data());
        usleep(50*1000);
    }
#endif


      mydevice1->SendCommand(":FORM:ELEM:SENS CURR\n ");
      mydevice2->SendCommand(":FORM:ELEM:SENS CURR\n");
      mydevice3->SendCommand(":FORM:ELEM:SENS CURR\n");
      mydevice4->SendCommand(":FORM:ELEM:SENS CURR\n");



    //设置静态发射电流
    flag = mydevice1->SendCommand(":SENS:CURR:RANG:UPP 2E-2\n");
    flag = mydevice2->SendCommand(":SENS:CURR:RANG:UPP 2E-2\n");
    flag = mydevice3->SendCommand(":SENS:CURR:RANG:UPP 2E-2\n");
    flag = mydevice4->SendCommand(":SENS:CURR:RANG:UPP 2E-2\n");

    while(1)
    {
        bool flag1 = mydevice1->CanAcess();
        cout<<"CanAcess ok flag1 = "<<flag1<<endl;

        bool flag2 = mydevice2->CanAcess();
        cout<<"CanAcess ok flag2 = "<<flag2<<endl;

        bool flag3 = mydevice3->CanAcess();
        cout<<"CanAcess ok flag3 = "<<flag3<<endl;

        bool flag4 = mydevice4->CanAcess();
        cout<<"CanAcess ok flag4 = "<<flag4<<endl;

//        string value1 = mydevice1->GetValue();
//        double tmp1 = atof(value1.data());
//        string _iValue1 = DoubleToString( tmp1*1000*1000, "%.2f");
//        cout<<" --- _iValue1 = "<<_iValue1<<endl;

//        string value2 = mydevice2->GetValue();
//        double tmp2 = atof(value2.data());
//        string _iValue2 = DoubleToString( tmp2*1000*1000, "%.2f");
//        cout<<" --- _iValue2 = "<<_iValue2<<endl;

//        string value3 = mydevice3->GetValue();
//        double tmp3 = atof(value3.data());
//        string _iValue3 = DoubleToString( tmp3*1000*1000, "%.2f");
//        cout<<" --- _iValue3 = "<<_iValue3<<endl;

//        string value4 = mydevice4->GetValue();
//        double tmp4 = atof(value4.data());
//        string _iValue4 = DoubleToString( tmp4*1000*1000, "%.2f");
//        cout<<" --- _iValue4 = "<<_iValue4<<endl;

        usleep(50);
    }


#endif

#if 0
     Tektronix *mydevice = new Tektronix(9004,"192.168.10.13","Tektronix");
     DeviceInfo di;
//     while(1)
     {
        bool flag = mydevice->CanAcess();
        cout<<"--- flag = "<<flag<<endl;

        mydevice->AutoPowerJudge(di);

//        mydevice->Reset();
//        sleep(1);
//        mydevice->SetCenterFreq(433.92, "MHz");
//        mydevice->SetSpan(2, "MHz");
//        mydevice->SetRefl(-35);
//        mydevice->SetPeakExc(100);

#if 0
        mydevice->Initialize(di);
        mydevice->KeepMode();
        //测试中
        sleep(8);
        mydevice->AddMark();
        mydevice->MarkTop();
        double power1 = mydevice->ReadPower();
        cout<<"--- power = "<<power1<<endl;
        double freq1 = mydevice->ReadFreq();
        cout<<"--- freq = "<<freq1<<endl;

        sleep(1);
        mydevice->MarkNext();
        double power2 = mydevice->ReadPower();
        cout<<"--- power2 = "<<power2<<endl;
        double freq2 = mydevice->ReadFreq();
        cout<<"--- freq2 = "<<freq2<<endl;
        mydevice->DeleteMark();
        usleep(100);
#endif


     }

#endif

#if 0
    SerialPortParameters *spp = new SerialPortParameters("COM2",9600,8,2,0);
    Micrometer *mydevice = new Micrometer(spp,"Micrometer");
    while(1)
    {
        bool flag = mydevice->CanAcess();
        cout<<"--- flag = "<<flag<<endl;
//        string buff = mydevice->GetValue();
//        cout<<"-- buff = "<<buff<<endl;
        usleep(100);
    }
#endif


#if 0
    ThreadViewCheck *mydevice = new ViewCheck(1234,"192.168.10.12","mydevice");
    bool flag;
    while(1)
    {
//       flag = mydevice->CanAcess();
       string res = mydevice->GetCheckResult("1104A36F");
       cout<<"-- res = "<<res<<endl;
         sleep(1);
    }
#endif

#if 0
    CycloneProUdp *mydevice1 = new CycloneProUdp(8738,"192.168.10.12","mydevice");
    CycloneProUdp *mydevice2 = new CycloneProUdp(8738,"192.168.10.13","mydevice");
    CycloneProUdp *mydevice3 = new CycloneProUdp(8738,"192.168.10.14","mydevice");
    CycloneProUdp *mydevice4 = new CycloneProUdp(8738,"192.168.10.15","mydevice");

    bool flag,flag1,flag2,flag3,flag4;
//    flag = mydevice->DeleteAllSap();

//    flag = mydevice1->AddSapFileByName("/home/root/PBOX/SapFile/PBOX.SAP");
//    sleep(10);
//    cout<<"--- flag = "<<flag<<endl;

//    while(1)
//    {
//         flag1 = mydevice1->CanAcess();
//         mydevice->WriteProgramAndSetId("DFB2","1104A347");
         flag1 = mydevice1->AddSapFileByName("/home/root/PBOX/SapFile/PBOX.SAP");
         sleep(10);
         cout<<"flag1 = "<<flag1<<endl;

//         flag2 = mydevice2->CanAcess();
////         mydevice->WriteProgramAndSetId("DFB2","1104A347");
         flag2 = mydevice2->AddSapFileByName("/home/root/PBOX/SapFile/PBOX.SAP");
         sleep(10);
         cout<<"flag2 = "<<flag2<<endl;

//         flag3 = mydevice3->CanAcess();
////         mydevice->WriteProgramAndSetId("DFB2","1104A347");
         flag3 = mydevice3->AddSapFileByName("/home/root/PBOX/SapFile/PBOX.SAP");
         sleep(10);
         cout<<"flag3 = "<<flag3<<endl;

//         flag4 = mydevice4->CanAcess();
////         mydevice->WriteProgramAndSetId("DFB2","1104A347");
         flag4 = mydevice4->AddSapFileByName("/home/root/PBOX/SapFile/PBOX.SAP");
         sleep(10);
         cout<<"flag4 = "<<flag4<<endl;
//         sleep(1);
//    }

//    string address = "DFB2";
//    string programId = "A1010B01";
//    mydevice->WriteProgramAndSetId(address,programId);

#endif

//    LaserMarker *mydevice = new LaserMarker(1234,"192.168.10.15","mydevice");
//    mydevice->DoPrint("1104BQD8");

#if 0
//    SerialPortParameters *spp = new SerialPortParameters("COM3",9600,8,1,0);
//    TpmsHandle *mydevice = new TpmsHandle(spp,"TpmsHandle");

    TpmsHandleTcp *mydevice = new TpmsHandleTcp(4001,"192.168.10.13","TpmsHandle");

//    while(1)
    {
        mydevice->SendId("1104A36B");
        usleep(2000 * 1000);
    }
    string data;
    while(1)
    {
//        data.append(mydevice->GetData());
        data = mydevice->GetData();
        cout << "---- data = " << data << endl;
        usleep(100 * 1000);
    }
#endif

#if 0
//    SerialPortParameters *spp = new SerialPortParameters("COM3",9600,8,1,0);
//    TpmsHandle *mydevice = new TpmsHandle(spp,"TpmsHandle");

    MicrocaliperTcp *mydevice = new MicrocaliperTcp(4001,"192.168.10.12","Microcaliper");

    while(1)
    {
        string result;
       double value=0.0;
        string resultbuff = mydevice->WriteAndReadHex("010300000002C40B",70);
        if(!resultbuff.empty() && resultbuff.length() > 0)
          result =  resultbuff;
        else
           result = "";
        _log.LOG_DEBUG("MicrocaliperTcp  GetValue  result=【%s】",result.data());
        if(result.empty())
        {
            sleep(2);
            continue;
        }
        if(result.length() == 18 && !result.substr(0,6).compare("010304"))
        {
            result = result.substr(6,8);
            if(result.length() == 8)
            {
                int sign;
                if(!result.substr(0,2).compare("01"))
                    sign = -1;
                else if(!result.substr(0,2).compare("00"))
                    sign = 1;
                value = hextodec(result.substr(4,4)) * sign / 1000.0;
            }
        }
        _log.LOG_DEBUG("MicrocaliperTcp  GetValue  value=【%f】",value);
        sleep(2);
    }
#endif

#if 0
//    SerialPortParameters *spp = new SerialPortParameters("COM3",9600,8,1,0);
    Xse6Tcp *mydevice = new Xse6Tcp(4004,"192.168.10.13");
    while(1)
    {
        bool flag = mydevice->CanAcess();
        cout<<"--- flag = "<<flag<<endl;
//        double num;
//        num = mydevice->GetValue();
//        cout<<"num = "<<num<<endl;
        usleep(500*1000);
/*        mydevice->SetPower()*/;
    }
#endif


#if 0
    SerialPortParameters *spp = new SerialPortParameters("COM3",9600,8,1,0);
    TpmsHandle *mydevice = new TpmsHandle(spp,"TpmsHandle");
    while(1)
    {
        bool flag = mydevice->CanAcess();
        cout<<"flag = "<<flag<<endl;
/*        usleep(50);
        mydevice->SetPower()*/;
    }
#endif

#if 0
    SerialPortParameters *spp = new SerialPortParameters("COM3",9600,8,1,0);
    ItechPower *mydevice = new ItechPower(spp,"ItechPower");
    while(1)
    {
        bool flag = mydevice->CanAcess();
        cout<<"flag = "<<flag<<endl;
        usleep(50);
        mydevice->SetPower();
    }
#endif

#if 0
/********************************RS422 BIT WORD 测试*********************************/
    //校验位为EVEN
    SerialPortParameters *spp = new SerialPortParameters("com2",9600,7,1,2);
    FxPlc *mydevice = new FxPlc(spp,"123",programProtocol);
    while(1)
    {
        vector<int> res;
        res = mydevice->ReadWord(500,1);
        cout << "result = " << res[0] << endl;
        usleep(500 * 1000);
    }

#if 0
    vector<int> value(10,1);            //初始化10个1
    flag = mydevice->WriteWord(10,value);
    cout<<"------ flag = "<<flag<<endl;
    sleep(1);
    vector<int> str = mydevice->ReadWord(10,20);
    for(int i=0;i<str.size();i++)
    {
        printf("----- str[%d] = %d ----\n",i,str[i]);
    }

#endif

#endif


//    ThreadTpmsHandle mydevice;
//   while(1)
//   {
//       mydevice.writePressFormulaToPython();
//       double value = mydevice.CalcuatePress(20);
//       cout<<"--- value = "<<value<<endl;
//       sleep(2);
//   }


#if 0
    ParseLineInfo parse;
    gLine = parse.parseLineInformation("96216702" ,"WC0010");

//    DeviceDriver dd(gLine);
//    dd.Start();

#if 0
    printf("Station name = %s  code = %s StartFlag = %s SnFlag = %s IdFlag = %s\n",\
         gLine.Si.Name.data(),gLine.Si.Code.data(),gLine.Si.StartFlag.data(),\
           gLine.Si.SnFlag.data(),gLine.Si.IdFlag.data());

    cout<<"$$$$$$$"<<endl;
    for(map<string,DeviceInfo>::iterator iterd = gLine.Si.Dis.begin();iterd!= gLine.Si.Dis.end();++iterd)
    {
        cout<<"------"<<endl;
        DeviceInfo di = iterd->second;
        printf("\n\n");
        printf("Device name = %s -- code=%s -- deviceId=%s -- startflag=%s -- ip =%s -- port=%s -- baudRate=%s -- dataBits=%s -- stopBits=%s -- parity=%s \n",\
          di.Name.data(),di.DeviceCode.data(),di.machDriverNo.data(),di.StartFlag.data(),di.Ip.data(),di.Port.data(),di.BaudRate.data(),di.DataBits.data(),di.StopBits.data(),di.Parity.data());

        //获得参数编码
        for(int j=0;j<di.parameterNo.size();j++)
        {
            printf(" -- parameterNo[%d] = %s -- parameter[%d] = %s ",\
                   j,di.parameterNo.at(j).data(),j,di.parameter.at(j).data());
        }
        //获得测试项信息
        for(int i=0; i<di.testItemCode.size();i++)
        {
printf(" -- testItemCode[%d] = %s -- judgeEneble[%d] = %d -- standardValue[%d] = %f -- maxValue[%d] = %f -- minValue[%d] = %f -- deviation[%d] = %f -- minPassrate[%d] = %d  -- maxLimit[%d] = %d\n"\
       ,i,di.testItemCode.at(i).data(),i,di.judgeEneble.at(i),\
       i,di.standardValue.at(i),i,di.maxValue.at(i),i,di.minValue.at(i),\
       i,di.deviation.at(i),i,di.minPassrate.at(i),i,di.maxLimit.at(i));
        }

        for(map<string,UnitInfo>::reverse_iterator iteru = di.Units.rbegin();iteru!= di.Units.rend();++iteru)
        {
            UnitInfo ui = iteru->second;
            printf("Unit name=%s enable=%d startadd=%d length=%d\n",\
              ui.Name.data(),ui.Enable,ui.StartAddress,ui.Length);
            for(map<string,Tag>::reverse_iterator itert = ui.Tags.rbegin();itert!= ui.Tags.rend();++itert)
            {
                Tag tag = itert->second;
                printf("Tag name=%s desc=%s address=%d tv.name=%s tagcode=%s logicalMode=%s ControlTagCount=%d\n",\
                       tag.Name.data(),tag.Description.data(),tag.Address,tag.TagName.data(),tag.TagCode.data(),tag.LogicalMode.data(),tag.ControlTagCount);
            }
            printf("\n");
        }
    }
#endif

//    DeviceDriver *dd = new DeviceDriver(li);
//    dd->Start();

#endif




//    ParseExcel parseExcel;
//    parseExcel.GetPlcVec(parseExcel.ParseExcelFile("./PLC项目信息模板.xls"));

#if 0
    ParseLineInfo line;

    vector<MachineInfo> machineInfo = line.getMachineInfo("WC0002");
    for(int i=0;i<machineInfo.size();i++)
    {
        printf("---- machCode[%d] = %s ",i,machineInfo.at(i).machCode.data());
        printf("machDesc[%d] = %s ",i,machineInfo.at(i).machDesc.data());
        printf(" machDriverNo[%d] = %s ",i,machineInfo.at(i).machDriverNo.data());
        printf(" type[%d] = %s ",i,machineInfo.at(i).type.data());
        printf(" ipAddress[%d] = %s ",i,machineInfo.at(i).ipAddress.data());
        printf(" port[%d] = %s ",i,machineInfo.at(i).port.data());
        printf(" baudrate[%d] = %s ",i,machineInfo.at(i).baudrate.data());
        printf(" databits[%d] = %s ",i,machineInfo.at(i).databits.data());
        printf(" stopbits[%d] = %s ",i,machineInfo.at(i).stopbits.data());
        printf(" parity[%d] = %s ",i,machineInfo.at(i).parity.data());
        printf(" enable[%d] = %s ----\n",i,machineInfo.at(i).enable.data());
    }

    vector<MachParameterInfo> machParameterInfo = line.getMachineParameterInfo("96216701",  "WC0002");
    for(int i=0;i<machParameterInfo.size();i++)
    {
        printf("-----MachParameterInfo  machCode[%d] = %s",i,machParameterInfo.at(i).machCode.data());
        printf(" parameterNo[%d] = %s ",i,machParameterInfo.at(i).parameterNo.data());
        printf(" standardValue[%d] = %s ----- \n",i,machParameterInfo.at(i).standardValue.data());
    }

    vector<MachTestItemInfo> machTestItemInfo = line.getMachineTestItemInfo("96216701",  "WC0002");
    for(int i=0;i<machTestItemInfo.size();i++)
    {
        printf("----- machCode[%d] = %s",i,machTestItemInfo.at(i).machCode.data());
        printf(" testItemCode[%d] = %s ",i,machTestItemInfo.at(i).testItemCode.data());
        printf(" judgeEneble[%d] = %d ",i,machTestItemInfo.at(i).judgeEneble);
        printf(" errorCode[%d] = %s ",i,machTestItemInfo.at(i).errorCode.data());
        printf(" standardValue[%d] = %f ",i,machTestItemInfo.at(i).standardValue);
        printf(" maxValue[%d] = %f ",i,machTestItemInfo.at(i).maxValue);
        printf(" minValue[%d] = %d ",i,machTestItemInfo.at(i).minValue);
        printf(" deviation[%d] = %f ",i,machTestItemInfo.at(i).deviation);
        printf(" minPassrate[%d] = %d ",i,machTestItemInfo.at(i).minPassrate);
        printf(" maxLimit[%d] = %d ----- \n",i,machTestItemInfo.at(i).maxLimit);
    }
#endif

#if 0
    vector<PlcPointInfo> info;
    ParseCsv parseCsv;
//    parseCsv.WriteToCsvFile(info,"./PlcPointInfo.csv");
    parseCsv.GetPlcVec(parseCsv.ReadCsvFile("./PlcPointInfo.csv"));

    mainWidget w;
    w.show();
    w.showFullScreen();
//    ReadPBoxIP(&w);

#endif

#if 0
    SerialPortParameters *spp = new SerialPortParameters("COM3",9600,8,1,0);
    XSE6* myDevice = new XSE6(spp);

    while(1)
    {
        sleep(5);
        printf("CanAcess\n");
        bool result = myDevice->CanAcess();
        printf("通信检测结果为%s\n",result ? "真" : "假");
        sleep(5);
        printf("GetMaxValue\n");
        double i = myDevice->GetMaxValue();
        printf("MaxValue = %f\n",i);
        sleep(5);
        printf("GetMinValue\n");
        i = myDevice->GetMinValue();
        printf("MinValue = %f\n",i);
    }

#endif

#if 0
//    DeviceInfo di;
    Tektronix* myDevice = new Tektronix(9004,"192.168.10.13");
    while(1)
    {
        bool result = myDevice->CanAcess();
        sleep(5);
        //bool result = myDevice->CanAcess();
        printf("result = %d\n",result);
//        sleep(5);
//        double result = myDevice->ReadPower();
//        printf("Power result = %f\n",result);
//        sleep(5);
//        result = myDevice->ReadFreq();
//        printf("Freq result = %f\n",result);
    }

#endif

#if 0
    ViewCheck* myDevice = new ViewCheck(5004,"192.168.20.223");
    while(1)
    {
        sleep(5);
        bool result = myDevice->CanAcess();
        printf("通信检测结果为%s\n",result ? "真" : "假");
        sleep(5);
        string i = myDevice->GetCheckResult("0");
        printf("获取检测结果为%s\n",i == "1" ? "真" : "假");
    }

#endif

#if 0
    SerialPortParameters *spp = new SerialPortParameters("COM3",9600,8,1,0);
    SwpC80* myDevice = new SwpC80(spp);
    while(1)
    {
        sleep(2);
        printf("send message\n");
        myDevice->GetValue();
    }
#endif

#if 0
    SerialPortParameters *spp = new SerialPortParameters("COM3",9600,8,1,0);
    TpmsHandle* myDevice = new TpmsHandle(spp);
    while(1)
    {
        sleep(2);
        printf("send message\n");
        myDevice->CanAcess();
        sleep(2);
        myDevice->SendId("00");
        sleep(2);
        myDevice->GetData();
        sleep(2);
        myDevice->SendEsc();
        sleep(2);
        myDevice->SendEnter();
        sleep(2);
        myDevice->SendUp();
        sleep(2);
        myDevice->SendDown();
        sleep(2);
        myDevice->SendPower();
        sleep(2);
        myDevice->SendLight();
    }
#endif

#if 0

    while(1)
    {
        string programId;
        string command = "CALL generate_id_by_model ('FS43X1')";
//        string command;
    //    _productModel = GetProductModel();
        MYSQL_RES* result;
        MYSQL* m_connection = new MYSQL;
        MySQLHelper helper("192.168.20.11","root","tpms","flexibleline");
        helper.MySQL_Init(m_connection);
    //    command.append("CALL generate_id_by_model ('").append(_productModel).append("');");
//        command.append("CALL generate_id_by_model ('").append("FS43X1").append("');");
    //    printf("==== ThreadCyclone::AssignProgramId command %s \n",command.data());
        result = helper.LoadTable(m_connection,command);
        MYSQL_ROW query;
        query = mysql_fetch_row(result);
        //获取烧程ID
        programId = query[0];
        printf("================ programId %s =============== \n",programId.data());
        helper.MySQL_Release(result);
        helper.MySQL_Close(m_connection);
    }

#endif

#if 0
    SerialPortParameters *spp = new SerialPortParameters("com3",115200,8,1,0);
    //FxPlc *mydevice = new FxPlc(spp,"Fxplc",dedicatedProtocol);
    //mydevice->WriteRelayM(116,"11111");
    //string ret = mydevice->ReadRelayM(116,5);
    //printf("ret %s \n",ret.data());

    DataManSR *mydevice = new DataManSR(spp,"DataMan");
    //int ret = mydevice->CanAcess();
    //printf("ret %d\n",ret);
    string str = mydevice->AutoScan(3);
    printf("str %d\n",str.data());

    //AgilentMultimeterTcp *mydevice = new AgilentMultimeterTcp(5025,"192.168.127.13");
    //int ret = mydevice->SendCommand("CONF:CURR:DC 10 mA,2.0E-5");
    //printf("ret %d \n",ret);
    //sleep(3);
    //string str = mydevice->GetValue();
    //printf("str %s \n",str.data());


/*
    SerialPortParameters *spp = new SerialPortParameters("com2",9600,8,1,0);
    AgilentMultimeterSerial *mydevice = new AgilentMultimeterSerial(spp,"Agilent");
    mydevice->CanAcess();
*/
#endif

#if 0
    //ModbusDevice *modbusdevice = new ModbusDevice(502,"192.168.20.122","modbus");
    DagongPlc *dagongplc = new DagongPlc(502,"192.168.20.130","dagongplc");
    string str = dagongplc->Read(19170,10);
    printf("=== str = %s \n",str.data());
    dagongplc->Write(19170,"0");

    string str_1 = dagongplc->Read(19170,1);
    printf("== str_1 %s \n",str_1.data());
    dagongplc->Write(19175,"0");
    string str_2 = dagongplc->Read(19175,1);
    printf("== str_2 %s \n",str_2.data());

    dagongplc->Write(19179,"1");
    string str_3 = dagongplc->Read(19179,1);
    printf("== str_3 %s \n",str_3.data());

    string str_4 = dagongplc->Read(19170,10);
    printf("=== str_4 = %s \n",str_4.data());
#endif

#if 1
    ConfigLoader configloader;
//    LineInfo li =  configloader.LoadConfig("./PBOX.xml");
    configloader.test();
#endif


#if 0

    SerialPortParameters *spp = new SerialPortParameters("com2",9600,8,1,0);
    //spp->_portName = "com3";
    //spp->_baudRate = 115200;
    //spp->_dataBits = 8;
    //spp->_stopBits = 1;
    //spp->_parity = 0;
    //cout << spp->_baudRate << " " <<spp->_dataBits << " " <<spp->_parity << endl;
    AgilentMultimeterSerial *mydevice = new AgilentMultimeterSerial(spp,"001");
    bool value = mydevice->SendCommand("CONF:CURR:DC 100 mA,2.0E-5\n");
    //bool str = mydevice->CanAcess();
    printf("value = %d\n",value);
#endif



#if 0
    printf("GetSystemError\n");
    string str = mydevice->GetSystemError();
    printf("str = %s\n",str.data());
    printf("Beep\n");
    mydevice->Beep();
    printf("CanAcess\n");
    mydevice->CanAcess();
    printf("CleraError\n");
    mydevice->CleraError();
    printf("GetValue\n");
    mydevice->GetValue();
    printf("SendCommand\n");
    mydevice->SendCommand("123");
    printf("SetDisplay\n");
    mydevice->SetDisplay(true);
    printf("GetIdn\n");
    mydevice->GetIdn();
    printf("Open\n");
    mydevice->Open();
    printf("Read\n");
    for(int i = 0;i < 50;i++)
    {
        usleep(600000);
        string data = mydevice->Read();
        printf("data = %s\n",data.data());
    }
#endif

#if 0
    KeyenceSR *mydevice = new KeyenceSR(9004,"192.168.10.11");
    while(1)
    {
        bool res = mydevice->CanAcess();
//        str = barcode.append(mydevice->GetBarCode());
        cout << "-- barcode = " << res << endl;
        sleep(1);
    }
#endif
}


