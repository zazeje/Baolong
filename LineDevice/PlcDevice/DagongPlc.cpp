#include "DagongPlc.h"

/**
 * @brief 通过端口号、ip地址以及设备名称构造
 * @param port  端口号
 * @param ip    ip地址
 * @param name  设备名称
 */
DagongPlc::DagongPlc(int port,string ip,string name) : ModbusDevice(port,ip,name)
{}

/**
 * @brief 读PLC单点值
 * @param address   PLC单点地址
 * @return
 */
string DagongPlc::Read(int address)
{
    unsigned char* results = ReadCoils(address-1, 1);
    if(results != NULL)
        return results[0] ? "1" : "0";
    return "3";
}

/**
 * @brief 读PLC多个点的值
 * @param address   起始点地址
 * @param length    读取长度
 * @return
 */
string DagongPlc::Read(int address, int length)
{
    char result[100]={0};
    string m_result;
    memset(result,0,sizeof(result));
    for(int i = 0;i < length;i++)
    {
        result[i] = '3';
    }
    unsigned char* values = ReadCoils(address-1,length);//此类库应该减1
    if (values != NULL)
    {
        for (int i = 0; i < length; i++)
        {
            result[i] = values[i] ? '1' : '0';
        }
    }
    m_result = result;
    return m_result;
}

/**
 * @brief 设置PLC单点状态
 * @param address   PLC单点地址
 * @param state     状态值
 * @return
 */
bool DagongPlc::Write(int address, string state)
{
    address--;//此类库应该减1
    return WriteCoil(address,state.compare("1")?false:true);
}

/**
 * @brief 设置PLC多点状态
 * @param address
 * @param length
 * @param state
 * @return
 */
bool DagongPlc::Write(int address, int length, string state)
{}

/**
 * @brief 通信检测
 * @return
 */
bool DagongPlc::CanAcess()
{

}
