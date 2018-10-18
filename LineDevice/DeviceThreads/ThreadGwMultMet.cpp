#include "ThreadGwMultMet.h"

#if 0

bool ThreadGwMultMet::_stopColl = true;

ThreadGwMultMet::ThreadGwMultMet()      //固伟万用表
{
    _stopprocess = false;
    _reinitflag = false;
    devtype = null;
}

/**
 * @brief 构造函数
 * @param 设备信息
 */
ThreadGwMultMet::ThreadGwMultMet(DeviceInfo di) : DeviceThread(di)
{
    _di = di;
    devtype = null;
    pcount = 0;
    _stopColl = true;
    _reinitflag = false;
    _counter = 0;
    _stopprocess = false;

    myComDevice = new GwinstekMultimeter(SerialPortParameters::GetSpp(_di),_di.Name);
    m_log.WriteToLogFile("LineAuto ThreadGwMultMet ","GwinstekMultimeter 创建成功 " + _di.Port + "  " + _di.BaudRate + "  " + _di.DataBits + "  " + _di.StopBits + "  " + _di.Parity);
}


/**
 * @brief 析构函数
 */
ThreadGwMultMet::~ThreadGwMultMet()
{

    delete myComDevice;
    myComDevice = NULL;
}


/**
 * @brief 初始化设备(初始化量程设置)
 * @return
 */
bool ThreadGwMultMet::InitDevice()
{
     return myComDevice->SendCommand(CheckModel(_di));
}

/**
 * @brief 通过工艺代码判断安捷伦万用表工作状态
 * @param 设备信息
 * @return
 */
string ThreadGwMultMet::CheckModel(DeviceInfo &m_di)
{
    if(!m_di.DeviceCode.compare("QCM"))         //静态电流
    {
        devtype = Emitter_SA;
        return "CONF:CURR:DC 10E-5,DEF";
    }
    else if(!m_di.DeviceCode.compare("TCM"))    //发射电流
    {
        devtype = Emitter_FA;
        return "CONF:CURR:DC 10E-3,DEF";
    }
    else                                        //静态电压QVM、发射电压TVM
    {
        devtype = null;
        return "CONF:VOLT:DC 10,DEF";
    }
}

/**
 * @brief 安捷伦驱动线程处理函数
 */
void ThreadGwMultMet::threadprocess()
{
    int index = 0;
    m_log.WriteToLogFile("LineAuto.ThreadGwMultMet threadprocess ","【" + _di.Name + "】" + "线程驱动已启动......");
    printf("【%s】线程驱动已启动......\n",_di.Name.data());
    if(!InitDevice())
        return;
    pthread_t _collectThread;
    //创建采集线程
    int ret = pthread_create(&_collectThread,NULL,CollectData,this);
    if(ret != 0)
    {
        m_log.WriteToLogFile("LineAuto.ThreadGwMultMet threadprocess ","安捷伦万用表采集线程创建失败！");
        printf("安捷伦万用表采集线程创建失败！\n");
        return;
    }
    while(!_stopprocess)
    {
        usleep(100*1000);
        startFlag = m_db.Read_TagMValue(_di.StartFlag);
        //开始信号为"1"时，清空遗留信息
        if(!startFlag.empty() && !startFlag.compare("1"))
        {
            m_log.WriteToLogFile("LineAuto ThreadGwMultMet threadprocess ","检测到启动信号，清空瞬时值，特征值，复位启动信号点");
            m_db.Write_TagMValue(_di.StartFlag,"0");
            //清空瞬时值和累积值
            _iValue.clear();
            _cValue.clear();
            m_db.Write_TagMValue(_di.DeviceCode + "$" + "IV","");
            m_db.Write_TagMValue(_di.DeviceCode + "$" + "EV","");
            _updateFlag = true;
        }
        //遍历设备层级下的采集单元
        for(map<string,UnitInfo>::iterator it = _di.Units.begin();it != _di.Units.end();it++)
        {
            //遍历采集单元下的采集点
            for (map<string,Tag>::iterator im = it->second.Tags.begin();im != it->second.Tags.end();im++)
            {
                Tag kvpt = im->second;
                string code = kvpt.TagCode;
                kvpt.MemortValue= m_db.Read_TagMValue(kvpt.TagName);
                //处理开始采集信号
                if (!code.compare("SC"))
                {
                    if(!kvpt.MemortValue.compare("1"))
                    {
                        m_log.WriteToLogFile("LineAuto ThreadGwMultMet threadprocess ","【" + _di.Name + "】" + "检测到开始采集信号! ");
                        printf("【%s】检测到开始采集信号! \n",_di.Name.data());
                        m_db.Write_TagMValue(kvpt.TagName,"0");
                        _stopColl = false;
                    }
                }
                //处理停止采集信号
                else if(!code.compare("EC"))
                {
                    if(!kvpt.MemortValue.compare("1"))
                    {
                        _stopColl = true;
                        _updateFlag = false;
                        m_log.WriteToLogFile("LineAuto ThreadGwMultMet threadprocess ","【" + _di.Name + "】" + "检测到停止采集信号!");
                        printf("【%s】检测到停止采集信号！! \n",_di.Name.data());
                        pi.testItemCollectValue = _cValue;
                        //处理采集值
                        int res = 0;
                        if(_di.testItemCode.size() > 0)
                        {
                            //获得测试项条目的下标
                            index = getTestItemInfo();
//                            cout<<"--- index = "<<index<<"--- devtype = "<<devtype<<endl;
                            //处理采集值,进行判定
                            res = ProcessCValue(pi, _di, index);
                        }
                        string key = _di.DeviceCode + "$" + "EV";
                        m_db.Write_TagMValue(key,pi.testItemEigenValue);
                        m_db.Write_TagMValue(kvpt.TagName,"0");
                        if(res == 1)
                        {
                            m_log.WriteToLogFile("LineAuto ThreadGwMultMet threadprocess ","JR append 1");
                            m_db.Write_TagMValue(_di.DeviceCode + "$" + "JR","1");
                            pi.errorCode = "";
                        }
                        else if(res == 0)
                        {
                            m_log.WriteToLogFile("LineAuto ThreadGwMultMet threadprocess ","JR append 0");
                            m_db.Write_TagMValue(_di.DeviceCode + "$" + "JR","0");
                            pi.errorCode = _di.cp.ErrorCode;
                        }
                        if(_di.cp.NeedJudge == 1)
                        {
                            SaveProductInfo(pi);
                        }
                    }
                }
                //处理瞬时值
                else if(!code.compare("IV"))
                {
                    string key = _di.DeviceCode + "$" + kvpt.TagCode;
                    if(_updateFlag)m_db.Write_TagMValue(key,_iValue);
                }
                //处理累积值
                else if(!code.compare("CV"))
                {
                    string key = _di.DeviceCode + "$" + kvpt.TagCode;
                    if(_updateFlag)m_db.Write_TagMValue(key,_cValue);
                }
                //处理产品计数
                else if(!code.compare("CC"))
                {
                    if(!kvpt.MemortValue.compare("1"))
                    {
                        m_db.Write_TagMValue(kvpt.TagName,"0");
                        pcount++;
                        if(pcount >= 2747482647)
                        {
                            pcount = 0;
                        }
                        string key = _di.DeviceCode + "$" + kvpt.TagCode;
                        m_db.Write_TagMValue(key,IntToString(pcount));
                    }
                }
                //处理通信状态
                else if(!code.compare("CS"))
                {
                    //正在采集数据则通信状态点为"1"
                    if(!_stopColl)
                        _connectstatus = "1";
                    else
                    {
                        //两秒钟检测一次通信状态
                        if(_counter % 20 == 0)
                        {
                            _connectstatus = myComDevice->CanAcess() ? "1": "0";
                            //如果通信检测失败，则断开连接次数自增一次
                            if(_connectstatus == "0")
                            {
                                myComDevice->Restart_Serial();
                                disconnectFlag++;
                            }
                            //通信检测成功，则将断开连接次数归零
                            else
                            {
                                disconnectFlag = 0;
                            }
                            string key = _di.DeviceCode + "$" + "CS";
                            //如果断开连接次数大于等于5次，即连续10秒钟以上连接失败，则认为设备连接断开，向内存数据库通信点位置为0
                            if(disconnectFlag >= 5)
                            {
                                m_log.WriteToLogFile("LineAuto ThreadGwMultMet threadprocess ","连续10秒钟以上连接失败,向上位机报警");
                                m_db.Write_TagMValue(key,"0");
                            }
                            else
                            {
                                m_db.Write_TagMValue(key,"1");
                            }
                            //断线重连后重新执行设备初始化操作
                            if(_connectstatus == "0")
                            {
                                _reinitflag = true;
                            }
                            if(_reinitflag && _connectstatus == "1")
                            {
                                if(InitDevice())
                                    _reinitflag = false;
                            }
                            _counter = 1;
                        }
                    }
                }
            }
        }
        _counter++;
    }
    m_log.WriteToLogFile("LineAuto.ThreadGwMultMet threadprocess ","【" + _di.Name + "】" + "线程驱动已停止 ......");
}

/**
 * @brief 线程处理接口
 * @param arg
 * @return
 */
void *ThreadGwMultMet::Start_Thread(void* arg)
{
    ThreadGwMultMet *th = (ThreadGwMultMet*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief 驱动线程启动
 * @return
 */
bool ThreadGwMultMet::Start()
{
    int ret = pthread_create(&pth,NULL,ThreadGwMultMet::Start_Thread,this);
    if(ret != 0)
    {
        m_log.WriteToLogFile("LineAuto.ThreadGwMultMet Start ","安捷伦万用表驱动线程创建失败！");
        return false;
    }
        return true;
}

/**
 * @brief 驱动线程关闭
 * @return
 */
bool ThreadGwMultMet::Stop()
{
    _stopprocess = true;
    pthread_detach(pth);
    pthread_cancel(pth);
    myComDevice->Close();
    return true;
}

/**
 * @brief 采集数据
 */
void ThreadGwMultMet::CollectDataProcess()
{

    while(!_stopprocess)
    {
        int count = 0;
        if(_stopColl == false)
        {
            m_log.WriteToLogFile("LineAuto.ThreadGwMultMet CollectDataProcess ","【" + _di.Name + "】" + "开始采集 .... ");
            printf("【%s】开始采集 ....\n",_di.Name.data());
        }
        while(!_stopColl)
        {
            usleep(100 * 1000);
            count++;
            char** p;
            string value = "";
            //读取数据
            value = myComDevice->GetValue();

            //如果采集到的数据为空，或者为超量程的异常数据，则不进行处理
            if (!strcmp(value.data(),"") || value.length() >= 20) continue;
            //将采集到的数据进行单位换算后保留两位小数
            if(devtype == Emitter_SA)
            {
                char data[20] = {0};
                double t = (strtod(value.data(),p) * 1000000);
                sprintf(data,"%.2f",t);
                _iValue = data;
            }
            else if(devtype == Emitter_FA)
            {
                char data[20] = {0};
                double t = (strtod(value.data(),p) * 1000);
                sprintf(data,"%.2f",t);
                _iValue = data;
            }
            else if(devtype == null)
            {
                char data[20] = {0};
                double t = strtod(value.data(),p);
                sprintf(data,"%.2f",t);
                _iValue = data;
            }
            string key;
            //将瞬时值写入内存数据库中瞬时值点位
            key.append(_di.DeviceCode).append("$").append("IV");
            m_db.Write_TagMValue(key,_iValue);
            _cValue.append(_iValue).append("/");
            key.clear();
            //将累计值写入内存数据库累计值点位
            key.append(_di.DeviceCode).append("$").append("CV");
            m_db.Write_TagMValue(key,_cValue);
            if (count >= 100)
            {
                count = 0;
                _stopColl = true;
            }
            if(_stopColl == true)
            {
                m_log.WriteToLogFile("LineAuto.ThreadGwMultMet CollectDataProcess ","【" + _di.Name + "】停止采集 .... ");
                printf("【%s】停止采集 ....\n",_di.Name.data());
            }
        }
        usleep(50 * 1000);
    }
}

/**
 * @brief 采集线程接口
 * @param arg
 * @return
 */
void* ThreadGwMultMet::CollectData(void *arg)
{
    ThreadGwMultMet *th = (ThreadGwMultMet*)arg;
    th->CollectDataProcess();
    return NULL;
}

#endif
