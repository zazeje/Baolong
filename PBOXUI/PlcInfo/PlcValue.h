#ifndef PLCVALUE_H
#define PLCVALUE_H

#include <string>
#include <vector>

using namespace std;

//包含每个触摸屏界面需要批量读取的寄存器信息
class PlcValue
{

public:

    PlcValue();
    string registerType;    //单个界面对应的寄存器类型
    int itemType;           //界面对应的项目类型
    int startAddr;          //将界面中所有需要读取的寄存器地址排序后得出的起始地址
    int length;             //批量读取界面所有点位所需要的最小长度
    vector<int> values;     //批量读取出来的值

    void Clear();

};

#endif // PLCVALUE_H
