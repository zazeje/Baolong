#include "MySQLHelper.h"

MySQLHelper::MySQLHelper()
{}

/**
 * @brief 构造函数
 * @param mysql服务器IP地址
 * @param 登录用户名
 * @param 登录密码
 * @param 数据库名称
 */
MySQLHelper::MySQLHelper(string datasource, string username, string passwd, string database)
{
    DataSource = datasource;
    UserName = username;
    PassWord = passwd;
    DataBase = database;
}

/**
 * @brief Mysql初始化连接
 * @return
 */
bool MySQLHelper::MySQL_Init(MYSQL* m_connection)
{
    int conecttimeout = 1;
    int readtimeout = 1;
    int writetimeout = 1;
    if(mysql_init(m_connection) == NULL)
        return false;

    //设置连接超时时间为5S
    mysql_options(m_connection, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&conecttimeout);
    mysql_options(m_connection, MYSQL_OPT_READ_TIMEOUT, (const char *)&readtimeout);
    mysql_options(m_connection, MYSQL_OPT_WRITE_TIMEOUT, (const char *)&writetimeout);

    //mysql连接
    if(mysql_real_connect(m_connection,(char *)DataSource.data(),(char *)UserName.data(),(char *)PassWord.data(),(char *)DataBase.data(),0,NULL,CLIENT_MULTI_STATEMENTS)==NULL)
    {
        _log.LOG_ERROR("MySQLHelper Connection Error 【%s】",mysql_error(m_connection));
        return false;
    }
//    _log.LOG_DEBUG("MySQLHelper MySQL_Init 【Success】");
    //设置mysql为utf8编码格式
    if (mysql_set_character_set(m_connection, "utf8" ))
    {
        _log.LOG_ERROR("MySQLHelper UTF8 Error 【%s】",mysql_error(m_connection));
        return false;
    }
    return true;
}

/**
 * @brief 数据库查询
 * @param 查询命令
 * @return
 */
MYSQL_RES* MySQLHelper::LoadTable(MYSQL* m_connection,string command)
{
    //查询
    MYSQL_RES *result;
    if(mysql_query(m_connection,(char *)command.data()))
    {        
        _log.LOG_ERROR("MySQLHelper LoadTable Error 【%s】",mysql_error(m_connection));
        return NULL;
    }
//    _log.LOG_DEBUG("MySQLHelper LoadTable 【Success】---------1");
    //保存查询到的数据到result
    result = mysql_store_result(m_connection);
    return result;
}

/**
 * @brief 数据库插入操作
 * @param 插入命令
 * @return 插入成功返回真
 */
bool MySQLHelper::InsertTable(MYSQL* m_connection,string command)
{    
    if(mysql_query(m_connection,(char *)command.data()))  //查询
    {
        _log.LOG_ERROR("MySQLHelper InsertTable Error  【%s】",mysql_error(m_connection));
        return false;
    }
    return true;
}

/**
 * @brief 获取数据库操作结果
 * @return
 */
MYSQL_RES* MySQLHelper::MySQL_GetContent(MYSQL* m_connection)
{
    //保存查询到的数据到result
    m_result=mysql_store_result(m_connection);
    return m_result;
}

/**
 * @brief 释放结果资源
 * @param
 */
void MySQLHelper::MySQL_Release(MYSQL_RES* result)
{
    mysql_free_result(result);
}

/**
 * @brief 关闭连接
 */
void MySQLHelper::MySQL_Close(MYSQL* m_connection)
{
    mysql_close(m_connection);
}



