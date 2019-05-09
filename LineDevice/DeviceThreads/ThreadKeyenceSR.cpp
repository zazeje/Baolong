#include "ThreadKeyenceSR.h"
#include "main.h"

int ThreadKeyenceSR::ProductCount = 0;
vector<string> PriorPartIdVector;

#ifdef _Debug
vector<string> prevbarcode = {"2133606610","956105754","1727488279","264578189","1299759286","653424105","2091641445","707787670","1646795234","1087587117","1912369695","1411760599","1280203741","97884198","2057493654","1130491132","1185877380","2070114195","1773333470"};
vector<string> prevPartID = {"54010383","54010384","54010385","54010386","54010387","54010388","54010389","5401038A","5401038B","5401038C","5401038D","5401038E","5401038F","54010390","54010391","54010392","54010393","54010394","54010395"};
int barcodeindex = 0;
#endif

/**
 * @brief ThreadKeyenceSR::ThreadKeyenceSR          构造函数
 * @param di
 */
ThreadKeyenceSR::ThreadKeyenceSR(DeviceInfo di): DeviceThread(di)
{
    if (di.Port.empty()) _di.Port = "9004";
    myDevice = new KeyenceSR(atoi(_di.Port.data()),_di.Ip,_di.Name);
    _stopprocess = false;
}

/**
 * @brief 析构函数
 */
ThreadKeyenceSR::~ThreadKeyenceSR()
{
    if(myDevice != NULL)
    {
        delete myDevice;
        myDevice = NULL;
    }
}


/**
 * @brief ThreadKeyenceSR::threadprocess            驱动线程处理函数
 */
void ThreadKeyenceSR::threadprocess()
{
    _log.LOG_INFO("ThreadKeyenceSR 【%s】线程驱动已启动 ......", _di.Name.data());

    //得到设备编号
    getDeviceNum();                     //1#扫码器/2#扫码器
    //获取PLC“扫码不良”点位
    m_scanOk = D3GetPLCScanOK();

    for(int i=0;i<m_scanOk.size();i++)
        cout<<"i = "<<i<<" -- plcNG = "<<m_scanOk.at(i)<<endl;

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
               _controlFlag = true;
               Tag tag = im->second;
               string name = tag.TagCode;
               tag.MemortValue = m_db.Read_TagMValue(tag.TagName);

               //烧程工位(扫序列号)
               if (!name.compare("D3SC1") || !name.compare("D3SC2") || !name.compare("D3SC3") || !name.compare("D3SC4"))
               {
                   if(!tag.MemortValue.compare("1"))
                   {
                       _log.LOG_DEBUG("ThreadKeyenceSR 【%s】检测到【%s】信号 ",_di.Name.data(),name.data());
                       clearData(tag.TagName);
#ifndef _Debug
                       m_barCode = myDevice->AutoScan(1);
#else
                       m_barCode = IntToString(rand());
#endif
                       processScanD3SC(name);
                       //界面显示产品个数
                       StationInfo::ProductCount++;
                   }
               }
               //电池组装工位（视觉检测）
               else if(!name.compare("D3SV"))
               {
                   if(!tag.MemortValue.compare("1"))
                   {
                      _log.LOG_DEBUG("ThreadKeyenceSR 【%s】检测到【%s】信号 ",_di.Name.data(),name.data());
                      clearData(tag.TagName);
                       //开始扫码，扫不到时最长持续扫3秒
#ifndef _Debug
                      m_barCode = myDevice->AutoScan(1);
#else
                    if(barcodeindex == 0)
                    {
                        m_barCode = prevPartID.at(prevPartID.size() - 1);
                    }
                    else
                    {
                        m_barCode = prevPartID.at(barcodeindex - 1);
                    }
#endif
                      string partId = m_db.Read_TagMValue(_di.IdFlag);
                      string partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                       _log.LOG_DEBUG("ThreadKeyenceSR 【%s】 获取id为【%s】 序列号为【%s】",_di.Name.data(),partId.data(),partSeqNo.data());
                       processScanD3SV(partId, partSeqNo);
                       if(_di.testItemCode.size() > 0)
                           pi.testItemCode = _di.testItemCode.at(0);
                       //插入数据到数据库
                       saveToSql(partId, partSeqNo,0);
                       //上传数据至上位机
                       sendValueToTcpServer(pi.testItemEigenValue);
                     }
               }
               //扫序列号(电流测试、分拣电池组装)
               else if(!name.compare("D3SQ") || !name.compare("D3SQ1") || !name.compare("D3SQ2") || !name.compare("D3SQ3") || !name.compare("D3SQ4"))
               {
                   if(!tag.MemortValue.compare("1"))
                   {
                       _log.LOG_DEBUG("ThreadKeyenceSR 【%s】检测到【%s】信号 ",_di.Name.data(),name.data());
                       clearData(tag.TagName);
                       //开始扫码，扫不到时最长持续扫3秒
#ifndef _Debug
                      m_barCode = myDevice->AutoScan(1);
#else
                      for(int i = barcodeindex;i < prevbarcode.size();i++)
                      {
                        m_barCode = prevbarcode.at(i);
                        barcodeindex++;
                        if(barcodeindex == prevbarcode.size())
                        {
                            barcodeindex = 0;
                        }
                        break;
                      }
#endif
                       processScanD3SQ(name);
                       //界面显示产品个数
                       StationInfo::ProductCount++;
                   }
               }
               //扫产品ID（电池焊接工位、涂覆工位、热熔、气压、打标、功率、侧漏、分拣）
               else if(!name.compare("D3SD") || !name.compare("D3SD1") || !name.compare("D3SD2") || !name.compare("D3SD3") || !name.compare("D3SD4") ||\
                       !name.compare("D3SD5") ||!name.compare("D3SD6") ||!name.compare("D3SD7") ||!name.compare("D3SD8"))
               {
                   if(!tag.MemortValue.compare("1"))
                   {
                       _log.LOG_DEBUG("ThreadKeyenceSR 【%s】检测到【%s】信号 ",_di.Name.data(),name.data());
                       clearData(tag.TagName);
                       //开始扫码，扫不到时最长持续扫3秒
#ifndef _Debug
                        m_barCode = myDevice->AutoScan(1);
#else
                      for(int i = barcodeindex;i < prevPartID.size();i++)
                      {
                        m_barCode = prevPartID.at(i);
                        barcodeindex++;
                        if(barcodeindex == prevPartID.size())
                        {
                            barcodeindex = 0;
                        }
                        break;
                      }
#endif
                       processScanD3SD(name);
                       //界面显示产品个数
                       StationInfo::ProductCount++;

                       if(g_dbInfo.GetWorkCenterNo() == "3116"){
                           string jr = m_db.Read_TagMValue(_di.Prefix + "SLR");
                           if(jr == "1")
                           {
                               pi.testItemJudgeResult = 1;
                               pi.testItemEigenValue = "OK";
                               m_db.Write_TagMValue(_di.JudgeResult,"1");
                               m_db.Write_TagMValue(_di.BarCode , "测漏良品");
                               _log.LOG_DEBUG("ThreadKeyenceSR 【%s】设备 【测漏良品】",_di.Name.data());
                               printf("【%s】 %s 测漏良品 \n",_di.Name.data(),_di.DeviceCode.data());
                           }
                           else
                           {
                               pi.testItemJudgeResult = 0;
                               pi.testItemEigenValue = "NG";
                               m_db.Write_TagMValue(_di.JudgeResult,"0");
                               _log.LOG_DEBUG("ThreadKeyenceSR 【%s】设备 【测漏不良】",_di.Name.data());
                               m_db.Write_TagMValue(_di.BarCode, "测漏不良");
                           }
                           //设备判定使能为0时，直接将判定结果置为1
                           if(!_di.judgeEneble.empty() && _di.judgeEneble.at(0) == 0)
                           {
                               _log.LOG_ERROR("ThreadKeyenceSR 【%s】 判定使能为 0 ",_di.Name.data());
                               m_db.Write_TagMValue(_di.JudgeResult,"1");
                               pi.testItemJudgeResult = 1;
                               pi.testItemEigenValue = "NA";
                           }
                           pi.partNoId = m_barCode;
                           pi.partSeqNo = m_db.Read_TagMValue(_di.SnFlag);
                           pi.operationNo = _di.currentOperationNo;
                           pi.machCode = _di.DeviceCode;
                           if(_di.testItemCode.size() > 0)
                               pi.testItemCode = _di.testItemCode.at(0);
                           if(!pi.partSeqNo.empty())
                                SaveProductInfo(pi);
                       }
                   }
               }
               //外壳打标工位（2#扫码器--扫ID不给114置1）
               else if(!name.compare("D3NG"))
               {
                   if(!tag.MemortValue.compare("1"))
                   {
                       _log.LOG_DEBUG("ThreadKeyenceSR 【%s】检测到【%s】信号 ",_di.Name.data(),name.data());
                       clearData(tag.TagName);
#ifndef _Debug
                        m_barCode = myDevice->AutoScan(1);
#else
                       if(barcodeindex == 0)
                       {
                           m_barCode = prevPartID.at(prevPartID.size() - 1);
                       }
                       else
                       {
                           m_barCode = prevPartID.at(barcodeindex - 1);
                       }
#endif
                       processScanD3NG();
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
               if(!tag.CTagNames.empty() && _controlFlag)
               {
                   for(list<string>::iterator it = tag.CTagNames.begin();it != tag.CTagNames.end();it++)
                   {
                       string key = *it;
                       m_db.Write_TagMValue(key,tag.MemortValue);                      
                   }
               }
           }
       }
       _counter++;
    }
    _log.LOG_INFO("ThreadKeyenceSR 【%s】 线程驱动已停止.....",_di.Name.data());
}

/**
 * @brief 驱动线程处理接口
 * @param arg
 * @return
 */
void *ThreadKeyenceSR::Start_Thread(void* arg)
{
    ThreadKeyenceSR *th = (ThreadKeyenceSR*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief 启动驱动线程
 * @return
 */
bool ThreadKeyenceSR::Start()
{
    pthread_attr_t pth_attr;
    pthread_attr_init(&pth_attr);
    pthread_attr_setdetachstate(&pth_attr,PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&pth,&pth_attr,ThreadKeyenceSR::Start_Thread,this);
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
bool ThreadKeyenceSR::Stop()
{
    _stopprocess = true;
    myDevice->Close();
    myDevice->CloseReconThread();
    return true;
}


/**
 * @brief ThreadKeyenceSR::getJudgeResult
 * @param barcode
 * @param type
 * @return
 */
string ThreadKeyenceSR::getJudgeResult(string barcode, KeyencePara type)
{
    string partNoId,priJudgeResult,seqNo;
    MYSQL_RES* result;
    MYSQL* connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(connection);
    string command;

    if(type == SeqtoPartId)       //通过序列号PartSeqenceNo来查找ID,写入内存数据库
    {
        command.append("select * from product_status_info where Part_No = '").append(partNo)\
                .append("' and Part_Seq_No = '").append(barcode).append("';");
        result = helper.LoadTable(connection,command);
        if(result != NULL)
        {
            MYSQL_ROW row;
            while(row = mysql_fetch_row(result))
            {
                partNoId = row[2];
                priJudgeResult = row[4];
            }
        }
        else
        {
            _log.LOG_ERROR("ThreadKeyenceSR 【%s】 通过序列号查询ID失败,查询语句为【%s】",_di.Name.data(),command.data());
        }
        _log.LOG_DEBUG("ThreadKeyenceSR 【%s】 SQ 查询到ID值为【%s】,前工位判定结果为【%s】",_di.Name.data(),partNoId.data(),priJudgeResult.data());
        if(!partNoId.empty())
            m_db.Write_TagMValue(_di.IdFlag, partNoId);        //把产品ID写入IdFlag，打标机打印
        else
            m_db.Write_TagMValue(_di.IdFlag, "");

        string sId = _num + "$" + "ID";
        m_db.Write_TagMValue(sId, partNoId);
        string sSeqNo = _num + "$" + "SQ";
        m_db.Write_TagMValue(sSeqNo, barcode);

    }
    else if(type == PartIdtoSeq)         //通过序列号PartID来查找PartSeqenceNo,写入内存数据库
    {
        command.append("select * from product_status_info where Part_No = '").append(partNo)\
                .append("' and Part_No_Id = '").append(barcode).append("';");
        result = helper.LoadTable(connection,command);
        if(result != NULL)
        {
            MYSQL_ROW row;
            while(row = mysql_fetch_row(result))
            {
                seqNo = row[3];
                priJudgeResult = row[4];
            }
        }
        else
        {
             _log.LOG_DEBUG("ThreadKeyenceSR 【%s】 通过ID查询序列号失败,查询语句为【%s】",_di.Name.data(),command.data());
        }

        _log.LOG_DEBUG("ThreadKeyenceSR 【%s】 设备，SD 查询到序列号值为【%s】,前工位判定结果为【%s】",_di.Name.data(),seqNo.data(),priJudgeResult.data());
        m_db.Write_TagMValue(_di.SnFlag, seqNo);

        //tcpServer
        string sId = _num + "$" + "ID";
        m_db.Write_TagMValue(sId, barcode);
        string sSeqNo = _num + "$" + "SQ";
        m_db.Write_TagMValue(sSeqNo, seqNo);
    }

    string sPos = _num + "$" + "PS";
    m_db.Write_TagMValue(sPos, _num);

    helper.MySQL_Release(result);
    helper.MySQL_Close(connection);
    delete connection;
    return priJudgeResult;
}

/**
 * @brief ThreadKeyenceSR::getPriorStationJudgeResult       D2线获得上一个工位的判定结果
 * @return                                                  成功返回上一个工位的判定结果；否则返回空字符串
 */
string ThreadKeyenceSR::getPriorStationJudgeResult(string barcode, KeyencePara type)
{
    return getJudgeResult(barcode, type);;
}

/**
 * @brief ThreadKeyenceSR::GetStationValue      获取工位数据记录
 * @param barcode                               ID
 * @return
 */
bool ThreadKeyenceSR::GetStationValue(string barcode)
{
    bool res = false;
    MYSQL_RES* result;
    MYSQL* connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(connection);
    string command = "select * from part_test_item_info where Part_No_Id = '" + barcode + "' and Operation_No = '" + operationNo + "' and part_no = '" + partNo + "';";
    result = helper.LoadTable(connection,command);
    if(result != NULL)
    {
        MYSQL_ROW row;
        row = mysql_fetch_row(result);
        if(row != NULL)
            res = true;
        else
            _log.LOG_ERROR("ThreadKeyenceSR 【%s】 当前工位采集值为空",_di.Name.data());
        helper.MySQL_Release(result);
    }
    else
    {
        _log.LOG_ERROR("ThreadKeyenceSR 【%s】查询工位数据记录失败, 查询语句为【%s】",_di.Name.data(),command.data());
    }

    helper.MySQL_Close(connection);
    delete connection;
    return res;
}


string ThreadKeyenceSR::D3GetBarCode(string barcode, KeyencePara type)
{
    MYSQL_RES* result;
    MYSQL* connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(connection);
    string command;
    string value;
    MYSQL_ROW row;

    if(type == SeqtoPartId)       //通过序列号PartSeqenceNo来查找ID,写入内存数据库
    {
        command.append("select Part_No_Id from part_test_item_info where Part_No = '").append(partNo)\
                .append("' and Part_Seq_No = '").append(barcode).append("' order by Record_Id desc limit 1;");
        _log.LOG_DEBUG("ThreadKeyenceSR D3GetBarCode 【%s】设备  command = 【%s】",_di.Name.data(),command.data());
        result = helper.LoadTable(connection,command);
        if(result != NULL)
        {
            row = mysql_fetch_row(result);
            if(row!=NULL)
                value = row[0];            
            else            
                value = "";            
        }
        else
        {
            _log.LOG_ERROR("ThreadKeyenceSR 【%s】 通过序列号查询ID【失败】",_di.Name.data());
        }
        _log.LOG_DEBUG("ThreadKeyenceSR 【%s】 SeqtoPartId 查询到ID值为【%s】",_di.Name.data(),value.data());

//        string sId = _num + "$" + "ID";
//        m_db.Write_TagMValue(sId, value);
//        string sSeqNo = _num + "$" + "SQ";
//        m_db.Write_TagMValue(sSeqNo, barcode);
    }
    else if(type == PartIdtoSeq)         //通过序列号PartID来查找PartSeqenceNo,写入内存数据库
    {
        command.append("select Part_Seq_No from part_test_item_info where Part_No = '").append(partNo)\
                .append("' and Part_No_Id = '").append(barcode).append("' order by Record_Id desc limit 1;");
        _log.LOG_DEBUG("ThreadKeyenceSR D3GetBarCode 【%s】设备  command = 【%s】",_di.Name.data(),command.data());
        result = helper.LoadTable(connection,command);
        if(result != NULL)
        {
            row = mysql_fetch_row(result);
            if(row!=NULL)
                value = row[0];           
            else
                value = "";
        }
        else
        {
             _log.LOG_DEBUG("ThreadKeyenceSR 【%s】 通过ID查询序列号【失败】",_di.Name.data());
        }

        _log.LOG_DEBUG("ThreadKeyenceSR 【%s】设备 PartIdtoSeq 查询到序列号值为【%s】",_di.Name.data(),value.data());

//        //tcpServer
//        string sId = _num + "$" + "ID";
//        m_db.Write_TagMValue(sId, barcode);
//        string sSeqNo = _num + "$" + "SQ";f
//        m_db.Write_TagMValue(sSeqNo, value);f
    }

//    string sPos = _num + "$" + "PS";
//    m_db.Write_TagMValue(sPos, _num);

    helper.MySQL_Release(result);
    helper.MySQL_Close(connection);
    delete connection;
    return value;
}

/**
 * @brief ThreadKeyenceSR::D3GetPriorStationJR      判断前工位判定结果
 * @param flag                                      要求传入字符串‘1’或者‘0’，以产品ID识别产品时为 1，以产品序列号识别产品时为 2
 * @param barCode                                   扫码值
 * @return                                          查询结果为 1 时表示为良品，为 0 时表示为不良品
 */
string ThreadKeyenceSR::D3GetPriorStationJR(string flag, string barCode)
{
    //CALL `get_product_status`('1', '96216708', '0A062950', '10')
    string jr = "0";
    MYSQL_ROW row;
    MYSQL_RES* result;
    MYSQL* m_connection = new MYSQL;
    MySQLHelper helper(g_dbInfo.GetHostIP(),g_dbInfo.GetDBUserName(),g_dbInfo.GetPassword(),g_dbInfo.GetDatabaseName());
    helper.MySQL_Init(m_connection);
    string command = "CALL `get_product_status`('" + flag + "','" + partNo + "','" + barCode + "'," + operationNo +")";

    _log.LOG_DEBUG("ThreadKeyenceSR D3GetPriorStationJR 查询命令command = 【%s】",command.data());
    result = helper.LoadTable(m_connection,command);
    if(result != NULL)
    {
        while(row = mysql_fetch_row(result))
        {
            if(row[0]!=NULL)
                jr = row[0];
        }
        helper.MySQL_Release(result);
    }
    else
    {
        _log.LOG_ERROR("ThreadKeyenceSR 【%s】 取前工位判定结果【失败】",_di.Name.data());
    }

    _log.LOG_DEBUG("ThreadKeyenceSR 【%s】 前工位判定结果为【%s】",_di.Name.data(),jr.data());
    helper.MySQL_Close(m_connection);
    delete m_connection;
    return jr;
}


/**
 * @brief ThreadKeyenceSR::D3GetPLCScanNG               获取PLC“扫码良品”点位
 * @return                                              返回PLC“扫码良品”点位
 */
vector<string> ThreadKeyenceSR::D3GetPLCScanOK()
{
    vector<string> plcScanOk;
    for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++)
    {
        DeviceInfo di = it->second;
        if(di.Name == "1#PLC")
        {
            for(map<string, UnitInfo>::iterator it = di.Units.begin();it != di.Units.end();it++)
            {
                map<string, Tag> tags = it->second.Tags;
                for(map<string, Tag>::iterator im = tags.begin();im != tags.end();im++)
                {
                    Tag tag = im->second;
                    if(tag.TagCode == "NG" || tag.TagCode == "NG1" || tag.TagCode == "NG2")
                        plcScanOk.push_back(tag.TagName);
                }
            }
        }
    }
    return plcScanOk;
}

/**
 * @brief ThreadKeyenceSR::GetPlcJR
 * @return
 */
string ThreadKeyenceSR::GetPlcJR()
{
    string plcJR;
    for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++)
    {
        DeviceInfo di = it->second;
        if(di.Name == "1#PLC")
        {
            plcJR = di.JudgeResult;
        }
    }
    return plcJR;
}

/**
 * @brief ThreadKeyenceSR::clearData                清除数据
 * @param tagName
 */
void ThreadKeyenceSR::clearData(string tagName)
{
    m_barCode.clear();
    m_value.clear();
    m_db.Write_TagMValue(tagName, "0");
}

/**
 * @brief ThreadKeyenceSR::checkBarCodeState        检查扫码结果状态
 * @return                                          扫码成功返回true；失败返回error
 */
bool ThreadKeyenceSR::checkBarCodeState()
{
    if(StringToUpper(m_barCode).find("ERROR") != std::string::npos || m_barCode.empty())
    {
         _log.LOG_ERROR("ThreadKeyenceSR 【%s】扫码【失败】,读扫码值为空!",_di.Name.data());
         m_barCode="";
         m_db.Write_TagMValue(_di.BarCode, "扫码失败");
         pi.testItemJudgeResult = 0;
         m_db.Write_TagMValue(_di.JudgeResult,"0");
         return false;
    }
    return true;
}


/**
 * @brief ThreadKeyenceSR::processScanD3SC              处理烧程工位(扫序列号)信号
 * @param name
 */
void ThreadKeyenceSR::processScanD3SC(string name)
{
    if(checkBarCodeState())
    {
         _log.LOG_DEBUG("ThreadKeyenceSR 【%s】扫码【成功】,扫码结果为【%s】", _di.Name.data(),m_barCode.data());
         m_db.Write_TagMValue(_di.BarCode, "扫码结果为：【" + m_barCode + "】");            //界面显示
         if(m_scanOk.size() == 1)
         {
               m_db.Write_TagMValue(m_scanOk.at(0), "1");          //扫码良品，向PLC扫码不良点（M114）置位
         }
    }
    getPointIndex(name);
    m_db.Write_TagMValue(_di.SnFlag + m_pointIndex, m_barCode);

//    if(name.length() == 5)      //name == D3SC1/D3SC2/D3SC3/D3SC4
//    {
//        string stationNum = name.substr(name.length()-1, 1);
//        m_db.Write_TagMValue(_di.SnFlag + stationNum, m_barCode);
//    }
}

/**
 * @brief ThreadKeyenceSR::processScanD3SV                  处理电池组装工位（视觉检测）信号
 * @param partId
 * @param partSeqNo
 */
void ThreadKeyenceSR::processScanD3SV(string partId, string partSeqNo)
{
    if(!partId.empty() && !partSeqNo.empty())
    {
        if(checkBarCodeState())
        {
            _log.LOG_DEBUG("ThreadKeyenceSR 【%s】 D3SV扫码【成功】,扫码值为【%s】,获取到的ID值为【%s】",_di.Name.data(),m_barCode.data(),m_db.Read_TagMValue(_di.IdFlag).data());
            //处理视觉检测，进行判定
            processVisionCheck();
            //设备判定使能为0时，直接将判定结果置为1
            noJudge();
        }
    }
    else
    {
        m_db.Write_TagMValue(_di.JudgeResult, "0");
        _log.LOG_DEBUG("ThreadKeyenceSR 【%s】 获取id为【空】，不进行测试，直接判定为【不良品】",_di.Name.data());
        m_db.Write_TagMValue(_di.BarCode,"1#扫码器扫码【失败】，不进行视觉测试");
    }
}

/**
 * @brief ThreadKeyenceSR::processScanD3SQ                  处理（电流测试、电池组装）工位信号
 * @param name
 */
void ThreadKeyenceSR::processScanD3SQ(string name)
{
    //m_barCode为序列号
    //m_value为ID
    if(checkBarCodeState())
    {
        _log.LOG_DEBUG("ThreadKeyenceSR 【%s】扫码【成功】,扫码结果为【%s】",_di.Name.data(), m_barCode.data());
        m_db.Write_TagMValue(_di.BarCode, "扫码结果为：【" + m_barCode + "】");
        if(EnableSampleMode == 1)
        {
            m_value = "12345678";
            dealWithScanOkPoint(name);
        }
        else
        {
            //获取产品ID
            m_value = D3GetBarCode(m_barCode, SeqtoPartId);
            //检测前工位操作
            checkPriorStationAction(name, "2");
        }
    }
    //写入序列号、ID到内存缓冲区
    writeToSeqNoAndID(name, m_barCode, m_value);
    //上传序列号、ID、位置给上位机
    sendSeqNoAndIDToTcpServer(m_barCode, m_value);

}

/**
 * @brief ThreadKeyenceSR::processScanD3SD            处理（电池焊接工位、涂覆工位、热熔、气压、打标、功率、侧漏、分拣）
 * @param name
 */
void ThreadKeyenceSR::processScanD3SD(string name)
{
    //m_barCode为ID
    //m_value为序列号
    if(checkBarCodeState())
    {
        m_db.Write_TagMValue(_di.BarCode, "扫码结果为：【" + m_barCode + "】");
        _log.LOG_DEBUG("ThreadKeyenceSR 【%s】扫码【成功】,扫码结果为【%s】",_di.Name.data(),m_barCode.data());
        if(EnableSampleMode == 1)
        {
            m_value = m_barCode;
            dealWithScanOkPoint(name);
        }
        else
        {
            //获取序列号
            m_value = D3GetBarCode(m_barCode, PartIdtoSeq);
            //检测前工位操作
            checkPriorStationAction(name, "1");
        }
    }
    //写入序列号、ID到内存缓冲区
    writeToSeqNoAndID(name, m_value, m_barCode);
    //上传序列号、ID、位置给上位机
    sendSeqNoAndIDToTcpServer(m_value, m_barCode);
}


/**
 * @brief ThreadKeyenceSR::dealWithScanOkPoint                   向PLC扫码良品点置位
 * @param name
 */
void ThreadKeyenceSR::dealWithScanOkPoint(string name)
{
    //只有1个扫码不良点
    if(m_scanOk.size() == 1)
    {
          m_db.Write_TagMValue(m_scanOk.at(0), "1");          //扫码良品，向PLC扫码良品点（M114）置位
    }
    //有2个扫码不良点
    else if(m_scanOk.size() == 2)
    {
         getPointIndex(name);
         if(m_pointIndex == "1")
             m_db.Write_TagMValue(m_scanOk.at(0), "1");          //扫码良品，向PLC扫码良品点（M114）置位
         else if(m_pointIndex == "2")
             m_db.Write_TagMValue(m_scanOk.at(1), "1");          //扫码良品，向PLC扫码良品点（M124）置位
    }
}

/**
 * @brief ThreadKeyenceSR::writeToSeqAndID                      写入序列号、ID到内存缓冲区
 * @param name
 * @param stationNum
 */
void ThreadKeyenceSR::writeToSeqNoAndID(string name, string partSeqNo, string partNoID)
{
    getPointIndex(name);
    m_db.Write_TagMValue(_di.SnFlag + m_pointIndex, partSeqNo);
    m_db.Write_TagMValue(_di.IdFlag + m_pointIndex, partNoID);
}


/**
 * @brief ThreadKeyenceSR::processScanD3NG                  处理（外壳打标）工位信号
 */
void ThreadKeyenceSR::processScanD3NG()
{
//    if(StringToUpper(barCode).find("ERROR") != std::string::npos || barCode.empty())
    if(checkBarCodeState())
    {
        m_db.Write_TagMValue(_di.BarCode, m_barCode);
        _log.LOG_DEBUG("ThreadKeyenceSR 【%s】扫码【成功】,扫码结果为【%s】",_di.Name.data(),m_barCode.data());
        if(EnableSampleMode == 1)
        {
            m_value = m_barCode;
        }
        else
        {
            //获取序列号
            m_value = D3GetBarCode(m_barCode, PartIdtoSeq);
        }
    }

    m_db.Write_TagMValue(_di.IdFlag, m_barCode);
    m_db.Write_TagMValue(_di.SnFlag, m_value);
}

/**
 * @brief ThreadKeyenceSR::getPointIndex                    获取扫码点名下标
 * @param name                                              D3SQ1/D3SQ2/D3SQ3/D3SQ4
 */
void ThreadKeyenceSR::getPointIndex(string name)
{
    if(name.length() > 4)
        m_pointIndex = name.substr(name.length()-1, 1);
    else
        m_pointIndex = "";
}

/**
 * @brief ThreadKeyenceSR::sendSeqNoAndIDToTcpServer            上传序列号、ID、位置给上位机
 * @param partNoId
 * @param partSeqNo
 */
void ThreadKeyenceSR::sendSeqNoAndIDToTcpServer(string partSeqNo, string partNoId)
{
    if(!m_pointIndex.empty())
    {
        //有多个扫码点
        string sId = m_pointIndex + "$" + "ID";
        m_db.Write_TagMValue(sId, partNoId);
        string sSeqNo = m_pointIndex + "$" + "SQ";
        m_db.Write_TagMValue(sSeqNo, partSeqNo);
        string sPos = m_pointIndex + "$" + "PS";
        m_db.Write_TagMValue(sPos, m_pointIndex);
    }
    else
    {
        //只有1个扫码点
        string sId = _num + "$" + "ID";
        m_db.Write_TagMValue(sId, partNoId);
        string sSeqNo = _num + "$" + "SQ";
        m_db.Write_TagMValue(sSeqNo, partSeqNo);
        string sPos = _num + "$" + "PS";
        m_db.Write_TagMValue(sPos, _num);
    }
}

/**
 * @brief ThreadKeyenceSR::checkPriorStationAction                  检测前工位操作
 * @param name                                                      扫码点名字
 * @param flag                                                      要求传入字符串‘1’或者‘0’，以产品ID识别产品时为 1，以产品序列号识别产品时为 2
 */
void ThreadKeyenceSR::checkPriorStationAction(string name, string flag)
{
    if(PBoxConfig::m_checkJRFlag == true)
    {
        if(D3GetPriorStationJR(flag,m_barCode) == "1")
        {
            dealWithScanOkPoint(name);
            //针对与分拣工位，需要放开，向上位机上传信息
//            m_db.Write_TagMValue(_num + "$" + "MG", "良品");
        }
        else
        {
            m_db.Write_TagMValue(_di.BarCode, "获取前工位为【不良品】，不置位【M114】");
            _log.LOG_ERROR("ThreadKeyenceSR 【%s】 扫码结果为：【%s】，获取前工位为【不良品】",_di.Name.data(), m_barCode.data());
//            m_db.Write_TagMValue(_num + "$" + "MG", "不良品");
        }
    }
    else
    {
        dealWithScanOkPoint(name);
    }
}

/**
 * @brief ThreadKeyenceSR::processVisionCheck               处理视觉检测信号，进行判定
 */
void ThreadKeyenceSR::processVisionCheck()
{
    if(_di.testItemCode.size() > 0)
    {
        //工艺判定
        if(m_barCode == m_db.Read_TagMValue(_di.IdFlag))
        {
            _log.LOG_DEBUG("ThreadKeyenceSR 【%s】 ID一致，【视觉检测通过】",_di.Name.data());
            m_db.Write_TagMValue(_di.BarCode , "ID一致，【视觉检测通过】");
            pi.testItemJudgeResult = 1;
            pi.testItemEigenValue = "OK";
            m_db.Write_TagMValue(_di.JudgeResult,"1");
        }
        else
        {
            _log.LOG_ERROR("ThreadKeyenceSR 【%s】 ID不一致，【视觉检测未通过】",_di.Name.data());
            m_db.Write_TagMValue(_di.BarCode , "ID不一致，【视觉检测未通过】");
            pi.testItemJudgeResult = 0;
            pi.testItemEigenValue = "NG";
            m_db.Write_TagMValue(_di.JudgeResult,"0");
        }
    }
    else
    {
        m_db.Write_TagMValue(_di.testParaAlarm, "1");
        _log.LOG_DEBUG("ThreadKeyenceSR 【%s】 缺少【测试项判定参数】",_di.Name.data());
        m_db.Write_TagMValue(_di.BarCode, "缺少【测试项判定参数】");
    }
}

/**
 * @brief ThreadKeyenceSR::CommunicateTest                      通信检测
 */
void ThreadKeyenceSR::CommunicateTest()
{
    _connectstatus = myDevice->CanAcess() ? "1" : "0";
    m_db.Write_TagMValue(_di.Alarm, _connectstatus);
    if(_connectstatus == "0")
    {
        _log.LOG_ERROR("ThreadKeyenceSR 【%s】通信检测【失败】 ",_di.Name.data());
        m_db.Write_TagMValue(_num + "$" + "NT",_di.Name + " 连接异常");
    }
    else
        m_db.Write_TagMValue(_num + "$" + "NT","");
}








