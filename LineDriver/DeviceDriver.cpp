#include "DeviceDriver.h"
#include "./LineDevice/DeviceThreads/ThreadFxPlc.h"
#include "./LineDevice/DeviceThreads/ThreadLaserMarker.h"
#include "./LineDevice/DeviceThreads/ThreadDgPlc.h"
#include "./LineDevice/DeviceThreads/ThreadCyclone.h"
#include "./LineDevice/DeviceThreads/ThreadAgilent.h"
#include "./LineDevice/DeviceThreads/ThreadKeyenceSR.h"
#include "./LineDevice/DeviceThreads/ThreadXse6.h"
#include "./LineDevice/DeviceThreads/ThreadDaMaSR.h"
#include "./LineDevice/DeviceThreads/ThreadDeltaPlc.h"
#include "./LineDevice/DeviceThreads/ThreadGwMultMet.h"
#include "./LineDevice/DeviceThreads/ThreadItechPower.h"
#include "./LineDevice/DeviceThreads/ThreadTpmsHandle.h"
#include "./LineDevice/DeviceThreads/ThreadViewCheck.h"
#include "./LineDevice/DeviceThreads/ThreadWenglorSR.h"
#include "./LineDevice/DeviceThreads/ThreadSpectrumAnalyzer.h"
#include "./LineDevice/DeviceThreads/ThreadMicrometer.h"
#include "./LineDevice/DeviceThreads/ThreadMarkViewCheck.h"
#include "./LineDevice/DeviceThreads/ThreadTekDMM6500.h"
#include "./LineDevice/DeviceThreads/ThreadFx5U.h"
#include "LineDevice/DeviceThreads/ThreadMicrocaliper.h"
#include "DataHelper/LogFile.h"

/**
 * @brief DeviceDriver::DeviceDriver            含参构造
 * @param li                                    产线信息
 */
DeviceDriver::DeviceDriver(LineInfo li)
{
    _li = li;
}

/**
 * @brief DeviceDriver::DeviceDriver
 */
DeviceDriver::DeviceDriver()
{
}

/**
 * @brief DeviceDriver::~DeviceDriver           析构函数
 */
DeviceDriver::~DeviceDriver()
{
    if(!dts.empty())
    {
        for(list<DeviceThread*>::iterator it = dts.begin();it != dts.end();it++)
        {
            DeviceThread *p = *it;
            delete p;
            p = NULL;
        }
    }
}

/**
 * @brief DeviceDriver::Start                   加载设备驱动
 * @return 加载成功返回true
 */
bool DeviceDriver::Start()
{
    _log.LOG_INFO("DeviceDriver 【开始加载设备驱动】 ......");

    if (_li.Name.data() == NULL)
    {
        printf("读取驱动配置文件失败！\n");
        _log.LOG_ERROR("DeviceDriver Start 读取驱动配置文件失败！");
        return false;
    }

    StationInfo si = _li.Si;
    map<string, DeviceInfo> dikvp = si.Dis;
    for(map<string, DeviceInfo>::reverse_iterator im = dikvp.rbegin();im != dikvp.rend();im++)
    {
        int code = atoi(im->second.machDriverNo.data());
        bool deviceEnable = im->second.Enable;
        //如果设备不使能则直接跳过启动设备线程
        if(!deviceEnable)
        {
            _log.LOG_INFO("DeviceDriver 【%s】 设备未使能 ",im->second.Name.data());
            continue;
        }
        switch(code)
        {
        case KeyenceSr:     //keyencesr TCP
              {
                ThreadKeyenceSR *keyencesr = new ThreadKeyenceSR(im->second);
                keyencesr->Start();
                dts.push_back((DeviceThread *)keyencesr);
                break;
              }
        case DataMan60:
              {
                ThreadDaMaSR *dataman = new ThreadDaMaSR(im->second);
                dataman->Start();
                dts.push_back((DeviceThread *)dataman);
                break;
              }
        case UdpCyclone:     //CycloneProgram烧程器 UDP
              {
                ThreadCyclone *threadcyclone = new ThreadCyclone(im->second);
                threadcyclone->Start();
                dts.push_back((DeviceThread *)threadcyclone);
                break;
              }
        case FXPLC:     //三菱FXPLC COM
              {
                ThreadFxPlc *threadfxplc = new ThreadFxPlc(im->second);
                threadfxplc->Start();
                dts.push_back((DeviceThread *)threadfxplc);
                break;
              }
        case DelPLC:     //台达PLC
              {
                ThreadDeltaPlc *threadDetalPlc = new ThreadDeltaPlc(im->second);
                threadDetalPlc->Start();
                dts.push_back((DeviceThread *)threadDetalPlc);
                break;
              }
        case DgPLC:     //大工PLC  TCP
              {
                ThreadDgPlc *threadgplc = new ThreadDgPlc(im->second);
                threadgplc->Start();
                dts.push_back((DeviceThread *)threadgplc);
                break;
              }
        case LM:     //LaserMarker打标机 TCP
              {
                ThreadLaserMarker *threadlasermarker = new ThreadLaserMarker(im->second);
                threadlasermarker->Start();
                dts.push_back((DeviceThread *)threadlasermarker);
                break;
              }
        case AgilMultMet:     //安捷伦电流表 TCP
              {
                ThreadAgilent *threadagilentcm = new ThreadAgilent(im->second);
                threadagilentcm->Start();
                dts.push_back((DeviceThread *)threadagilentcm);
                break;
              }
        case TekDMM6500:    //泰克万用表
            {
                ThreadTekDMM6500 *threadTekDMM6500 = new ThreadTekDMM6500(im->second);
                threadTekDMM6500->Start();
                dts.push_back((DeviceThread *)threadTekDMM6500);
                break;
            }
//        case GwMultMet:     //固伟万用表
//              {
//                ThreadGwMultMet *threadGw = new ThreadGwMultMet(im->second);
//                threadGw->Start();
//                dts.push_back((DeviceThread *)threadGw);
//                break;
//              }
        case Xse6:    //Xse6数显表
              {
                ThreadXse6 *threadxse6 = new ThreadXse6(im->second);
                threadxse6->Start();
                dts.push_back((DeviceThread *)threadxse6);
                break;
              }
        case TpmsHand:    //Tpms手柄
              {
                ThreadTpmsHandle *threadTpms = new ThreadTpmsHandle(im->second);
                threadTpms->Start();
                dts.push_back((DeviceThread *)threadTpms);
                break;
              }
        case VC:     //视觉检测设备
              {
                ThreadViewCheck *threadViewCheck = new ThreadViewCheck(im->second);
                threadViewCheck->Start();
                dts.push_back((DeviceThread *)threadViewCheck);
                break;
              }
        case Itech:     //稳压电源
              {
                ThreadItechPower *threadItechPower = new ThreadItechPower(im->second);
                threadItechPower->Start();
                dts.push_back((DeviceThread *)threadItechPower);
                break;
              }
        case MicroMeter:     //高度检测
              {
                ThreadMicrometer *threadMicrometer = new ThreadMicrometer(im->second);
                threadMicrometer->Start();
                dts.push_back((DeviceThread *)threadMicrometer);
                break;
              }
        case TekTronix: case RohdSpecAnal: case AgilentN9010:      //频谱仪
              {
                ThreadSpectrumAnalyzer *threadSA = new ThreadSpectrumAnalyzer(im->second);
                threadSA->Start();
                dts.push_back((DeviceThread *)threadSA);
                break;
              }
        case MarkView:      //MarkView视觉检测
              {
                ThreadMarkViewCheck* threadMV = new ThreadMarkViewCheck(im->second);
                threadMV->Start();
                dts.push_back((DeviceThread *)threadMV);
                break;
              }
        case FX5U:      //FX5U PLC
              {
                ThreadFx5U* threadFx5U = new ThreadFx5U(im->second);
                threadFx5U->Start();
                dts.push_back((DeviceThread *)threadFx5U);
                break;
              }
        case MicroCaliper:      //测微计
              {
                ThreadMicrocaliper* threadCaliper = new ThreadMicrocaliper(im->second);
                threadCaliper->Start();
                dts.push_back((DeviceThread *)threadCaliper);
                break;
              }
        default:
               break;
        }
    }
    return true;
}

/**
 * @brief DeviceDriver::Stop                    驱动停止
 * @return                                      处理完毕返回true
 */
bool DeviceDriver::Stop()
{
    if(!dts.empty())
    {
        for(list<DeviceThread*>::iterator it = dts.begin();it != dts.end();it++)
        {
            DeviceThread *p = *it;
            p->Stop();
        }
    }
    return true;
}

