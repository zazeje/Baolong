#include "ConfigLoader.h"

int g_deviceNum = 0;
int testCodeNum = 0;
int ConfigLoader::m_deviceNumcount = 0;
ConfigLoader::ConfigLoader()
{
}

#if 1
/**
 * @brief 测试函数
 */
void ConfigLoader::test()
{
    ConfigLoader *configloader = new ConfigLoader();
    LineInfo li =  configloader->LoadConfig("./PBOX.xml");
    printf("Line name=%s\n",li.Name.data());
     StationInfo si;
     si = li.Si;
     printf("Station name = %s  code = %s  StartFlag =%s SnFlag = %s IdFlag = %s\n",\
          si.Name.data(),si.Code.data(),si.StartFlag.data(),si.SnFlag.data(),si.IdFlag.data());
     for(map<string,DeviceInfo>::reverse_iterator iterd = si.Dis.rbegin();iterd!= si.Dis.rend();++iterd)
     {
         DeviceInfo di = iterd->second;
         printf("Device name = %s code=%s  startflag=%s ip =%s port=%s baudRate=%s dataBits=%s stopBits=%s parity=%s\n",\
           di.Name.data(),di.DeviceCode.data(),di.StartFlag.data(),di.Ip.data(),di.Port.data(),di.BaudRate.data(),di.DataBits.data(),di.StopBits.data(),di.Parity.data());
         for(map<string,UnitInfo>::reverse_iterator iteru = di.Units.rbegin();iteru!= di.Units.rend();++iteru)
         {
             UnitInfo ui = iteru->second;
             printf("Unit name=%s enable=%d startadd=%d length=%d\n",\
               ui.Name.data(),ui.Enable,ui.StartAddress,ui.Length);
             for(map<string,Tag>::reverse_iterator itert = ui.Tags.rbegin();itert!= ui.Tags.rend();++itert)
             {
                 Tag tag = itert->second;
                 printf("Tag name=%s desc=%s address=%d tv.name=%s tagcode=%s logicalMode=%s ControlTagCount=%d\n",\
                        tag.Name.data(),tag.Description.data(),tag.Address,tag.TagName.data(),tag.TagCode.data(),tag.LogicalMode.data(),tag.ControlTagCount);
             }
             printf("\n");
         }
     }
}

#endif
/**
 * @brief 解析配置文件
 * @param 配置文件路径
 * @return
 */
LineInfo ConfigLoader::LoadConfig(string linename)
{
    xmlDocPtr doc;  //解析树
    xmlNodePtr cur_li;  //当前节点
    LineInfo li;
    LineInfo null;
    g_deviceNum = 0;
    m_deviceNumcount = 0;
    null.Init();
    QFile file(linename.data());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return null;
    QByteArray byte = file.readAll();
    doc = xmlParseMemory(byte.data(), byte.size());
    if (doc == NULL)
    {
      printf("Document not parsed successfully!\n");
      xmlFreeDoc(doc);
      return null;
    }
    //得到根节点
    cur_li = xmlDocGetRootElement(doc);
    if (cur_li == NULL)
    {
        xmlFreeDoc(doc);
        return null;
    }
    //判断根节点
    if((!xmlStrcmp(cur_li->name,(const xmlChar *)"Line")))
    {
        xmlChar *node;
        if((node = xmlGetProp(cur_li,(const xmlChar *)"name")) != NULL)
            li.Name = (const char *)node;
        else
            return null;
        //得到当前节点的第一个子节点，即第一个station
        xmlNodePtr cur_si;
        cur_si = cur_li->xmlChildrenNode;
        while(cur_si != NULL)
        {
            if((!xmlStrcmp(cur_si->name,(const xmlChar *)"station")))
            {
                StationInfo si;
                if((node = xmlGetProp(cur_si,(const xmlChar *)"name")) != NULL)
                {
                    si.Name = (const char *)node;
                }
                else
                {
                    cout<<"------ 1 ------"<<endl;
                    return null;
                }
                if((node = xmlGetProp(cur_si,(const xmlChar *)"code")) != NULL)
                {
                    si.Code = (const char *)node;
                }
                else
                {
                    cout<<"------ code ------"<<endl;
                    return null;
                }
                if((node = xmlGetProp(cur_si,(const xmlChar *)"endnum")) != NULL)
                {
                    si.endNum = (const char *)node;
                }
                else
                {
                    cout<<"------ endnum ------"<<endl;
                    return null;
                }
                if((node = xmlGetProp(cur_si,(const xmlChar *)"testcodenum")) != NULL)
                {
                    si.testCodeNum = (const char *)node;
                    testCodeNum = atoi(si.testCodeNum.data());
                }
                else
                {
                    cout<<"------ testcodenum ------"<<endl;
                    return null;
                }
                si.Prefix = si.Code + "$";
                si.StartFlag = si.Prefix + "SB";
                si.SnFlag = si.Prefix + "SN";
                si.IdFlag = si.Prefix + "PID";
                si.StateFlag = si.Prefix + "SS";
                si.alarmStatus = si.Prefix + "AS";
                si.JudgeResult = si.Prefix + "JR";
//                si.ProductCount = si.Prefix + "PC";
                xmlNodePtr cur_di;
                cur_di = cur_si->xmlChildrenNode;
                while(cur_di != NULL)
                {
                    if((!xmlStrcmp(cur_di->name,(const xmlChar *)"device")))
                    {
                        DeviceInfo di;
                        if((node = xmlGetProp(cur_di,(const xmlChar *)"name")) != NULL)
                        {
                            di.Name = (const char *)node;
                        }
                        else
                        {
                            cout<<"------ 3 ------"<<endl;
                            return null;
                        }
                        if((node = xmlGetProp(cur_di,(const xmlChar *)"code")) != NULL)
                        {
                            di.DeviceCode = (const char *)node;
                        }
                        else
                        {
                            cout<<"------ 4 ------"<<endl;
                            return null;
                        }
                        if((node = xmlGetProp(cur_di,(const xmlChar *)"teststartpos")) != NULL)
                        {
                            di.testStartPos = (const char *)node;
                        }
                        else
                        {
                            cout<<"------ testStartPos ------"<<endl;
                            return null;
                        }
                        di.Prefix = si.Prefix + di.DeviceCode + "$";
                        di.StartFlag = di.Prefix + "SB";
                        di.SnFlag = di.Prefix + "SN";
                        di.IdFlag = di.Prefix + "PID";
                        di.MiFlag = di.Prefix + "MI";
                        di.StateFlag = si.StateFlag;
                        di.JudgeResult = di.Prefix + "JR";
                        di.EndFlag = di.Prefix + "END";
                        di.MAV = di.Prefix + "MAV";          //气压表
                        di.EigenValue = di.Prefix + "EV";
                        di.iValue = di.Prefix + "IV";
                        di.BarCode = di.Prefix + "BARCODE";
                        di.DeviceState = di.Prefix + "STATE";
                        di.Alarm = di.Prefix + "AM";
                        di.communicateOk = di.Prefix + "COK";       //通信恢复，向PLC点位写值
                        di.communicateERR = di.Prefix + "CERR";     //通信异常，向PLC点位写值
                        di.devParaAlarm = di.Prefix + "DPA";        //加工参数报警
                        di.testParaAlarm = di.Prefix + "TPA";       //测试项参数报警
                        di.beat = di.Prefix + "BT";                 //PLC心跳检测
                        di.devInitAlarm = di.Prefix + "INIT";       //设备初始化报警
                        xmlNodePtr cur_ui = cur_di->xmlChildrenNode;
                        while(cur_ui != NULL)
                        {
                            if((!xmlStrcmp(cur_ui->name,(const xmlChar *)"Unit")))
                            {
                                UnitInfo ui;
                                if((node = xmlGetProp(cur_ui,(const xmlChar *)"name")) != NULL)
                                {
                                    ui.Name = (const char *)node;
                                }
                                else
                                {
                                    cout<<"------ 5 ------"<<endl;
                                    return null;
                                }
                                if((node = xmlGetProp(cur_ui,(const xmlChar *)"enable")) != NULL)
                                {
                                    ui.Enable = (*(int *)node) == 0 ? false : true;
                                    if(!ui.Enable)
                                    {
                                        cur_ui = cur_ui->next;
                                        continue;
                                    }
                                }
                                else
                                {
                                    cout<<"------ 6 ------"<<endl;
                                    return null;
                                }
                                if((node = xmlGetProp(cur_ui,(const xmlChar *)"startAddr")) != NULL)
                                {
                                    ui.StartAddress = atoi((const char *)node);
                                }
                                else
                                {
                                    cout<<"------ 7 ------"<<endl;
                                    return null;
                                }
                                if((node = xmlGetProp(cur_ui,(const xmlChar *)"length")) != NULL)
                                {
                                    ui.Length = atoi((const char *)node);
                                }
                                else
                                {
                                    cout<<"------ 8 ------"<<endl;
                                    return null;
                                }
                                xmlNodePtr cur_tag = cur_ui->xmlChildrenNode;
                                while(cur_tag != NULL)
                                {
                                    if((!xmlStrcmp(cur_tag->name,(const xmlChar *)"tag")))
                                    {
                                        Tag tag;
                                        if((node = xmlGetProp(cur_tag,(const xmlChar *)"name")) != NULL)
                                        {
                                            tag.Name = (const char *)node;
                                        }
                                        else
                                        {
                                            cout<<"------ 9 ------"<<endl;
                                            return null;
                                        }
                                        if((node = xmlGetProp(cur_tag,(const xmlChar *)"desc")) != NULL)
                                        {
                                            tag.Description = (const char *)node;
                                        }
                                        else
                                        {
                                            cout<<"------ 10 ------"<<endl;
                                            return null;
                                        }
                                        if((node = xmlGetProp(cur_tag,(const xmlChar *)"address")) != NULL)
                                        {
                                            tag.Address = atoi((const char *)node);
                                        }
                                        else
                                        {
                                            cout<<"------ 11 ------"<<endl;
                                            return null;
                                        }
                                        if((node = xmlGetProp(cur_tag,(const xmlChar *)"logicalMode")) != NULL)
                                        {
                                            tag.LogicalMode = (const char *)node;
                                        }
                                        else
                                        {
                                            cout<<"------ 12 ------"<<endl;
                                            return null;
                                        }
                                        if((node = xmlGetProp(cur_tag,(const xmlChar *)"tagcode")) != NULL)
                                        {
                                            tag.TagCode = (const char *)node;
                                        }
                                        else
                                        {
                                            cout<<"------ 13 ------"<<endl;
                                            return null;
                                        }
                                        if((node = xmlGetProp(cur_tag,(const xmlChar *)"ctagcount")) != NULL)
                                        {
                                            tag.ControlTagCount = atoi((const char *)node);
                                        }
                                        else
                                        {
                                            cout<<"------ 14 ------"<<endl;
                                            return null;
                                        }
                                        int i;
//                                        cout<<"--- tag.ControlTagCount = "<<tag.ControlTagCount<<endl;
                                        for(i = 1;i <= tag.ControlTagCount;i++)
                                        {
                                            char str[10];
                                            sprintf(str,"ctagname%d",i);
//                                            cout<<"--- str = "<<str<<endl;
                                            if((node = xmlGetProp(cur_tag,(const xmlChar *)str)) != NULL)
                                            {
                                                tag.CTagNames.push_back(si.Code + "$" + (const char *)node);
                                            }
                                            else
                                            {
                                                cout<<"------ 15 ------"<<endl;
                                                return null;
                                            }
                                        }
                                        tag.TagName =  di.Prefix + tag.TagCode;
                                        ui.Tags.insert(pair<string,Tag>(tag.Name,tag));
                                     }
                                     cur_tag = cur_tag->next;
                                }
                                di.Units.insert(pair<string,UnitInfo>(ui.Name,ui));
                            }
                            cur_ui = cur_ui->next;
                        }
                        //修改，为实现显示界面设备排序，key值采用DeviceCode
                        si.Dis.insert(pair<string,DeviceInfo>(di.DeviceCode,di));
                        m_deviceNumcount++;
//                        g_deviceNum++;
                    }
                    cur_di = cur_di->next;
                }
                li.Si = si;
            }
            cur_si = cur_si->next;
        }
    }
    return li;
}


