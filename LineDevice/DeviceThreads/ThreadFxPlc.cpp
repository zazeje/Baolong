#include "ThreadFxPlc.h"
#include "main.h"

vector<int> ThreadFxPlc::m_alarmAddress;
vector<int> ThreadFxPlc::m_pointValue;


/**
 * @brief 构造函数
 * @param di
 */
ThreadFxPlc::ThreadFxPlc(DeviceInfo di): DeviceThread(di)
{
    _stopprocess = false;
    ProtocolMode m_pro = dedicatedProtocol;
    myDevice  = new FxPlc(SerialPortParameters::GetSpp(_di),_di.Name,m_pro);
}

ThreadFxPlc::~ThreadFxPlc()
{
    if(myDevice != NULL)
    {
        delete myDevice;
        myDevice = NULL;
    }
}


/**
 * @brief 处理采集单元的采集点
 * @param 采集单元信息
 */
void ThreadFxPlc::ProcessUnit(UnitInfo ui)
{
    string pvs;
    //读PLC点位
    pvs = myDevice->ReadBits(ui.StartAddress, ui.Length, "M");
//    _log.LOG_DEBUG("ThreadFxPlc ProcessUnit startAdd = 【%d】 length = 【%d】 pvs = 【%s】",ui.StartAddress,ui.Length,pvs.data());

    //遍历采集单元
    for(map<string,Tag>::iterator is = ui.Tags.begin();is != ui.Tags.end();is++)
    {
        Tag tag;
        tag = is->second;
        if(tag.Name.empty())
            continue;
        string key = tag.TagName;
        tag.PlcValue = pvs.data()[tag.Address - ui.StartAddress];
        tag.MemortValue = m_db.Read_TagMValue(key);

        //处理逻辑控制模式为空的点位
        if(!tag.LogicalMode.compare("None"))
        {           
            m_db.Write_TagMValue(key,tag.PlcValue);
            //向关联控制点写1
            for (list<string>::reverse_iterator i = tag.CTagNames.rbegin();i != tag.CTagNames.rend();i++)
            {
                string cTagName = *i;
                m_db.Write_TagMValue(cTagName,tag.PlcValue);
            }

            //PLC报警，PlcValue值为"1"时，报警
            if(tag.Address == 115)
            {
                if(tag.PlcValue == "1")
                {
                    //上位机上传中，"0"代表报警，"1"代表正常
                    m_db.Write_TagMValue("PLcAlarm", "0");
//                    _log.LOG_ERROR("ThreadFxPLC 【%s】 PLC【M115】报警 ... ",_di.Name.data());
                    m_db.Write_TagMValue(_num + "$" + "NT", _di.Name + " 【M115】报警");
                }
                else if(tag.PlcValue == "0")
                {
                    m_db.Write_TagMValue("PLcAlarm", "1");
//                    _log.LOG_ERROR("ThreadFxPLC 【%s】 PLC通信正常 ... ",_di.Name.data());
                }
            }

            //PLC报警，PlcValue值为"1"时，PLC点胶干胶报警
            if(tag.Address == 118)
            {
                if(tag.PlcValue == "1")
                {
                    //上位机上传中，"0"代表报警，"1"代表正常
//                    _log.LOG_ERROR("ThreadFxPLC 【%s】 PLC【M115】报警 ... ",_di.Name.data());
                    m_db.Write_TagMValue(_num + "$" + "NT", _di.Name + " 【M118】PLC点胶干胶报警");
                }
                else if(tag.PlcValue == "0")
                {
//                    _log.LOG_ERROR("ThreadFxPLC 【%s】 PLC通信正常 ... ",_di.Name.data());
                }
            }
        }
        //处理逻辑控制模式为设备控制内存数据库的点位
        else if(!tag.LogicalMode.compare("DCM"))
        {
            if(!tag.PlcValue.compare("1"))
            {
                _log.LOG_DEBUG("ThreadFxPlc 【DCM模式】 检测到 【%s】 【%s】 地址【%d】 PLC值为1",_di.Name.data(),tag.Name.data(),tag.Address);
                m_db.Write_TagMValue(key,tag.PlcValue);
                //界面显示PLC状态
                m_db.Write_TagMValue("采集点名", tag.Name + "(M" + IntToString(tag.Address) + ")");
                //检测到PLC值为1 重置PLC值为0
                myDevice->WriteBits(tag.Address, "0", "M");
                _log.LOG_DEBUG("ThreadFxPlc 【DCM模式】 复位PLC点位【%s】 PLC地址【%d】",tag.Name.data(),tag.Address);

                //向关联控制点写1
                for (list<string>::reverse_iterator i = tag.CTagNames.rbegin();i != tag.CTagNames.rend();i++)
                {
                    string cTagName = *i;
                    m_db.Write_TagMValue(cTagName,tag.PlcValue);
//                    cout<<"---- cTagName = "<<cTagName<<" --- value = "<<tag.PlcValue<<endl;
                }

//                //清空界面(display.cpp)PLC的状态
//                if(tag.TagCode == "END")
//                {
//                    usleep(500*1000);
//                    m_db.Write_TagMValue("采集点名", "");
//                }
            }
        }
#ifdef _D2Line
        //处理逻辑控制模式为内存数据库控制设备的点位
        else if(!tag.LogicalMode.compare("MCD"))
        {
            if (!tag.MemortValue.compare("1"))
            {
                if(tag.Address != 110)
                {
                    _log.LOG_DEBUG("ThreadFxPlc 【MCD模式】 检测到 【%s】 【%s】 地址【%d】 内存数据库值为1",_di.Name.data(),tag.Name.data(),tag.Address);
                    //检测到实时库值为1 重置实时库值为0
                    m_db.Write_TagMValue(key,"0");
                }
                else
                {
                    m_db.Write_TagMValue(key,"");
                }

                //向关联控制点写1
                for (list<string>::reverse_iterator i = tag.CTagNames.rbegin();i != tag.CTagNames.rend();i++)
                {
                    string cTagName = *i;
                    m_db.Write_TagMValue(cTagName,"1");
                }
                //向PLC地址写1
                bool res = myDevice->WriteBits(tag.Address, "1", "M");
                _log.LOG_DEBUG("ThreadFxPlc 【MCD模式】 向PLC【%s】 PLC地址【%d】 置位结果为【%d】",tag.Name.data(),tag.Address,res);
            }
            //针对于D2线烧程工位、电流测试工位扫码不良置位M110
            else if(tag.Address == 110 && tag.MemortValue != "")
            {
                myDevice->WriteBits(tag.Address, tag.MemortValue, "M");
                m_db.Write_TagMValue(key,"");
                _log.LOG_DEBUG("ThreadFxPlc 【MCD模式】 向PLC【扫码不良】点位写【%s】",tag.MemortValue.data());
            }
        }
#else
        //处理逻辑控制模式为内存数据库控制设备的点位
        else if(!tag.LogicalMode.compare("MCD"))
        {
            if (!tag.MemortValue.compare("1"))
            {
                //M101为PLC心跳检测点，每隔1秒置位(不打印)
                if(tag.Address != 101)
                {
                    _log.LOG_DEBUG("ThreadFxPlc 【MCD模式】 检测到 【%s】 【%s】 地址【%d】 内存数据库值为1",_di.Name.data(),tag.Name.data(),tag.Address);
                    //检测到实时库值为1 重置实时库值为0
                    m_db.Write_TagMValue(key,"0");
                    //界面显示PLC状态
                    m_db.Write_TagMValue("采集点名", tag.Name + "(M" + IntToString(tag.Address) + ")");
                }

                //向关联控制点写1
                for (list<string>::reverse_iterator i = tag.CTagNames.rbegin();i != tag.CTagNames.rend();i++)
                {
                    string cTagName = *i;
                    m_db.Write_TagMValue(cTagName,"1");
                }
                //向PLC地址写1
                bool res = myDevice->WriteBits(tag.Address, "1", "M");
                //M101为PLC心跳检测点，每隔1秒置位(不打印)
                if(tag.Address != 101)
                    _log.LOG_DEBUG("ThreadFxPlc 【MCD模式】 向PLC【%s】 PLC地址【%d】 置位结果为【%d】",tag.Name.data(),tag.Address,res);
            }
        }
#endif
        //处理通信状态点位
        if(!tag.TagCode.compare("CS"))
        {
            if(_counter % 10 == 0)
            {
                _counter = 1;
                _connectstatus = myDevice->CanAcess() ? "1" : "0";
                m_db.Write_TagMValue(_di.Alarm, _connectstatus);
                //PLC心跳检测
                m_db.Write_TagMValue(_di.beat, "1");
                if(_connectstatus == "0")
                {
                    _log.LOG_ERROR("ThreadFxPlc 【%s】 通信检测【失败】",_di.Name.data());
                    m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
                }
                else
                {
                    m_db.Write_TagMValue(_num + "$" + "NT","");
//                    _log.LOG_DEBUG("ThreadFxPlc 【%s】 通信检测【正常】",_di.Name.data());
                }
            }
        }

        //添加D2线功率测试站，样件模式给信号到PLC
        if(g_dbInfo.GetWorkCenterNo() == "3117")
        {
            if(EnableSampleMode == 1)
                m_db.Write_TagMValue("3117$E0000050$YJ","1");
            else
                m_db.Write_TagMValue("3117$E0000050$YJ","0");
        }
    }
}

/**
 * @brief PLC驱动线程处理函数
 */
void ThreadFxPlc::threadprocess()
{
    _counter = 0;//用于模10然后与PLC通讯一次，看看PLC通讯连接是否正常。
    _log.LOG_DEBUG("ThreadFxPlc 【%s】线程驱动已启动......",_di.Name.data());
    m_alarmAddress = sortAlarmPlc(PlcAlarmVec);//排序PlcPointInfo.csv中的PLC报警点列表，main.cpp下的initCsv()内的ParseCsv::GetPlcVec(vector<PlcPointInfo> PlcInfo)
    //按寄存器地址排序，用于后续获取报警点计算点位读取区段
    if(!m_alarmAddress.empty())
        _log.LOG_DEBUG("ThreadFxPlc 【%s】 sortAlarmPlc startaddress=【%d】 size=【%d】",_di.Name.data(),m_alarmAddress.at(0),m_alarmAddress.size());
    else
        _log.LOG_ERROR("ThreadFxPlc 【%s】 sortAlarmPlc alarmAddress值【为空】",_di.Name.data());

//    for(int i=0;i<m_alarmAddress.size();i++)
//    {
//        cout<<"----- ThreadFxPlc address = "<<m_alarmAddress[i]<<endl;
//    }


    //得到设备编号
    _num = _di.Name.substr(0,1);//获取PLC的编号


    while (!_stopprocess)
    {
        for (map<string,UnitInfo>::reverse_iterator it = _di.Units.rbegin();it != _di.Units.rend();it++)
        {
            UnitInfo unit = it->second;
            //根据PLC指令最短间隔时间，设置此处延迟时间
            ProcessUnit(unit);
            usleep(35 * 1000);
            readAlarmPlc();//用于读取plc报警点位值，以便在AlarmDriver中循环比对报警值是否改变，改变即用于界面显示和插入数据库
        }
        _counter++;//用于模10然后与PLC通讯一次，看看PLC通讯连接是否正常。
    }
    _log.LOG_DEBUG("ThreadFxPlc 【%s】线程驱动已停止......",_di.Name.data());
}

/**
 * @brief PLC驱动线程处理接口
 * @param arg
 * @return
 */
void *ThreadFxPlc::Start_Thread(void* arg)
{
    ThreadFxPlc *th = (ThreadFxPlc*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief 启动PLC驱动线程
 * @return
 */
bool ThreadFxPlc::Start()
{
    pthread_attr_t pth_attr;
    pthread_attr_init(&pth_attr);
    pthread_attr_setdetachstate(&pth_attr,PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&pth,&pth_attr,ThreadFxPlc::Start_Thread,this);
    if(ret != 0)
        return false;
    else
        return true;
}

/**
 * @brief 关闭PLC驱动线程
 * @return
 */
bool ThreadFxPlc::Stop()
{
    _stopprocess = true;
    myDevice->Close();
    return true;
}

/**
 * @brief ThreadFxPlc::sortAlarmPlc             PLC点位排序
 * @param PlcAlarmVec                           保存报警点的数组
 * @return                                      返回一个顺序排列的数组
 */
vector<int> ThreadFxPlc::sortAlarmPlc(vector<PlcPointInfo> PlcAlarmVec)
{
    vector<int> vec;
    for(int i=0;i<PlcAlarmVec.size();i++)
        vec.push_back(PlcAlarmVec[i].m_registerAddr);

    if(!PlcAlarmVec.empty())
    {
        sort(vec.begin(), vec.end());
    }
    return vec;
}

/**
 * @brief ThreadFxPlc::readAlarmPlc         批量读取PLC报警点
 */
void ThreadFxPlc::readAlarmPlc()
{
    vector<int> mv;
    if(!m_alarmAddress.empty())
    {
        int startAddress = m_alarmAddress[0];
        int length = m_alarmAddress[m_alarmAddress.size() -1] - m_alarmAddress[0] + 1;
        string value = myDevice->ReadBits(startAddress, length, "M");
//        _log.LOG_DEBUG("ThreadFxPlc 【%s】 readAlarmPlc startAddress=【%d】 length=【%d】 value=【%s】",_di.Name.data(),startAddress,length,value.data());
        mv = binaryStrToIntVec(value);
        setPlcAlarmValue(mv);
    }
}

void ThreadFxPlc::setPlcAlarmValue(vector<int> pointValue)
{
    m_pointValue = pointValue;//与报警线程同时读写，vector是否主要加锁操作
}

vector<int> ThreadFxPlc::getPlcAlarmValue()
{
    return m_pointValue;
}

