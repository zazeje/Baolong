#include "ThreadDaMaSR.h"
#include "./LineModel/DeviceConfig.h"
#include "./DataHelper/MemoryDBHelper.h"
using namespace std;


ThreadDaMaSR::ThreadDaMaSR()            //康耐视扫码器
{}

/**
 * @brief 构造函数
 * @param di
 */
ThreadDaMaSR::ThreadDaMaSR(DeviceInfo di): DeviceThread(di)
{
    _stopprocess = false;
    myDevice = new DataManSR(SerialPortParameters::GetSpp(_di),_di.Name);
}

/**
 * @brief 析构函数
 */
ThreadDaMaSR::~ThreadDaMaSR()
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
void ThreadDaMaSR::threadprocess()
{
    string barCode;
    _counter = 0;
    m_log.WriteToLogFile("LineAuto ThreadDaMaSR threadprocess ","【" + _di.Name + "】线程驱动已启动......");
    printf("【%s】线程驱动已启动......\n", _di.Name.data());
    int productId = 0;
    while(!_stopprocess)
    {
        usleep(100 * 1000);
        string startFlag = m_db.Read_TagMValue(_di.StartFlag);
        //开始信号为"1"时，将遗留信息清空
        if((!startFlag.empty()) && (!startFlag.compare("1")))
        {
            m_log.WriteToLogFile("LineAuto ThreadDaMaSR threadprocess ","检测到启动信号，清空序列号，复位启动信号");
            barCode.clear();
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
                //处理开始扫码信号
                if (!name.compare("SSB"))
                {
                    if (!tag.MemortValue.compare("1"))
                    {
                        m_log.WriteToLogFile("LineAuto ThreadDaMaSR threadprocess ","【" + _di.Name + "】检测到开始扫码信号");
                        m_db.Write_TagMValue("Note","【" + _di.Name + "】" + "检测到开始扫码信号");
                        printf("LineAuto ThreadDaMaSR threadprocess 【%s】检测到开始扫码信号\n",_di.Name.data());
                        barCode.clear();
                        m_db.Write_TagMValue(tag.TagName, "0");
                        m_db.Write_TagMValue(_di.DeviceState, "扫码");
                        _stopprocess = false;
                        //开始扫码，扫码时间为3秒，3秒后自动停止扫码
                        barCode = myDevice->AutoScan(3);
                        int i = 0;
                        //去除扫码结果字符串最后的'\r''\n'
                        while((i = barCode.find('\n')) != string::npos && (i = barCode.find('\r')) != string::npos)
                        {
                            barCode.erase(i);
                        }
                        m_log.WriteToLogFile("LineAuto ThreadDaMaSR threadprocess ","【" + _di.Name + "】读扫码结果为 [" + barCode + "] ");
                        m_db.Write_TagMValue("SerialNo",barCode);
                        pi.ProductCount++;
                        m_db.Write_TagMValue("ProductCount",IntToString(pi.ProductCount));
                        //工艺判定
                        if(barCode.empty())
                        {
                            m_log.WriteToLogFile("LineAuto ThreadDaMaSR threadprocess ","JR append 0");
                            m_db.Write_TagMValue(_di.DeviceCode + "$" + "JR","0");
                        }
                        else
                        {
                            m_log.WriteToLogFile("LineAuto ThreadDaMaSR threadprocess ","JR append 1");
                            m_db.Write_TagMValue(_di.DeviceCode + "$" + "JR","1");
                        }
                    }
                }
                //处理判定结果点位
                else if(!name.compare("JR"))
                {
                     m_db.Write_TagMValue(tag.TagName,barCode.empty() ? "0":"1");
                }
                //处理通信状态点
                else if(!name.compare("CS"))
                {
                    if (_counter % 20 == 0)
                    {
                        _connectstatus = myDevice->CanAcess() ? "1" : "0";
                        _counter = 1;
                        string key = _di.DeviceCode + "$" + "CS";
                        if(_connectstatus == "0")
                        {
                            m_log.WriteToLogFile("LineAuto ThreadDaMaSR threadprocess ","连接失败,向上位机报警");
                            m_db.Write_TagMValue(key, "0");
                        }
                        else
                            m_db.Write_TagMValue(key, "1");
                    }
                }
            }
        }
    _counter++;
    }
    m_log.WriteToLogFile("LineAuto.ThreadDaMaSR threadprocess ","【" + _di.Name + "】线程驱动已停止......");
    printf("【%s】线程驱动已停止......\n", _di.Name.data());
}

/**
 * @brief 驱动线程处理接口
 * @param arg
 * @return
 */
void *ThreadDaMaSR::Start_Thread(void* arg)
{
    ThreadDaMaSR *th = (ThreadDaMaSR*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief 启动设备驱动线程
 * @return
 */
bool ThreadDaMaSR::Start()
{
    int ret = pthread_create(&pth,NULL,ThreadDaMaSR::Start_Thread,this);
    if(ret != 0)
    {
        return false;
    }
    return true;
}

/**
 * @brief 设备驱动线程关闭
 * @return
 */
bool ThreadDaMaSR::Stop()
{
    _stopprocess = true;
    pthread_detach(pth);
    pthread_cancel(pth);
    myDevice->Close();
    myDevice->CloseReconThread();
    return true;
}

