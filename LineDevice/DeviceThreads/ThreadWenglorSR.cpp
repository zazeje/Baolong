#include "ThreadWenglorSR.h"
#include "./LineModel/DeviceConfig.h"
#include "./DataHelper/MemoryDBHelper.h"

using namespace std;

ThreadWenglorSR::ThreadWenglorSR()      //威格乐扫码器
{}

/**
 * @brief 构造函数
 * @param di
 */
ThreadWenglorSR::ThreadWenglorSR(DeviceInfo di): DeviceThread(di)
{
    _stopprocess = false;
    myDevice = new WenglorSR(SerialPortParameters::GetSpp(_di),_di.Name);
}

/**
 * @brief 析构函数
 */
ThreadWenglorSR::~ThreadWenglorSR()
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
void ThreadWenglorSR::threadprocess()
{
    string barCode;
    m_log.WriteToLogFile("LineAuto.ThreadWenglorSR threadprocess ","【" + _di.Name + "】线程驱动已启动......");
    printf("%s,线程已启动......\n", _di.Name.data());
    _counter = 0;
    while(!_stopprocess)
    {
       usleep(100 * 1000);
       string startFlag = m_db.Read_TagMValue(_di.StartFlag);
       //检测到开始信号，清空以前的扫码值
       if((!startFlag.empty()) && (!startFlag.compare("1")))
       {
            m_log.WriteToLogFile("LineAuto.ThreadWenglorSR threadprocess ","【" + _di.Name + "】读实时库 StartFlag 值为1 ");
            printf("%s 读实时库 StartFlag 值为1 \n",_di.Name.data());
            barCode="";
            m_db.Write_TagMValue("SerialNo","");
            m_db.Write_TagMValue(_di.StartFlag,"0");
       }
       // 遍历采集点
       for(map<string, UnitInfo>::reverse_iterator it = _di.Units.rbegin();it != _di.Units.rend();it++)
       {
           map<string, Tag> tags = it->second.Tags;
           for(map<string, Tag>::reverse_iterator im = tags.rbegin();im != tags.rend();im++)
           {
               Tag tag = im->second;
               string name = tag.TagCode;
               tag.MemortValue = m_db.Read_TagMValue(tag.TagName);
               if (tag.MemortValue.empty()) continue;
               //处理开始扫码信号
               if (!name.compare("SSB"))
               {
                   if (!tag.MemortValue.compare("1"))
                   {
                       m_log.WriteToLogFile("LineAuto.ThreadWenglorSR threadprocess ","【" + _di.Name + "】检测到开始扫码信号 ");
                       m_db.Write_TagMValue("Note","【" + _di.Name + "】" + "检测到开始扫码信号");
                       m_db.Write_TagMValue(tag.TagName, "0");
                       _stopprocess = false;
                       barCode = myDevice->AutoScan(3);
                       pi.ProductCount++;
                       int i = 0;
                       while((i = barCode.find('\n')) != string::npos && (i = barCode.find('\r')) != string::npos)
                       {
                           barCode.erase(i);
                       }
                       m_db.Write_TagMValue("ProductCount",IntToString(pi.ProductCount));
                       m_db.Write_TagMValue("SerialNo",barCode);
                       m_log.WriteToLogFile("LineAuto.DeviceThreads Start ","【" + _di.Name + "】扫码结果为 " + barCode);
                       printf("%s 扫码结果 %s \n",_di.Name.data(),barCode.data());
                       if(barCode.empty())
                           m_db.Write_TagMValue(_di.DeviceCode + "$" + "JR","0");
                       else
                           m_db.Write_TagMValue(_di.DeviceCode + "$" + "JR","1");
                   }
               }
               //处理通信状态
               else if(!name.compare("CS"))
               {
                   if (_counter % 20 == 0)
                   {
                       _connectstatus = myDevice->CanAcess() ? "1" : "0";
                       _counter = 1;
                       string key = _di.DeviceCode + "$" + "CS";
                       m_db.Write_TagMValue(key, _connectstatus);
                   }
               }
           }
       }
       _counter++;
    }
    m_log.WriteToLogFile("LineAuto.ThreadWenglorSR threadprocess ","【" + _di.Name + "】线程驱动已停止......");
    printf("%s,线程已停止......\n", _di.Name.data());
}

/**
 * @brief 启动线程接口
 * @param arg
 * @return
 */
void *ThreadWenglorSR::Start_Thread(void* arg)
{
    ThreadWenglorSR *th = (ThreadWenglorSR*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief 启动驱动线程
 * @return
 */
bool ThreadWenglorSR::Start()
{
    int ret = pthread_create(&pth,NULL,ThreadWenglorSR::Start_Thread,this);
    if(ret != 0)
        return false;
    else
        return true;
}

/**
 * @brief 关闭驱动线程
 * @return
 */
bool ThreadWenglorSR::Stop()
{
    _stopprocess = true;
    pthread_detach(pth);
    pthread_cancel(pth);
    myDevice->Close();
    return true;
}
