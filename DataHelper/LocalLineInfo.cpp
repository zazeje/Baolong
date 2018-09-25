#include "LocalLineInfo.h"
LocalLineInfo g_locallineInfo;

LocalLineInfo::LocalLineInfo()
{
    m_localPartNo = "96216708";
    m_localDispatchNo = "361712210192";
    m_localOperationNo = "5";
    m_localUser = "test";
    m_localProjectNo = "QY1104";
    m_localProductNumber = "1";
    m_localPartNofirst = "96216708";
    m_localDispatchNofirst = "361712210192";
    m_localProjectNofirst = "QY1104";
}

bool LocalLineInfo::LoadLineInfo()
{
    bool ret = false;
    fstream file;
    file.open("./LocalLineInfo.dat",fstream::in);
    if(!file.is_open())
    {
        _log.LOG_ERROR("LocalLineInfo 配置文件【LocalLineInfo.dat】打开失败");
        file.open("./LocalLineInfo.dat",fstream::trunc | fstream::out);
        file.close();
    }
    else
    {
        getline(file, m_localPartNo);
        getline(file, m_localDispatchNo);
        getline(file, m_localOperationNo);
        getline(file, m_localUser);
        getline(file, m_localProjectNo);
        getline(file, m_localProductNumber);
        _log.LOG_DEBUG("LocalLineInfo【LineInfo.dat】 localPartNo=【%s】 localDispatchNo=【%s】 localOperationNo=【%s】 localUser=【%s】 localProjectNo=【%s】 localProductNumber=【%s】",\
                       m_localPartNo.data(),m_localDispatchNo.data(),m_localOperationNo.data(),m_localUser.data(),m_localProjectNo.data(),m_localProductNumber.data());

        file.close();

        if(!m_localPartNo.empty()&&!m_localDispatchNo.empty()&&!m_localOperationNo.empty()&&!m_localUser.empty()&&!m_localProjectNo.empty()&&!m_localProductNumber.empty())
        {
            m_localPartNofirst = m_localPartNo;
            m_localDispatchNofirst = m_localDispatchNo;
            m_localProjectNofirst = m_localProjectNo;
            _log.LOG_DEBUG("LocalLineInfo LocalLineInfo.dat 解析【成功】");
            ret = true;
        }
        else
        {
            _log.LOG_ERROR("LocalLineInfo LocalLineInfo.dat 解析【失敗】\n");
        }
    }
    if(!ret)
    {
        m_localPartNo = "96216708";
        m_localDispatchNo = "361712210192";
        m_localOperationNo = "5";
        m_localUser = "test";
        m_localProjectNo = "QY1104";
        m_localProductNumber = "1";
        m_localPartNofirst = "96216708";
        m_localDispatchNofirst = "361712210192";
    }
    return ret;
}

bool LocalLineInfo::SaveLineInfo()
{
    fstream file;
    file.open("./LocalLineInfo.dat",fstream::trunc|fstream::out);
    if(!file.is_open())
    {
        _log.LOG_ERROR("LocalLineInfo 配置文件【LocalLineInfo.dat】打开失败");
        return false;
    }
    else
    {
        file<<m_localPartNo<<endl;
        file<<m_localDispatchNo<<endl;
        file<<m_localOperationNo<<endl;
        file<<m_localUser<<endl;
        file<<m_localProjectNo<<endl;
        file<<m_localProductNumber<<endl;
        _log.LOG_DEBUG("LocalLineInfo【LocalLineInfo.dat】 localPartNo=【%s】 localDispatchNo=【%s】 localOperationNo=【%s】 localUser=【%s】 localProjectNo=【%s】 localProductNumber=【%s】",\
                       m_localPartNo.data(),m_localDispatchNo.data(),m_localOperationNo.data(),m_localUser.data(),m_localProjectNo.data(),m_localProductNumber.data());

        file.close();
        return true;
    }
}
