#ifndef PARSELINEINFO_H
#define PARSELINEINFO_H

#include <string>
#include <vector>
#include "DataCommunication/TcpServer.h"
#include "DataHelper/MySQLHelper.h"
#include "DataHelper/DatabaseInfo.h"
#include "LineInformation.h"
#include "ConfigLoader.h"
#include "DataHelper/LogFile.h"


using namespace std;

/**
 * @brief The DispatchItem struct
 */
struct DispatchItem
{
    string Dispatch_No;     //派工单号
    string Part_No;         //件号
};

extern vector<DispatchItem> currDistpatchList;
class ParseLineInfo
{
public:
    ParseLineInfo();

//    DispatchInfo getDispatchInfo(string dispatchNo);
    void getOperationNoInfo(string partNo,string workCenterNo);
    void getWorkCenterInfo(string workCenterNo, LineInfo &li);
    vector<MachineInfo> getMachineInfo(string workCenterNo);
    vector<MachParameterInfo> getMachineParameterInfo(string partNo , string workCenterNo);
    vector<MachTestItemInfo> getMachineTestItemInfo(string partNo , string workCenterNo);
    vector<MachParameterCodeInfo> getParameterCodeInfo();
    vector<MachTestItemCodeInfo> getTestItemCodeInfo();

    LineInfo parseLineInformation(string partNo, string workCenterNo);
    string GetTpmsHandleFormula();
    int getDispatchInfoByDB();
private:
    string m_formula;
};

#endif // PARSELINEINFO_H
