#ifndef THREADSPECTRUMANALYZER_H
#define THREADSPECTRUMANALYZER_H

#include "LineDevice/Device/DeviceThread.h"
#include "LineDevice/SpectrumAnalyzerDevice/SpectrumBaseFactory.h"
#include "LineDevice/SpectrumAnalyzerDevice/AgilentN9010A.h"
#include "LineDevice/SpectrumAnalyzerDevice/RohdeSchwarz.h"
#include "DataCommunication/TcpServer.h"
#include "DataHelper/LogFile.h"




class ThreadSpectrumAnalyzer : public DeviceThread
{
public:

    ThreadSpectrumAnalyzer();
    ThreadSpectrumAnalyzer(DeviceInfo di);
    ~ThreadSpectrumAnalyzer();

    bool Start();
    bool Stop();

private:

    bool _stopprocess;
    string _lastProductModel;
    double _minStandar;
    double _maxStandar;

    pthread_t start_pth;

    SpectrumAnalyzerBase* mydevice;

    void threadprocess();
    void noJudge(string partNoId,string partSeqNo);
    void sendSpectrumAnalyzerValue(vector<string>& result);
    void saveAllValueToSql(string partNoId,string partSeqNo,vector<string>& result);
    void processSpectrumAnalyzerCheck(string partSeqNo, string partNoId, vector<string>& result);
    void CommunicateTest();

    static void* Start_Thread(void*);
};

#endif //THREADSPECTRUMANALYZER_H
