#include "ThreadMarkViewCheck.h"
#include "main.h"

ThreadMarkViewCheck::ThreadMarkViewCheck()
{}

/**
 * @brief 构造函数
 * @param di
 */
ThreadMarkViewCheck::ThreadMarkViewCheck(DeviceInfo di): DeviceThread(di)
{
    if (di.Port.empty()) _di.Port = "9600";
    myDevice = new MarkViewCheck(atoi(_di.Port.data()),_di.Ip,_di.Name);
    _stopprocess = false;
}

/**
 * @brief 析构函数
 */
ThreadMarkViewCheck::~ThreadMarkViewCheck()
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
void ThreadMarkViewCheck::threadprocess()
{
    _log.LOG_INFO("ThreadMarkViewCheck 【%s】线程驱动已启动......",_di.Name.data());

    //得到设备编号
    getDeviceNum();                     //1#视觉设备

    while (!_stopprocess)
    {
       usleep(100 * 1000);
       //设备不使能时,默认设备的判定结果为良品
       if(!getDeviceEnable())
           continue;
       //处理信号结束点
       processEndFlag();

       // 遍历采集点
       for(map<string, UnitInfo>::reverse_iterator it = _di.Units.rbegin();it != _di.Units.rend();it++)
       {
           map<string, Tag> tags = it->second.Tags;
           for(map<string, Tag>::reverse_iterator im = tags.rbegin();im != tags.rend();im++)
           {
               Tag tag = im->second;
               string name = tag.TagCode;
               tag.MemortValue = m_db.Read_TagMValue(tag.TagName);
               //处理开始扫码信号
               if (!name.compare("SC"))
               {
                   if (!tag.MemortValue.compare("1"))
                   {
                       _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】检测到视觉检测信号",_di.Name.data());
                       clearData(tag.TagName);
                       string partNoId = m_db.Read_TagMValue(_di.IdFlag);
                       string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                       _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】 partId = 【%s】  partSeqNo = 【%s】 ",_di.Name.data(),partNoId.data(), partSeqNo.data());
                       //处理视觉信号逻辑
                       processViewCheck(partSeqNo, partNoId);
                       if(_di.testItemCode.size() > 0)
                            pi.testItemCode = _di.testItemCode.at(0);
                       //插入数据到数据库
                       saveToSql(partNoId, partSeqNo,0);
                       //将视觉检测结果上传至上位机
                       sendValueToTcpServer(m_viewRes);
                   }
               }
               //处理通信状态点
               else if (!name.compare("CS"))
               {
                   if (_counter % 20 == 0)
                   {
                      CommunicateTest();
                       _counter = 1;
                   }
               }
           }
       }
       _counter++;
    }
    _log.LOG_INFO("ThreadMarkViewCheck 【%s】线程驱动已停止......",_di.Name.data());
}

/**
 * @brief 驱动线程处理接口
 * @param arg
 * @return
 */
void *ThreadMarkViewCheck::Start_Thread(void* arg)
{
    ThreadMarkViewCheck *th = (ThreadMarkViewCheck*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief 启动驱动线程
 * @return
 */
bool ThreadMarkViewCheck::Start()
{
    int ret = pthread_create(&pth,NULL,ThreadMarkViewCheck::Start_Thread,this);
    if(ret != 0)
    {
        return false;
    }
        return true;
}

/**
 * @brief 关闭驱动线程
 * @return
 */
bool ThreadMarkViewCheck::Stop()
{
    _stopprocess = true;
    pthread_detach(pth);
    pthread_cancel(pth);
    myDevice->Close();
    myDevice->CloseReconThread();
    return true;
}

/**
 * @brief ThreadMarkViewCheck::CheckDate            对视觉设备返回的时间进行判定
 * @param currentTime
 * @return
 */
bool ThreadMarkViewCheck::CheckDate(string currentTime)
{
    /******************************************************
     *
     * 修改：20180117
     * 修改人：李世龙
     * 应现场要求，标准时间改为由派工单获取，不从系统时间获取
     *
     * ****************************************************/
    string standardTime = dispatchNo.substr(2,6);
    _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】 standardTime = 【%s】 currentTime = 【%s】",standardTime.data(),currentTime.data());
    if(!currentTime.empty() && !standardTime.empty())
    {
        if(!currentTime.compare(standardTime))
        {
            _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】 时间校验【成功】",_di.Name.data());
            return true;
        }
        else
        {
            _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】 时间校验【失败】",_di.Name.data());
            return false;
        }
    }
    return false;
}

/**
 * @brief ThreadMarkViewCheck::processViewCheck
 * @param partSeqNo
 * @param partNoId
 */
void ThreadMarkViewCheck::processViewCheck(string partSeqNo, string partNoId)
{
    if(!partSeqNo.empty() && !partNoId.empty())
    {
        if(_di.testItemCode.size() > 0)
        {
            m_checkRes = myDevice->GetCheckResult();
            judgeViewCheck(partNoId);
        }
        else
        {
             m_db.Write_TagMValue(_di.testParaAlarm, "1");
             _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】 缺少【测试项判定参数】",_di.Name.data());
             m_db.Write_TagMValue(_di.BarCode, "缺少【测试项判定参数】");
        }
        //设备判定使能为0时，直接将判定结果置为1
        noJudge();
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】 获取序列号、id为【空】，不进行测试，直接判定为【不良品】",_di.Name.data());
        string svalue;
        svalue += partNoId.empty()?("产品ID为空，"):("");
        svalue += partSeqNo.empty()?("序列号为空，"):("");
        svalue += "直接判定为【不良品】";
        m_db.Write_TagMValue(_di.BarCode,svalue);
    }
}


/**
 * @brief ThreadMarkViewCheck::judgeViewCheck
 * @param partNoId
 */
void ThreadMarkViewCheck::judgeViewCheck(string partNoId)
{
    getViewCollectValue();
    if(!m_checkRes.empty() && m_checkRes.size() == 5)
    {
        //视觉设备回复第一个数据为"OK"或"NG",第二个数据为ID,第三个数据为产品型号,第四个数据为日期,第五个数据为ID
        _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】 m_checkRes[0] = 【%s】 ",_di.Name.data(), m_checkRes[0].data());
        _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】 m_checkRes[1] = 【%s】 partNoId = 【%s】 ",_di.Name.data(), m_checkRes[1].data(),partNoId.data());
        _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】 m_checkRes[2] = 【%s】 ProductModel = 【%s】",_di.Name.data(), m_checkRes[2].data(),GetProductModel().data());
        _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】 m_checkRes[3] = 【%s】 ",_di.Name.data(), m_checkRes[3].data());
        _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】 m_checkRes[4] = 【%s】 partNoId = 【%s】",_di.Name.data(), m_checkRes[4].data(),partNoId.data());

        startToJudge(partNoId);
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult,"0");
        pi.testItemJudgeResult = 0;
        pi.testItemEigenValue = "NG";
        m_viewRes = "视觉检测不合格";
        _log.LOG_ERROR("ThreadMarkViewCheck 【%s】 视觉检测【不合格】 视觉检测结果集大小不等于5 size=【%d】",_di.Name.data(),m_checkRes.size());
        m_db.Write_TagMValue(_di.BarCode, "视觉检测【失败】");
    }
}

/**
 * @brief ThreadMarkViewCheck::getViewCollectValue
 */
void ThreadMarkViewCheck::getViewCollectValue()
{
    for(int i = 0; i< m_checkRes.size(); i++)
    {
//      _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】 视觉检测返回值 m_checkRes = 【%s】",_di.Name.data(),m_checkRes.at(i).data());
        pi.testItemCollectValue.append(m_checkRes.at(i)).append("/");
    }
    _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】  测试项采集值为【%s】",_di.Name.data(),pi.testItemCollectValue.data());
    m_db.Write_TagMValue(_di.BarCode, "视觉返回值为：【" + pi.testItemCollectValue + "】");
}


/**
 * @brief ThreadMarkViewCheck::startToJudge
 * @param partNoId
 */
void ThreadMarkViewCheck::startToJudge(string partNoId)
{
    if(m_checkRes[0] == "OK" && partNoId == m_checkRes[1] /*&& m_checkRes[2] == GetProductModel() && CheckDate(m_checkRes[3])*/ && partNoId == m_checkRes[4])
    {
        m_db.Write_TagMValue(_di.JudgeResult,"1");
        pi.testItemJudgeResult = 1;
        pi.testItemEigenValue = "OK";
        m_viewRes = "视觉检测合格";
        _log.LOG_DEBUG("ThreadMarkViewCheck 【%s】 视觉检测【合格】",_di.Name.data());
        m_db.Write_TagMValue(_di.BarCode, "视觉检测【成功】");
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult,"0");
        pi.testItemJudgeResult = 0;
        pi.testItemEigenValue = "NG";
        m_viewRes = "视觉检测不合格";
        _log.LOG_ERROR("ThreadMarkViewCheck 【%s】 视觉检测【不合格】",_di.Name.data());
        m_db.Write_TagMValue(_di.BarCode, "视觉检测【失败】");
    }
}

/**
 * @brief ThreadMarkViewCheck::clearData
 * @param tagName
 */
void ThreadMarkViewCheck::clearData(string tagName)
{
    m_viewRes.clear();
    m_checkRes.clear();
    m_db.Write_TagMValue(tagName, "0");
    pi.Clear();
}

/**
 * @brief ThreadMarkViewCheck::CommunicateTest
 */
void ThreadMarkViewCheck::CommunicateTest()
{
    _connectstatus = myDevice->CanAcess() ? "1" : "0";
    m_db.Write_TagMValue(_di.Alarm, _connectstatus);
    if(_connectstatus == "0")
    {
        _log.LOG_ERROR("ThreadMarkViewCheck 【%s】 通信检测【失败】",_di.Name.data());
        m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
    }
    else
        m_db.Write_TagMValue(_num + "$" + "NT","");
}
