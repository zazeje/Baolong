#include "ThreadViewCheck.h"
#include "main.h"

ThreadViewCheck::ThreadViewCheck()
{
//    myDevice = new ViewCheck(1234,"192.168.");
//    _stopprocess = false;
}

/**
 * @brief 构造函数
 * @param di
 */
ThreadViewCheck::ThreadViewCheck(DeviceInfo di): DeviceThread(di)
{
    if (di.Port.empty()) _di.Port = "1234";
    myDevice = new ViewCheck(atoi(_di.Port.data()),_di.Ip,_di.Name);
    _stopprocess = false;
}

/**
 * @brief 析构函数
 */
ThreadViewCheck::~ThreadViewCheck()
{
    if(myDevice != NULL)
    {
        delete myDevice;
        myDevice = NULL;
    }
}

/**
 * @brief ThreadViewCheck::judgeViewCheck
 * @param result 视觉设备返回的检查结果值
 */
void ThreadViewCheck::judgeViewCheck(string result)
{
    _log.LOG_DEBUG("【%s】 视觉检测返回值为：【%s】",_di.Name.data(),result.data());
    if(!result.compare("1"))
    {
        m_db.Write_TagMValue(_di.JudgeResult,"1");
        pi.testItemJudgeResult = 1;
        pi.testItemEigenValue = "OK";
        //display.cpp 显示
        m_db.Write_TagMValue(_di.BarCode, "视觉检测合格");
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult,"0");
        pi.testItemJudgeResult = 0;
        pi.testItemEigenValue = "NG";
        m_db.Write_TagMValue(_di.BarCode, "视觉检测不合格");
    }
}

/**
 * @brief ThreadViewCheck::processViewCheck
 * @param partSeqNo
 * @param partId
 */
void ThreadViewCheck::processViewCheck(string partSeqNo, string partId)
{
    if(!partSeqNo.empty() && !partId.empty())
    {
        if(_di.testItemCode.size() > 0)
        {
#ifndef _Debug
            string result = myDevice->GetCheckResult(partId);
#else
            string result = "0";
            if((atoi(partId.data()) % 2) == 0)
            {
                result = "1";
            }
            else
            {
                result = "1";
            }
            if((atoi(partId.data()) % 3) == 0)
            {
                result = "0";
            }
#endif
            judgeViewCheck(result);
//                               _log.LOG_DEBUG("ThreadViewCheck 【%s】 检测结果为【%s】",_di.Name.data(),result.data());
        }
        else
        {
             m_db.Write_TagMValue(_di.testParaAlarm, "1");
             _log.LOG_DEBUG("ThreadViewCheck 【%s】 缺少【测试项判定参数】",_di.Name.data());
             m_db.Write_TagMValue(_di.BarCode, "缺少【测试项判定参数】");
        }
        //设备使能为0时，不进行判定，结果置为1
        noJudge();
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadViewCheck 【%s】 获取序列号、id为【空】，不进行测试，直接判定为【不良品】",_di.Name.data());
        string svalue;
        svalue += partId.empty()?("产品ID为空，"):("");
        svalue += partSeqNo.empty()?("序列号为空，"):("");
        svalue += "直接判定为【不良品】";
        m_db.Write_TagMValue(_di.BarCode,svalue);
    }
}

/**
 * @brief 驱动线程处理函数
 */
void ThreadViewCheck::threadprocess()
{
    _log.LOG_INFO("ThreadViewCheck 【%s】线程驱动已启动......",_di.Name.data());

    getDeviceNum();

    while (!_stopprocess)
    {
       usleep(100 * 1000);
       //设备不使能时,默认设备的判定结果为良品
       if(!getDeviceEnable())
           continue;
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
                       _log.LOG_DEBUG("ThreadViewCheck 【%s】 检测到视觉检测信号",_di.Name.data());
                       m_db.Write_TagMValue(tag.TagName, "0");
                       _stopprocess = false;                                              
                       string partId = m_db.Read_TagMValue(_di.IdFlag);
                       string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                       _log.LOG_DEBUG("ThreadViewCheck 【%s】 id为【%s】 序列号为【%s】",_di.Name.data(),partId.data(),partSeqNo.data());

                       processViewCheck(partSeqNo, partId);

                       //将产品信息存储到数据库
                       saveToSql(partId, partSeqNo,0);
                       //将视觉检测结果上传至上位机
                       sendValueToTcpServer(pi.testItemEigenValue);
                   }
               }
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
    _log.LOG_DEBUG("ThreadViewCheck 【%s】线程驱动已停止......",_di.Name.data());
}

/**
 * @brief ThreadViewCheck::CommunicateTest
 */
void ThreadViewCheck::CommunicateTest()
{
    _connectstatus = myDevice->CanAcess() ? "1" : "0";
    m_db.Write_TagMValue(_di.Alarm, _connectstatus);
    if(_connectstatus == "0")
    {
        _log.LOG_ERROR("ThreadViewCheck 【%s】 通信检测【失败】",_di.Name.data());
        m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
    }
    else
        m_db.Write_TagMValue(_num + "$" + "NT","");
}

/**
 * @brief 驱动线程处理接口
 * @param arg
 * @return
 */
void *ThreadViewCheck::Start_Thread(void* arg)
{
    ThreadViewCheck *th = (ThreadViewCheck*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief 启动驱动线程
 * @return
 */
bool ThreadViewCheck::Start()
{
    pthread_attr_t pth_attr;
    pthread_attr_init(&pth_attr);
    pthread_attr_setdetachstate(&pth_attr,PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&pth,&pth_attr,ThreadViewCheck::Start_Thread,this);
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
bool ThreadViewCheck::Stop()
{
    _stopprocess = true;
    myDevice->Close();
    myDevice->CloseReconThread();
    return true;
}
