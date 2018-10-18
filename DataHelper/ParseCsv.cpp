#include "ParseCsv.h"
#include "DataHelper/LogFile.h"

vector<PlcPointInfo> PlcDisplayVec;
vector<PlcPointInfo> PlcSettingVec;
vector<PlcPointInfo> PlcDDisplayVec;
vector<PlcPointInfo> PlcControlVec;
vector<PlcPointInfo> PlcWholeInfoVec;
vector<PlcPointInfo> PlcAlarmVec;
vector<PlcPointInfo> PlcStateVec;

ParseCsv::ParseCsv()
{}

/**
 * @brief 解析PLC点位信息csv文件
 * @param filename
 * @return
 */
vector<PlcPointInfo> ParseCsv::ReadCsvFile(string filename)
{
    vector<PlcPointInfo> infoVec;
    QFile csvFile(filename.data());
    csvFile.open(QFile::ReadOnly);
    if(csvFile.isOpen())
    {
        csvFile.readLine().toStdString();
        while(!csvFile.atEnd())
        {
            PlcPointInfo info;
            QString str = csvFile.readLine();
            if(!Trim(str.toStdString()).empty())
            {
                str.remove(str.length() - 1,1);
                QStringList list = str.split(',');
                info.m_pointName = list.at(0).toStdString();
                info.m_pointType = list.at(1).toInt();
                info.m_registerType = list.at(2).toStdString();
                info.m_registerAddr = list.at(3).toInt();
                if(info.m_registerType == "D" && list.length() >= 6)
                {
                    info.m_pointCoefficient = list.at(4).toDouble();
                    info.m_pointUnit = list.at(5).toStdString();
                }
                infoVec.push_back(info);
            }
        }
        csvFile.close();
    }
    return infoVec;
}

/**
 * @brief 将PLC信息写入csv文件
 * @param info
 * @param filename
 * @return
 */
bool ParseCsv::WriteToCsvFile(vector<PlcPointInfo> info,string filename)
{
    QFile csvFile(filename.data());
    csvFile.open(QFile::WriteOnly | QFile::Truncate);
    string str = "项目描述,项目类型,寄存器类型,寄存器地址\n";
    if(csvFile.isOpen())
    {
        csvFile.write(str.data());
        if(!info.empty())
        {
            for(int i = 0;i < info.size();i++)
            {
                str.clear();
                str.append(info[i].m_pointName).append(",").append(IntToString(info[i].m_pointType)).append(",");
                str.append(info[i].m_registerType).append(",").append(IntToString(info[i].m_registerAddr));
                if(info[i].m_registerType == "D")
                    str.append(",").append(DoubleToString(info[i].m_pointCoefficient,"%.6f")).append(",").append(info[i].m_pointUnit).append("\n");
                else
                    str.append("\n");
                csvFile.write(str.data());
            }
        }
        csvFile.close();
    }
    string command = "chmod 777 " + filename;
    system(command.data());
    return true;
}

/**
 * @brief 将PLC信息进行分类，分别写入状态，机械动作，参数三个不同的数组中
 * @param PlcInfo
 */
bool ParseCsv::GetPlcVec(vector<PlcPointInfo> PlcInfo)
{
    int value = 0;
    PlcWholeInfoVec = PlcInfo;
    PlcPointInfo plc;

    PlcDisplayVec.clear();
    PlcControlVec.clear();
    PlcSettingVec.clear();
    PlcDDisplayVec.clear();
    PlcAlarmVec.clear();
    PlcStateVec.clear();
    if(!PlcInfo.empty())
    {
        for(int i = 0; i < PlcInfo.size(); i++)
        {
            value = PlcInfo.at(i).m_pointType;
            plc = PlcInfo.at(i);
            switch(value)
            {
                case 1:     //显示界面
                {
                    if(plc.m_registerType == "M")
                    {
                        PlcStateVec.push_back(plc);
                    }
                    else if(plc.m_registerType == "D")
                    {
                        PlcDDisplayVec.push_back(plc);
                    }
                }
                break;
                case 2:     //机械动作
                {
                    PlcControlVec.push_back(plc);
                }
                break;
                case 3:     //参数设置
                {
                    PlcSettingVec.push_back(plc);
                }
                break;
                case 4:     //报警点
                {
                    PlcAlarmVec.push_back(plc);
                }
                break;
                default:
                      break;
            }
        }
         _log.LOG_INFO("ParseCsv 解析CSV文件【成功】");
         return true;
    }
    else
    {
        _log.LOG_ERROR("ParseCsv 解析CSV文件【失败】");
        return false;
    }
}
