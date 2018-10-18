#include "DeviceThread.h"
#include "main.h"

string DeviceThread::BillNo = "";
string DeviceThread::productModel = "";
string DeviceThread::m_partNo = "";

DeviceThread::DeviceThread(DeviceInfo di)
{
    _di  = di;
    _debug=false;
    _stopprocess=false;
    _counter=0;
}

DeviceThread::DeviceThread()
{}

DeviceThread::~DeviceThread()
{
}

/**
 * @brief 将产品信息赋初值
 * @param 包含产品信息的对象引用
 */
void DeviceThread::InitProductInfo(PartTestItemInfo &pi)
{
    if(EnableSampleMode == 1)
    {
        pi.dispatchNo = "sam" + dispatchNo;
        pi.partNoId = "sam" + pi.partNoId;
        pi.partSeqNo = "sam" + pi.partSeqNo;
    }
    else
    {
        pi.dispatchNo = dispatchNo;
    }
    pi.partNo = partNo;
    pi.operationNo = operationNo;
//    pi.operationNo = "25";
    pi.enterUser = user;
}


/**
 * @brief 将产品信息保存到数据库中
 * @param 包含产品信息的对象引用
 */
bool DeviceThread::SaveProductInfo(PartTestItemInfo &pi)
{
    string sql;
    InitProductInfo(pi);
    MYSQL* m_connection = new MYSQL;
    MySQLHelper mysql(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    if(m_connection != NULL)
    {
        if(!mysql.MySQL_Init(m_connection))
        {
            mysql.MySQL_Close(m_connection);
            delete m_connection;
            return false;
        }

        sql = "insert into part_test_item_info_temp(Dispatch_No,Part_No,Part_No_Id,Part_Seq_No,Operation_No,";
        sql = sql + "MCH_Code,Test_Item_Code,Test_Item_Collect_Value,Test_Item_Eigen_Value,Test_Item_Judge_Result,Enter_User) ";
        sql = sql + "values("+"'" + (pi.dispatchNo) + "','" + (pi.partNo) + "','";
        sql = sql + (pi.partNoId) + "','" + pi.partSeqNo + "'," ;
        sql = sql + pi.operationNo + ",'" + pi.machCode+ "','" + pi.testItemCode+ "','";
        sql = sql + pi.testItemCollectValue+ "','"+ pi.testItemEigenValue+ "','"+ IntToString(pi.testItemJudgeResult) + "','" + pi.enterUser+"')";

//        printf("%s %s \n",_di.Name.data(), sql.data());
        _log.LOG_DEBUG("DeviceThread SaveProductInfo 设备【%s】 插入数据库命令【%s】",_di.Name.data(),sql.data());

        //执行sql语句，插入数据库
        bool result = mysql.InsertTable(m_connection,sql);
        if(!result)
        {
            _log.LOG_ERROR("DeviceThread SaveProductInfo MySQL InsertTable Error");
            mysql.MySQL_Close(m_connection);
            delete m_connection;
            return false;
        }
        mysql.MySQL_Close(m_connection);
        delete m_connection;
        _log.LOG_DEBUG("DeviceThread SaveProductInfo MySQL InsertTable finish !");
    }
    else
    {
        return false;
    }
    return true;
}


/**
 * @brief DeviceThread::FetchSmallBoardSeraialNo        扫其中一块小板序列号推导出另外11个小板序列号
 * @param barcode                                       暂定为第5块小板
 */
void DeviceThread::FetchSmallBoardSeraialNo(string barcode)
{
    vector<string> smallBoardSeraialNo;    
    int dec = 0;
    string prefix;
    string value;

    if(!barcode.empty() && barcode.length() > 4)
    {
        _log.LOG_DEBUG("DeviceThread FetchSmallBoardSeraialNo 基准小板序列号为【%s】",barcode.data());
        //修改：截取序列号前8位作为前缀（错误原因：9位16进制数转化为10进制int时超限）
        prefix = barcode.substr(0, 8);                       //序列号前8位保留
        value = barcode.substr(8, barcode.length() - 8);     //从后4位开始为具体的值
        dec = hextodec(value);
        for(int i = 0;i < 12;i++)
        {
            smallBoardSeraialNo.push_back(prefix + padZero(dectoHex(dec + i - 7), value.length()));      //暂定扫码器扫第8块小板
        }
        string data = ArrayJoint2String(smallBoardSeraialNo,"-");
        _log.LOG_DEBUG("DeviceThread FetchSmallBoardSeraialNo 小板序列号数组转为字符串【%s】",data.data());
        m_db.Write_TagMValue(SMALL_BORD_SERIAL_NO,data);
    }
    else
        _log.LOG_ERROR("DeviceThread FetchSmallBoardSeraialNo 序列号错误，长度小于4");
}


/**
 * @brief 处理采集值
 * @param pi
 * @param di
 * @param index
 * @return
 */
void DeviceThread::ProcessCValue(PartTestItemInfo &pi, int index, JudgeMethod type, string cValue)
{
    vector<double> dv;
    string values;
    double tmp=0;
    int count=0;
    vector<double> NormalValues;
    vector<double> LowValues;
    vector<double> HigthValues;
    int passCount=0;
    int lowCount=0;
    int hightCount=0;
    int passRate=0;
    double badValue=0;
    string testItemAvgValue;
    string testItemMaxValue;
    string testItemMinValue;

    NormalValues.clear();
    HigthValues.clear();
    LowValues.clear();

    if(!cValue.empty())
    {
        if(cValue.length() > 500)
        {
            cValue = cValue.substr(0,500);
            if(cValue.rfind("/") != string::npos)
            {
                values = cValue.substr(0, cValue.rfind("/"));
            }
        }
        else
        {
            values = cValue;
        }
        pi.testItemCollectValue = values;
        split(values,"/",dv);
        count = dv.size();
        testItemAvgValue = DoubleToString(Average(dv), "%.2f");
        testItemMaxValue = DoubleToString(Max(dv), "%.2f");
        testItemMinValue = DoubleToString(Min(dv), "%.2f");
        for(int i=0;i < count;i++)
        {
            if((dv[i] >= _di.minValue.at(index)) && (dv[i] <= _di.maxValue.at(index)))
            {
                passCount++;
                NormalValues.push_back(dv[i]);
            }
            else if (dv[i] < _di.minValue.at(index))
            {
                lowCount++;
                LowValues.push_back(dv[i]);
                badValue = dv[i];
            }
            else if (dv[i] > _di.maxValue.at(index))
            {
                hightCount++;
                HigthValues.push_back(dv[i]);
                badValue = dv[i];
            }
        }
        passRate = passCount * 100.0 / count;

        if(_di.judgeEneble.at(index) == 1)
        {
            switch (type)
            {
                //判定模式1，最低合格率判定(静态电流)
             case lowPassrate:
                if((fabs(_di.minValue.at(index)) <= EPSINON || fabs(_di.maxValue.at(index)) <= EPSINON) ||
                (_di.minValue.at(index) >= _di.maxValue.at(index)) || (_di.minPassrate.at(index) == 0))
                {
                    m_db.Write_TagMValue(_di.testParaAlarm, "1");
                    _log.LOG_ERROR("DeviceThread 【%s】 获取【测试项】参数【失败】",_di.Name.data());
                }

                if (passRate >= _di.minPassrate.at(index))
                {
                   pi.testItemJudgeResult = 1;
                   pi.testItemEigenValue = DoubleToString(Average(NormalValues), "%.2f");
                }
                else
                {
                   pi.testItemJudgeResult = 0;
                   pi.testItemEigenValue = DoubleToString(badValue, "%.2f");
                }
                break;
                //判定模式2，超最大值个数判定（发射电流）
             case maxLimit:
                if((fabs(_di.minValue.at(index)) <= EPSINON || fabs(_di.maxValue.at(index)) <= EPSINON) ||
                (_di.minValue.at(index) >= _di.maxValue.at(index)))
                {
                    m_db.Write_TagMValue(_di.testParaAlarm, "1");
                    _log.LOG_ERROR("DeviceThread 【%s】 获取【测试项】参数【失败】",_di.Name.data());
                }
                if (passRate != 0 && hightCount <= _di.maxLimit.at(index))        //至少一个值在范围内 --11.05
                    pi.testItemJudgeResult = 1;               
                else
                    pi.testItemJudgeResult = 0;
                pi.testItemEigenValue = testItemMaxValue;
                break;
                //判定模式3 气压测试
             case pressJudge:
                if((fabs(_di.standardValue.at(index)) <= EPSINON || fabs(_di.deviation.at(index)) <= EPSINON))
                {
                    m_db.Write_TagMValue(_di.testParaAlarm, "1");
                    _log.LOG_ERROR("DeviceThread 【%s】 获取【测试项】参数【失败】",_di.Name.data());
                }
                pi.testItemEigenValue = testItemAvgValue;
                if(fabs(atof(testItemAvgValue.data()) - _di.standardValue.at(index)) < _di.deviation.at(index))
                    pi.testItemJudgeResult = 1;
                else
                    pi.testItemJudgeResult = 0;
                break;
                //判定模式4 手柄测试
            case tpmsJudge:
                if((fabs(_di.standardValue.at(index)) <= EPSINON || fabs(_di.deviation.at(index)) <= EPSINON))
                {
                    m_db.Write_TagMValue(_di.testParaAlarm, "1");
                    _log.LOG_ERROR("DeviceThread 【%s】 获取【测试项】参数【失败】",_di.Name.data());
                }
               pi.testItemEigenValue = testItemAvgValue;
               //气压表的平均值
               tmp = atof((const char*)m_db.Read_TagMValue(_di.MAV).data());
               cout<<"--- pi.testItemEigenValue = "<<pi.testItemEigenValue<<" --- tmp = "<<tmp<<" --- di.deviation = "<<_di.deviation.at(index)<<endl;
               if(fabs(atof(testItemAvgValue.data()) - tmp) < _di.deviation.at(index))
                   pi.testItemJudgeResult = 1;
               else
                   pi.testItemJudgeResult = 0;
               break;
             case minLimit:    //判定模式5，超最小值个数判定
                if (passCount != 0 && lowCount <= _di.maxLimit.at(index))
                {
                    pi.testItemJudgeResult = 1;
                    pi.testItemEigenValue = testItemAvgValue;
                }
                else
                {
                    pi.testItemJudgeResult = 0;
                    pi.testItemEigenValue = DoubleToString(badValue, "%.2f");
                }
                break;
            default:
                pi.testItemJudgeResult = 0;
                pi.testItemEigenValue = testItemAvgValue;
                break;
            }
        }
        else if(_di.judgeEneble.at(index) == 0)
        {
            pi.testItemJudgeResult = 1;
        }
    }
}

void DeviceThread::SetProInfo(PartTestItemInfo m_pi)
{
    pi = m_pi;
}

PartTestItemInfo DeviceThread::GetProInfo()
{
    return pi;
}

DeviceInfo DeviceThread::GetDeviceInfo()
{
    return _di;
}

void DeviceThread::SetDeviceInfo(DeviceInfo m_di)
{
    _di = m_di;
}

void DeviceThread::SetProductModel(string productmodel)
{
    productModel = productmodel;
}

string DeviceThread::GetProductModel()
{
    return productModel;
}

void DeviceThread::SetPartNo(string partno)
{
    m_partNo = partno;
}

string DeviceThread::GetPartNo()
{
    return m_partNo;
}

void DeviceThread::SetBillNum(string billnum)
{
    BillNo = billnum;
}

string DeviceThread::GetBillNum()
{
    return BillNo;
}

vector<string> DeviceThread::GetSmallBoardSeraialNo()
{
    string data = m_db.Read_TagMValue(SMALL_BORD_SERIAL_NO);
    _log.LOG_DEBUG("DeviceThread GetSmallBoardSeraialNo 包含12块小板序列号字符串【%s】",data.data());
    vector<string> smallBoardSeraialNo;
    if(!data.empty())
    {
        splittostring(data,"-",smallBoardSeraialNo);
    }
    return smallBoardSeraialNo;
}

/**
 * @brief 单个流程中当需要设置设备多次工作时，把工作次数初始化为0，否则工作次数设置为-1
 * @param 需要多次工作则为true，否则为false
 * @param 表示工作次数的变量
 */
void DeviceThread::SetDeviceRepeatWork(bool flag)
{
    if(flag)
        repeatFlag = true;
    else
        repeatFlag = false;
}

/**
 * @brief DeviceThread::GetPartID           扫码器SR点，获得前工位产品ID
 * @return
 */
vector<string> DeviceThread::GetPartID()
{
    vector<string> partIdVector;
    vector<string> partSeqNoVector;

    partSeqNoVector = GetSmallBoardSeraialNo();

    MYSQL_RES* result;
    string command;
    MYSQL* connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(connection);
    if(partSeqNoVector.size() == 12)
    {
        for(int i = 0;i < partSeqNoVector.size();i++)
        {
            command.clear();
            command = "select * from product_status_info where Part_No = '" + partNo + "' and Part_Seq_No = '" + partSeqNoVector.at(i) + "' order by Record_Id desc limit 1;";
            result = helper.LoadTable(connection,command);
            if(result != NULL)
            {
                MYSQL_ROW row;
                row = mysql_fetch_row(result);
                if(row != NULL)
                    partIdVector.push_back(row[2]);
                else
                {
                    _log.LOG_DEBUG("DeviceThread GetPartID 数据库查询前工位ID为空 查询语句:【%s】",command.data());
                    partIdVector.push_back("");
                }
                helper.MySQL_Release(result);
            }
            else
            {
                _log.LOG_ERROR(" DeviceThread GetPartID 数据库查询前工位ID失败 查询语句:【%s】",command.data());
                partIdVector.push_back("");
            }
        }
    }
    else
    {
         _log.LOG_ERROR("DeviceThread GetPartID 取小板序列号数组错误，数组大小为:【%d】",partSeqNoVector.size());
    }
    helper.MySQL_Close(connection);
    delete connection;
    return partIdVector;
}

/**
 * @brief DeviceThread::getDeviceEnable
 * @return
 */
bool DeviceThread::getDeviceEnable()
{
    //设备不使能时,默认设备的判定结果为良品
    if(!_di.Enable)
    {
        m_db.Write_TagMValue(_di.JudgeResult,"1");
        _log.LOG_ERROR("DeviceThread 【%s】 设备未使能!",_di.Name.data());
        m_db.Write_TagMValue(_di.iValue, "设备未使能!!!");
        m_db.Write_TagMValue(_di.BarCode, "设备未使能!!!");
        return false;
    }
    return true;
}

/**
 * @brief DeviceThread::getDeviceNum        得到设备编号
 */
void DeviceThread::getDeviceNum()
{
    //得到设备编号
    _num = _di.Name.substr(0,1);      //1#电流表/2#电流表/3#电流表
    if(_num >= "1" && _num <= "9")
        _devicePosition = atoi(_num.data()) - 1;
    else
        _devicePosition = 0;
}

/**
 * @brief DeviceThread::noJudge
 */
void DeviceThread::noJudge()
{
    //设备使能为0时，不进行判定，结果置为1
    if(!_di.judgeEneble.empty() && _di.judgeEneble.at(0) == 0)
    {
        _log.LOG_DEBUG("DeviceThread 【%s】设备，judgeEnable=0，不参与判定，检测【合格】",_di.Name.data());
        m_db.Write_TagMValue(_di.JudgeResult,"1");
        pi.testItemJudgeResult = 1;
        pi.testItemEigenValue = "NA";
        m_db.Write_TagMValue(_di.iValue, "不进行判定,【" + _di.Name + "】检测合格");
        m_db.Write_TagMValue(_di.BarCode, "不进行判定,【" + _di.Name + "】检测合格");        
    }
}

/**
 * @brief DeviceThread::saveToSql
 * @param partNoId
 * @param partSeqNo
 */
void DeviceThread::saveToSql(string partNoId, string partSeqNo,int testItemCodeIndex)
{
    pi.partNoId = partNoId;
    pi.partSeqNo = partSeqNo;
    pi.machCode = _di.DeviceCode;
    if(!_di.testItemCode.empty())
    {
        pi.testItemCode = _di.testItemCode.at(testItemCodeIndex);
        if(!partNoId.empty() && !partSeqNo.empty())
            SaveProductInfo(pi);
    }
}

/**
 * @brief DeviceThread::sendValueToTcpServer
 * @param cValue
 */
void DeviceThread::sendValueToTcpServer(string cValue)
{
    int pos=0;
    pos = atoi(_di.testStartPos.data());
    for(int i = pos;i < _di.testItemCode.size() + pos;i++)
    {
        string sCollectValue = _num + "$" + "CV" + IntToString(i);
        m_db.Write_TagMValue(sCollectValue, cValue);
    }
}

/**
 * @brief DeviceThread::processEndFlag              处理信号结束点
 */
void DeviceThread::processEndFlag()
{
    _endFlag = m_db.Read_TagMValue(_di.EndFlag);
    //开始信号为"1"时，清空遗留信息
    if((!_endFlag.empty()) && (!_endFlag.compare("1")))
    {
         usleep(500*1000);
         _log.LOG_DEBUG("DeviceThread 【%s】检测到结束采集信号，清空序列号，ID，复位结束采集信号点",_di.Name.data());
         m_db.Write_TagMValue(_di.EndFlag,"0");
         m_db.Write_TagMValue(_di.IdFlag,"");
         m_db.Write_TagMValue(_di.SnFlag,"");
         m_db.Write_TagMValue(_di.JudgeResult, "");
         m_db.Write_TagMValue(_di.BarCode,"");
         m_db.Write_TagMValue(gLine.Si.JudgeResult, "");
         m_db.Write_TagMValue(_di.iValue, "");
         pi.Clear();
    }
}

