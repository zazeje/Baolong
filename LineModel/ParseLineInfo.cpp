#include "ParseLineInfo.h"
#include "main.h"

extern string operationNo;         //工序号
vector<DispatchItem> currDistpatchList;


ParseLineInfo::ParseLineInfo()
{

}

/**
 * @brief ParseLineInfo::getMachineInfo           获取设备信息
 * @param workCenterNo                              工作中心编号
 * @return
 */
vector<MachineInfo> ParseLineInfo::getMachineInfo(string workCenterNo)
{
    //select * from machine_info where Work_Center_No = 'WC001'
    vector<MachineInfo> machineInfoVector;
    MYSQL_RES* result;
    MYSQL_ROW row;
    MYSQL* m_connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(m_connection);
    string command = "";
    command.append("select MCH_Code,MCH_Desc,Work_Center_No,MCH_Drive_No,Communication_Type,IPAddress,Port,BaudRate,DataBits,StopBits,Parity,Enable from machine_info where Work_Center_No=").append("'").append(workCenterNo).append("';");
    _log.LOG_INFO("ParseLineInfo 【getMachineInfo】 command = 【%s】", command.data());
    result = helper.LoadTable(m_connection,command);
    if(result != NULL)
    {
        while(row = mysql_fetch_row(result))
        {
            MachineInfo machineInfo;
            machineInfo.init();
            if(row[0]!=NULL) machineInfo.machCode = row[0];
            if(row[1]!=NULL) machineInfo.machDesc = row[1];
            if(row[2]!=NULL) machineInfo.workCenterNo = row[2];
            if(row[3]!=NULL) machineInfo.machDriverNo = row[3];
            if(row[4]!=NULL) machineInfo.type = row[4];
            if(row[5]!=NULL) machineInfo.ipAddress = row[5];
            if(row[6]!=NULL) machineInfo.port = row[6];
            if(row[7]!= NULL) machineInfo.baudrate = row[7];
            if(row[8]!= NULL) machineInfo.databits = row[8];
            if(row[9]!=NULL) machineInfo.stopbits = row[9];
            if(row[10]!=NULL) machineInfo.parity = row[10];
            if(row[11]!=NULL) machineInfo.enable = row[11];
            machineInfoVector.push_back(machineInfo);
        }
        helper.MySQL_Release(result);
    }
	else
		_log.LOG_ERROR("ParseLineInfo 【getMachineInfo】 数据库查询失败");

    command = "select now();";
    result = helper.LoadTable(m_connection, command);
    string date;
    if(result != NULL)
    {
        while(row = mysql_fetch_row(result))
        {
            date = row[0];
        }
        helper.MySQL_Release(result);
    }
    else
        _log.LOG_ERROR("ParseLineInfo 【getMysqlTime】 数据库查询失败");
    int pos = 0;
    while((pos = date.find('-')) != string::npos)
    {
        date.replace(pos, 1, "/");
    }
    _log.LOG_INFO("ParseLineInfo 通过日志时间记录下更新时间前的系统时间");
    if(!date.empty())
        TcpServer::setPBOXSysTime(date);
    _log.LOG_INFO("ParseLineInfo 【setPBOXSysTime】 系统时间：%s ",date.data());

    helper.MySQL_Close(m_connection);
    delete m_connection;
    return machineInfoVector;
}


/**
 * @brief ParseLineInfo::getMachineParameterInfo          获取设备参数信息
 * @param partNo                                            件号
 * @param workCenterNo                                      工作中心编号
 * @return
 */
vector<MachParameterInfo> ParseLineInfo::getMachineParameterInfo(string partNo , string workCenterNo)
{
    //select * from machine_parameter_info where Parameter_Type = '1' and Part_No = '110302070612'  and MCH_Code in (select MCH_Code from machine_info where Work_Center_No = 'WC001')
    vector<MachParameterInfo> machParameterInfoVector;
    MYSQL_RES* result;
    MYSQL_ROW row;
    MYSQL* m_connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(m_connection);
    string command;
    command.append("select a.*,b.MCH_Desc from machine_parameter_info a,machine_info b where a.MCH_Code = b.MCH_Code and a.Parameter_Type = '1' and a.Part_No = '")\
            .append(partNo).append("' and b.Work_Center_No = '").append(workCenterNo).append("';");
    _log.LOG_INFO("ParseLineInfo 【getMachineParameterInfo】 command = 【%s】", command.data());
    result = helper.LoadTable(m_connection,command);
    if(result != NULL)
    {
        while(row = mysql_fetch_row(result))
        {
            MachParameterInfo machParameterInfo;
            machParameterInfo.init();
            if(row[0]!=NULL) machParameterInfo.partNo = row[0];
            if(row[1]!=NULL) machParameterInfo.machCode = row[1];
            if(row[2]!=NULL) machParameterInfo.type = row[2];
            if(row[3]!=NULL) machParameterInfo.parameterNo = row[3];
            if(row[4]!=NULL) machParameterInfo.standardValue = row[4];
            machParameterInfoVector.push_back(machParameterInfo);
        }
        helper.MySQL_Release(result);
    }
	else
		_log.LOG_ERROR("ParseLineInfo 【getMachineParameterInfo】 数据库查询失败 !");
    helper.MySQL_Close(m_connection);
    delete m_connection;
    return machParameterInfoVector;
}


/**
 * @brief ParseLineInfo::getMachineTestItemInfo         获取设备测试项信息
 * @param partNo                                          件号
 * @param workCenterNo                                    工作中心编号
 * @return
 */
vector<MachTestItemInfo> ParseLineInfo::getMachineTestItemInfo(string partNo , string workCenterNo)
{
    //select * from machine_test_item_info where Part_No = '110302070612' and MCH_Code = 'MC0008'
    //select a.*,b.MCH_Desc from machine_test_item_info a,machine_info b where a.MCH_Code = b.MCH_Code and a.Part_No = '96216701' and b.Work_Center_No = 'WC0010';
    vector<MachTestItemInfo> machTestItemInfoVector;
    MYSQL_RES* result;
    MYSQL_ROW row;
    MYSQL* m_connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(m_connection);
    string command = "";
    command.append("select a.*,b.MCH_Desc from machine_test_item_info a,machine_info b where a.MCH_Code = b.MCH_Code and a.Part_No = '")\
            .append(partNo).append("' and b.Work_Center_No = '").append(workCenterNo).append("';");
    _log.LOG_INFO("ParseLineInfo 【getMachineTestItemInfo】 command = 【%s】", command.data());
    result = helper.LoadTable(m_connection,command);
    if(result != NULL)
    {
        while(row = mysql_fetch_row(result))
        {
            MachTestItemInfo machTestItemInfo;
            machTestItemInfo.init();
            if(row[0]!=NULL) machTestItemInfo.partNo = row[0];
            if(row[1]!=NULL) machTestItemInfo.machCode = row[1];
            if(row[2]!=NULL) machTestItemInfo.testItemCode = row[2];
            if(row[3]!=NULL) machTestItemInfo.judgeEneble = atoi(row[3]);
            if(row[4]!=NULL) machTestItemInfo.errorCode = row[4];
            if(row[5]!=NULL) machTestItemInfo.standardValue = atof(row[5]);
            if(row[6]!=NULL) machTestItemInfo.maxValue = atof(row[6]);
            if(row[7]!=NULL) machTestItemInfo.minValue = atof(row[7]);
            if(row[8]!=NULL) machTestItemInfo.deviation = atof(row[8]);
            if(row[9]!=NULL) machTestItemInfo.minPassrate = atoi(row[9]);
            if(row[10]!=NULL) machTestItemInfo.maxLimit = atoi(row[10]);
            machTestItemInfoVector.push_back(machTestItemInfo);
        }
        helper.MySQL_Release(result);
    }
	else
		_log.LOG_ERROR("ParseLineInfo 【getMachineTestItemInfo】 数据库查询失败 !");
    helper.MySQL_Close(m_connection);
    delete m_connection;
    return machTestItemInfoVector;
}

/**
 * @brief ParseLineInfo::getProcessInfo
 * @param workCenterNo
 * @param partNo
 * @return
 */
void ParseLineInfo::getOperationNoInfo(string partNo,string workCenterNo)
{
    MYSQL_RES* result;
    MYSQL_ROW row;
    MYSQL* m_connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(m_connection);
    string command = "select Operation_No from process_info where Work_Center_No='" + workCenterNo + "' and Part_No = '" + partNo + "';";
    _log.LOG_INFO("ParseLineInfo 【getOperationNoInfo】 command = 【%s】", command.data());
    result = helper.LoadTable(m_connection,command);
    if(result != NULL)
    {
        while(row = mysql_fetch_row(result))
        {
            if(row[0] != NULL)
                g_locallineInfo.m_localOperationNo = row[0];
            else
                g_locallineInfo.m_localOperationNo = "";
        }
        helper.MySQL_Release(result);
    }
    else
    {
        g_locallineInfo.m_localOperationNo = "";
        _log.LOG_ERROR("ParseLineInfo 【getOperationNoInfo】 数据库查询失败 !");
    }
    helper.MySQL_Close(m_connection);
    delete m_connection;
}

/**
 * @brief ParseLineInfo::getProcessInfo
 * @param workCenterNo
 * @param partNo
 * @return
 */
void ParseLineInfo::getWorkCenterInfo(string workCenterNo, LineInfo& li)
{
    MYSQL_RES* result;
    MYSQL_ROW row;
    MYSQL* m_connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(m_connection);
    string command = "select * from work_center_info where Work_Center_No='" + workCenterNo + "';";
    _log.LOG_INFO("ParseLineInfo 【getWorkCenterInfo】 command = 【%s】", command.data());
    result = helper.LoadTable(m_connection,command);
    if(result != NULL)
    {
        while(row = mysql_fetch_row(result))
        {
            if(row[1] != NULL)
                li.Si.Name = row[1];
            else
                li.Si.Name = "";
            if(row[2] != NULL)
                li.Si.ProductionLine = row[2];
            else
                li.Si.ProductionLine = "";
            if(row[3] != NULL)
                li.Si.judgeMethod = atoi(row[3]);
            else
                li.Si.judgeMethod = 0;
            if(row[5] != NULL)
            {
                li.Si.productNumber = atoi(row[5]);
                productnum = li.Si.productNumber;
            }
            else
                li.Si.productNumber = 0;
            if(row[6] != NULL)
                li.Si.judgeEnable = atoi(row[6]);
            else
                li.Si.judgeEnable = true;
        }
        helper.MySQL_Release(result);
    }
	else
		_log.LOG_ERROR("ParseLineInfo 【getWorkCenterInfo】 数据库查询失败 !");
    helper.MySQL_Close(m_connection);
    delete m_connection;
}

vector<MachParameterCodeInfo> ParseLineInfo::getParameterCodeInfo()
{
    vector<MachParameterCodeInfo> mpcInfo;
    MYSQL_RES* result;
    MYSQL_ROW row;
    MYSQL* m_connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(m_connection);
    string command = "select mch_code,parameter_no,parameter_desc from machine_parameter_code_info;";
    _log.LOG_INFO("ParseLineInfo 【getParameterCodeInfo】 command = 【%s】", command.data());
    result = helper.LoadTable(m_connection,command);
    if(result != NULL)
    {
        while(row = mysql_fetch_row(result))
        {
            MachParameterCodeInfo info;
            if(row[0] != NULL)
                info.machCode = row[0];
            else
                info.machCode = "";
            if(row[1] != NULL)
                info.parameterNo = row[1];
            else
                info.parameterNo = "";
            if(row[2] != NULL)
                info.parameterDesc = row[2];
            else
                info.parameterDesc = "";
            mpcInfo.push_back(info);
        }
        helper.MySQL_Release(result);
    }
    else
        _log.LOG_ERROR("ParseLineInfo 【getParameterCodeInfo】 数据库查询失败 !");
    helper.MySQL_Close(m_connection);
    delete m_connection;
    return mpcInfo;
}

vector<MachTestItemCodeInfo> ParseLineInfo::getTestItemCodeInfo()
{
    vector<MachTestItemCodeInfo> mticInfo;
    MYSQL_RES* result;
    MYSQL_ROW row;
    MYSQL* m_connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(m_connection);
    string command = "select mch_code,test_item_code,test_item_desc from machine_test_item_code_info;";
    _log.LOG_INFO("ParseLineInfo 【getTestItemCodeInfo】 command = 【%s】", command.data());
    result = helper.LoadTable(m_connection,command);
    if(result != NULL)
    {
        while(row = mysql_fetch_row(result))
        {
            MachTestItemCodeInfo info;
            if(row[0] != NULL)
                info.machCode = row[0];
            else
                info.machCode = "";
            if(row[1] != NULL)
                info.testItemCode = row[1];
            else
                info.testItemCode = "";
            if(row[2] != NULL)
                info.testItemDesc = row[2];
            else
                info.testItemDesc = "";
            mticInfo.push_back(info);
        }
        helper.MySQL_Release(result);
    }
    else
        _log.LOG_ERROR("ParseLineInfo 【getTestItemCodeInfo】 数据库查询失败 !");
    helper.MySQL_Close(m_connection);
    delete m_connection;
    return mticInfo;
}


LineInfo ParseLineInfo::parseLineInformation(string partNo, string workCenterNo)
{
    int matchDev=0;
    LineInfo li;
    LineInfo null;
    vector<MachineInfo> machineInfo = getMachineInfo(workCenterNo);
    vector<MachParameterInfo> machParameterInfo = getMachineParameterInfo(partNo,  workCenterNo);
    vector<MachTestItemInfo> machTestItemInfo = getMachineTestItemInfo(partNo,  workCenterNo);
//    vector<MachParameterCodeInfo> machParaCodeInfo = getParameterCodeInfo();
//    vector<MachTestItemCodeInfo> machTestItemCodeInfo = getTestItemCodeInfo();
    ConfigLoader *configloader = new ConfigLoader();
    li =  configloader->LoadConfig("./PBOX.xml");
    getWorkCenterInfo(workCenterNo,li);

    for(map<string,DeviceInfo>::iterator iterdi = li.Si.Dis.begin();iterdi!= li.Si.Dis.end();++iterdi)
    {
        DeviceInfo di = iterdi->second;
        if(machineInfo.size() > 0)
        {
            for(int i=0; i<machineInfo.size(); i++)
            {
//                cout<<"--- name = "<<di.Name<<"---- DeviceCode = "<<di.DeviceCode<<" ---- machCode = "<<machineInfo.at(i).machCode<<endl;
                if(!di.DeviceCode.compare(machineInfo.at(i).machCode))
                {
                    di.machDriverNo = machineInfo.at(i).machDriverNo;
                    di.type = machineInfo.at(i).type;
                    di.Ip = machineInfo.at(i).ipAddress;
                    di.Port = machineInfo.at(i).port;
                    di.BaudRate = machineInfo.at(i).baudrate;
                    di.DataBits = machineInfo.at(i).databits;
                    di.StopBits = machineInfo.at(i).stopbits;
                    di.Parity = machineInfo.at(i).parity;
                    di.Enable = atoi(machineInfo.at(i).enable.data());
                    matchDev++;
                }
            }
        }
        else
        {
             _log.LOG_ERROR("ParseLineInfo  machineInfo 【%s】设备信息参数为【空】",di.Name.data());
             return null;
        }


        if(machParameterInfo.size() > 0)
        {
            for(int k=0; k<machParameterInfo.size(); k++)
            {
                if(!di.DeviceCode.compare(machParameterInfo.at(k).machCode))
                {
                    di.parameterNo.push_back(machParameterInfo.at(k).parameterNo);      //每个设备有多个参数编码
                    di.parameter.push_back(machParameterInfo.at(k).standardValue);      //参数（量程、路径）
                    di.max.push_back(machParameterInfo.at(k).maxValue);
                    di.min.push_back(machParameterInfo.at(k).minValue);
                    _log.LOG_ERROR("ParseLineInfo  machine_parameter_info 【%s】获取加工参数【成功】",di.Name.data());
                }
            }
        }
        else
            _log.LOG_ERROR("ParseLineInfo  machine_parameter_info 【%s】加工参数为【空】",di.Name.data());

        if(machTestItemInfo.size() > 0)
        {
            for(int j=0; j<machTestItemInfo.size(); j++)
            {
                if(!di.DeviceCode.compare(machTestItemInfo.at(j).machCode))
                {
                    di.testItemCode.push_back(machTestItemInfo.at(j).testItemCode);     //每个设备有多个测试项
                    di.judgeEneble.push_back(machTestItemInfo.at(j).judgeEneble);
                    di.errorCode.push_back(machTestItemInfo.at(j).errorCode);
                    di.standardValue.push_back(machTestItemInfo.at(j).standardValue);
                    di.maxValue.push_back(machTestItemInfo.at(j).maxValue);
                    di.minValue.push_back(machTestItemInfo.at(j).minValue);
                    di.deviation.push_back(machTestItemInfo.at(j).deviation);
                    di.minPassrate.push_back(machTestItemInfo.at(j).minPassrate);
                    di.maxLimit.push_back(machTestItemInfo.at(j).maxLimit);
                    _log.LOG_ERROR("ParseLineInfo  machine_test_item_info 【%s】获取测试项参数【成功】",di.Name.data());
                }
            }
        }
        else
            _log.LOG_ERROR("ParseLineInfo  machine_test_item_info 【%s】测试项参数为【空】",di.Name.data());
        di.currentOperationNo = operationNo;
        iterdi->second = di;
    }

    cout<<matchDev <<" "<<ConfigLoader::m_deviceNumcount<<endl;
    if((ConfigLoader::m_deviceNumcount != 0) && (matchDev == ConfigLoader::m_deviceNumcount))
    {
        gLine = li;
        g_deviceNum = ConfigLoader::m_deviceNumcount;
        return li;
    }
    else
    {
        _log.LOG_ERROR("ParseLineInfo  machineInfo 设备编码【不正确】");
        null.Si.Name = "设备编码或工作中心不正确，请检查";
        gLine = null;
        return null;
    }

}

string ParseLineInfo::GetTpmsHandleFormula()
{
    return m_formula;
}

int ParseLineInfo::getDispatchInfoByDB()
{
    currDistpatchList.clear();
    vector<DispatchItem> wholeDistpatchList;
    DispatchItem dispatchinfotmp;
    int dispatchnum = 0;
    MYSQL_RES* result;
    MYSQL_ROW row;
    MYSQL* m_connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(m_connection);
    string command = "select Dispatch_No,Part_No from dispatch_info order by Issued_Date DESC;";
    _log.LOG_INFO("ParseLineInfo 【getDispatchInfoByDB】 command = 【%s】", command.data());
    result = helper.LoadTable(m_connection,command);
    if(result != NULL)
    {
        while(row = mysql_fetch_row(result))
        {
            if((row[0] != NULL) && (row[1] != NULL))
            {
                dispatchinfotmp.Dispatch_No = row[0];
                dispatchinfotmp.Part_No = row[1];
                wholeDistpatchList.push_back(dispatchinfotmp);
                if(dispatchinfotmp.Part_No == currEditPartNo)
                {
                    currDistpatchList.push_back(dispatchinfotmp);
                }
                dispatchnum++;
            }
        }
        helper.MySQL_Release(result);
        int num = (int)currDistpatchList.size();
        if(num > 0)
        {
            dispatchnum = num;
        }
        else
        {
            currDistpatchList = wholeDistpatchList;
        }
    }
    else
    {
        dispatchnum = -1;
        _log.LOG_ERROR("ParseLineInfo 【getDispatchListByDB】 数据库查询失败 !");
    }
    helper.MySQL_Close(m_connection);
    delete m_connection;
    return dispatchnum;
}

