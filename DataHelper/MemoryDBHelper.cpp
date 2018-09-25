#include "MemoryDBHelper.h"

MemoryDB m_db;

/**
 * @brief 构造函数
 */
MemoryDB::MemoryDB()
{
    pthread_rwlock_init(&m_lock,NULL);
}

/**
 * @brief 析构函数
 */
MemoryDB::~MemoryDB()
{}

/**
 * @brief 根据键名读内存数据库点
 * @param 键名
 * @return 找不到对应的键值对则返回空字符串
 */
string MemoryDB::Read_TagMValue(string key)
{
    pthread_rwlock_rdlock(&m_lock);
    map<string,TagM>::iterator it = M.find(key);
    if(it != M.end())
    {
        string res = it->second.value;
        pthread_rwlock_unlock(&m_lock);
        return res;
    }
    else
    {
        pthread_rwlock_unlock(&m_lock);
        return "";
    }
}

/**
 * @brief 向内存数据库插入键值对
 * @param 键名
 * @param 键值
 * @return
 */
bool MemoryDB::Write_TagMValue(string key, string value)
{
    pthread_rwlock_wrlock(&m_lock);
    map<string,TagM>::iterator it = M.find(key);
    if(it != M.end())
    {
        it->second.value = value;
        pthread_rwlock_unlock(&m_lock);
        return true;
    }
    else
    {
        TagM m_tag;
        m_tag.Name = key;
        m_tag.value = value;
        M.insert(pair<string,TagM>(m_tag.Name,m_tag));
        pthread_rwlock_unlock(&m_lock);
        return true;
    }
}
