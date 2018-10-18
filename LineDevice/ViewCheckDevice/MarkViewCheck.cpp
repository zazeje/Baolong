#include "MarkViewCheck.h"

/**
 * @brief MarkViewCheck::MarkViewCheck              初始化构造函数
 * @param port                                      端口号
 * @param ip                                        IP地址
 */
MarkViewCheck::MarkViewCheck(int port, string ip) : TcpDevice(port, ip)
{
    _getValue = "M";
    _getState = "ITS";
}


/**
 * @brief MarkViewCheck::MarkViewCheck               初始化构造函数
 * @param port                                       端口号
 * @param ip                                         IP地址
 * @param name                                       设备名
 */
MarkViewCheck::MarkViewCheck(int port, string ip, string name) : TcpDevice(port, ip, name)
{
    _getValue = "M";
    _getState = "ITS";
}

/**
 * @brief MarkViewCheck::CanAcess
 * @return
 */
bool MarkViewCheck::CanAcess()
{
    string result = WriteAndRead(_getState + endStr, "Ascii",200);
    if(!result.empty() && result.length() > 0)
        m_communicateStatus = true;
    else
        m_communicateStatus = false;
    return m_communicateStatus;
}


/**
 * @brief MarkViewCheck::GetCheckResult
 * @param id
 * @return
 */
vector<string> MarkViewCheck::GetCheckResult()
{    
    vector<string> result;
    vector<string> res;
    string partNoId;
    string ocrInfo;
    string value;

    //value = OK\rER\r3C14206E\rQY1183-180111-3C14206E
    /********************************************************
     *   视觉的返回值，有4种情况（1次OK、2次OK、3次OK、ER）
     *
     *   1次OK的情况                 2次OK的情况
     *   OK                         OK
     *   3C14206E                   ER(结尾有控制字符)
     *   QY1183-180111-3C14206E     3C14206E(结尾有控制字符)
     *                              QY1183-180111-3C14206E
     *
     * ******************************************************/

    //每行数据用换行符间隔
    value = WriteAndRead(_getValue + endStr, "Ascii", 1500);
    //把所有的换行符去掉存放在result数组里
    splittostring(value,"\r",result);
    _log.LOG_DEBUG("MarkViewCheck GetCheckResult size = %d",result.size());
    for(int i=0;i<result.size();i++)
    {
        cout<<"MarkViewCheck result = "<<result.at(i)<<endl;
        _log.LOG_DEBUG("MarkViewCheck  result = 【%s】",result.at(i).data());
    }

    if(result.size() > 0)
    {
        int length = result.size();
        //将返回数据中的OK/NG放入结果数组中的第一个元素
        res.push_back(result.at(0));

        if(result.size() > 1)
        {
            //倒数第二组数据为ID号
            partNoId = result.at(length - 2);
//            _log.LOG_DEBUG("1--- partNoId = %s length = %d ",partNoId.data(),partNoId.length());
            //去掉EXT特殊字符
            partNoId = partNoId.substr(0, partNoId.length() - 1);
//            _log.LOG_DEBUG("2--- partNoId = %s",partNoId.data());
            res.push_back(partNoId);

            //最后一组数据为OCR数据，其中以"-"作为分割
            ocrInfo = result.at(length - 1);
//            _log.LOG_DEBUG("3 --- ocrInfo = %s",ocrInfo.data());
            vector<string> ocrVec;
            if(!ocrInfo.empty())
                splittostring(ocrInfo,'-',ocrVec);
            if(!ocrVec.empty())
            {
                for(int i = 0;i < ocrVec.size();i++)
                    res.push_back(ocrVec[i]);
            }
            else
            {
                //ER都有控制字符“ETX”
                res.push_back(ocrInfo.substr(0,ocrInfo.length()-1));
            }
            return res;
        }
        else
        {
            return res;
        }
    }
    return res;




#if 0

    string qrCode = "";
    //至少包含2位结果，8位二维码，以及3位分割符
    if(!data.empty())
    {
        //将返回数据中的OK/NG放入结果数组中的第一个元素
        result.push_back(data.substr(0,2));
        //将返回数据的前三个字符删除(包含一个'\r')
        data.erase(0,3);
        //将返回数据中的二维码数据提取出来，二维码数据以两个控制字符结尾
        if(data.empty())
            return result;
        int index = 0;
        for(int i = 0;i < data.length();i++)
        {
            if(data[i] < 32 || i == data.length() - 1)
            {
                index = i;
                qrCode = data.substr(0,i);
                break;
            }
        }

        cout << "qrcode = " << qrCode << endl;
        result.push_back(qrCode);      

        //将二维码数据从返回数据中删除
        if(data.length() >= index + 2)
            data.erase(0,index + 2);
        else
            return result;

        //剩下的则为OCR数据，其中以"-"作为分割
        vector<string> ocrVec;
        if(!data.empty())
            splittostring(data,'-',ocrVec);
        if(!ocrVec.empty())
        {
            for(int i = 0;i < ocrVec.size();i++)
                result.push_back(ocrVec[i]);
        }
        //        for(int i = 0;i < result.size();i++)
        //        {
        //            printf("result[%d] = %s\n",i,result[i].data());
        //        }

//        cout << "result[2] = " << result[2] << " result[3] = " << result[3] << " result[4] = " << result[4] << endl;
    }
#endif
}
