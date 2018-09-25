#include "DatabaseInfo.h"

DatabaseInfo g_dbInfo;

/**
 * @brief 构造函数，从配置文件中获取数据库信息
 */
DatabaseInfo::DatabaseInfo()
{

}

DatabaseInfo::~DatabaseInfo()
{

}

bool DatabaseInfo::Init()
{
    fstream file;
    file.open("./config.txt");
    if(!file.is_open())
    {
        _log.LOG_ERROR("DatabaseInfo 配置文件【config.txt】打开失败");
        return false;
    }
    else
    {
        getline(file, m_workCenterNo);
        getline(file, m_hostIP);
        getline(file, m_userName);
        getline(file, m_password);
        getline(file, m_databaseName);
        getline(file, m_LineName);
        _log.LOG_DEBUG("DatabaseInfo【config.txt】 workCenterNo=【%s】 hostIP=【%s】 userName=【%s】 password=【%s】 databaseName=【%s】",\
                       m_workCenterNo.data(),m_hostIP.data(),m_userName.data(),m_password.data(),m_databaseName.data());

        file.close();

        if(!m_workCenterNo.empty()&&!m_hostIP.empty()&&!m_userName.empty()&&!m_password.empty()&&!m_databaseName.empty())
        {
            _log.LOG_DEBUG("DatabaseInfo config.txt 解析【成功】");
            return true;
        }
        else
        {
            _log.LOG_ERROR("DatabaseInfo config.txt 解析【失敗】\n");
            return false;
        }
    }
}

string DatabaseInfo::GetWorkCenterNo()
{
    return m_workCenterNo;
}

bool DatabaseInfo::SetWorkCenterNo(string WorkCenterNo)
{
    fstream file;
    file.open("./config.txt",fstream::trunc|fstream::out);
    if(!file.is_open())
    {
        _log.LOG_ERROR("DatabaseInfo 配置文件【config.txt】打开失败");
        return false;
    }
    else
    {
        file<<WorkCenterNo<<endl;
        file<<m_hostIP<<endl;
        file<<m_userName<<endl;
        file<<m_password<<endl;
        file<<m_databaseName<<endl;
        _log.LOG_DEBUG("DatabaseInfo 写入配置文件【config.txt】 workCenterNo=【%s】 hostIP=【%s】 userName=【%s】 password=【%s】 databaseName=【%s】",\
                       WorkCenterNo.data(),m_hostIP.data(),m_userName.data(),m_password.data(),m_databaseName.data());

        file.close();
        m_workCenterNo = WorkCenterNo;
        return true;
    }
}

string DatabaseInfo::GetHostIP()
{
    return m_hostIP;
}

string DatabaseInfo::GetDBUserName()
{
    return m_userName;
}

string DatabaseInfo::GetPassword()
{
    return m_password;
}

string DatabaseInfo::GetDatabaseName()
{
    return m_databaseName;
}
