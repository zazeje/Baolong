#include "PlcPointBase.h"
#include "main.h"

PlcBase* PlcPointBase::m_device = NULL;
vector<PlcValue> PlcPointBase::m_pv;
//pthread_rwlock_t PlcPointBase::m_lock = PTHREAD_RWLOCK_INITIALIZER;
int PlcPointBase::m_plcDisplayType = 0;
bool PlcPointBase::m_readFlag = false;

/**
 * @brief PlcPointBase::PlcPointBase
 */
PlcPointBase::PlcPointBase()
{
    if(m_device == NULL)
    {
#ifdef _D2Line
        if(g_dbInfo.GetWorkCenterNo() == "3106")
        {
            //电池组装工位使用的是5UPLC，触摸屏端口为232协议,触摸屏使用串口站号为1
            SerialPortParameters* spp = new SerialPortParameters("COM2",9600,7,1,1);
            m_device = new Fx5U(spp,1,"控制触摸屏Plc",false);
        }
        else
        {
            SerialPortParameters* spp = new SerialPortParameters("COM2",9600,7,1,2);
            m_device = new FxPlc(spp,"控制触摸屏Plc",programProtocol,false);
        }
#else
        SerialPortParameters* spp = new SerialPortParameters("COM4",9600,7,1,1);
        m_device = new FxPlc(spp,"PLC触摸屏",dedicatedProtocol,false);
//        SerialPortParameters* spp = new SerialPortParameters("COM4",9600,7,1,2);
//        m_device = new FxPlc(spp,"PLC触摸屏",programProtocol,false);
#endif

        pthread_t th;
        pthread_create(&th,NULL,CreateThread,this);
    }
    m_cycleTime = 0;
    m_productCount = 0;
    m_communicateState = true;
}

/**
 * @brief PlcPointBase::~PlcPointBase
 */
PlcPointBase::~PlcPointBase()
{
    if(m_device != NULL)
    {
        delete m_device;
        m_device = NULL;
    }
    delete m_titleLabel;
    delete m_titleHLayout;
    delete m_titleGroup;
    delete m_titleVLayout;

    delete m_previousPage;
    delete m_nextPage;
    delete m_pageMessage;
    delete m_currentpageLabel;
    delete m_spiltLabel;
    delete m_totalpageLabel;
    delete m_pageLabel;
    delete m_pageLayout;
    delete m_pageHBoxLayout;
    delete m_tableWidget;
    delete m_returnButton;
    delete m_returnLayout;

    delete m_pageVBoxLayout;
    delete m_pageGroupBox;
    delete m_vBoxLayout;
    delete m_wholeLayout;
    delete m_mainPartBox;

    delete m_timer;
}


/**
 * @brief 翻到下一页槽函数
 */
void PlcPointBase::GoToNextPage()
{
    if(m_currentPageIndex < m_wholePageNum)
    {
        UpdateDisplayItem(++m_currentPageIndex);
    }
}

/**
 * @brief 翻到上一页槽函数
 */
void PlcPointBase::GoToPreviousPage()
{
    if(m_currentPageIndex > 1)
    {
        UpdateDisplayItem(--m_currentPageIndex);
    }
}

/**
 * @brief 跟新当前页与总页数信息
 */
void PlcPointBase::UpdatePageMessage()
{
    m_wholePageNum = m_pointInfoVec.size() / m_currentPageItems;
    if(m_pointInfoVec.size() % m_currentPageItems > 0)
        m_wholePageNum += 1;
    string message = "当前第  " + IntToString(m_currentPageIndex) + "  /  " + IntToString(m_wholePageNum) + "  页";
    m_pageMessage->setText(QString::fromStdString(message));
}

/**
 * @brief 更新显示的条目
 * @param 需要显示的起始条目编号
 */
void PlcPointBase::UpdateDisplayItem(int pNum)
{
    m_currentPageIndex = pNum;
    UpdatePageMessage();
    int i = (pNum - 1) * m_currentPageItems;
    for(int t = 0;t < m_pointInfoVec.size();t++)
        m_tableWidget->setRowHidden(t / (m_currentPageItems / 10),true);
    for(int t = i;t < i + m_currentPageItems;t++)
    {
        if(t >= m_pointInfoVec.size())
            break;
        m_tableWidget->setRowHidden(t / (m_currentPageItems / 10),false);
    }
}

/**
 * @brief 更新PLC的值，显示在界面上
 */
void PlcPointBase::UpdatePlcValue()
{
    vector<int> addrVec;
    for(int i = 0;i < m_pointInfoVec.size();i++)
    {
        addrVec.push_back(m_pointInfoVec[i].m_registerAddr);
    }
}

void PlcPointBase::SetPlcValueVec(vector<PlcValue> pv)
{
    m_pv = pv;
}

void PlcPointBase::GetPlcValueVec(vector<PlcValue>& pv)
{
    pv = m_pv;

}

void PlcPointBase::SetReadPlcFlag(bool flag)
{
    m_readFlag = flag;
}

void PlcPointBase::SetDisplayType(int type)
{
    m_plcDisplayType = type;
}

string PlcPointBase::GetCycleTime()
{
    if(m_cycleTime != -1)
        return IntToString(m_cycleTime);
    else
        return "";
}

string PlcPointBase::GetProductCount()
{
    if(m_productCount != -1)
        return IntToString(m_productCount);
    else
        return "";
}

void* PlcPointBase::CreateThread(void* arg)
{
    PlcPointBase* th = (PlcPointBase*)arg;
    th->ReadPlcDisplayValue();
}
/**
 * @brief 读取PLC寄存器的值
 */
void PlcPointBase::ReadPlcDisplayValue()
{
    int count = 0;
//    cout<<"m_readFlag = "<<m_readFlag<<endl;
    while(1)
    {
        while(m_readFlag)
        {
            // 触摸屏主界面 m_plcDisplayType = 5
            if(m_plcDisplayType == 5)
            {
                vector<int> vec;
                vec = m_device->ReadWord(500,1);
                if(vec.size() > 0)
                    m_productCount = vec[0];
                vec.clear();
                vec = m_device->ReadWord(502,1);
                if(vec.size() > 0)
                    m_cycleTime = vec[0];
            }
            else if(m_plcDisplayType == 1)// 触摸屏显示PLC状态界面 m_plcDisplayType = 1
            {
                if(!m_pv.empty())
                {
                    vector<int> mv;
                    for(int i = 0;i < m_pv.size();i++)
                    {

                        if((m_pv[i].registerType == "M") && ((m_pv[i].itemType == 1) || (m_pv[i].itemType == 4)))
                        {
                            string value = m_device->ReadBits(m_pv[i].startAddr, m_pv[i].length, "M");
//                                cout<<"startAddr = "<<m_pv[i].startAddr<<"  length = " <<m_pv[i].length<<endl;
                            mv = binaryStrToIntVec(value);
                            m_pv[i].values = mv;
                        }
                    }
                }
            }
            else if(m_plcDisplayType == 3)// 触摸屏设置参数界面 m_plcDisplayType = 3
            {
                if(!m_pv.empty())
                {
                    vector<int> dv;
                    for(int i = 0;i < m_pv.size();i++)
                    {
                        if((m_pv[i].registerType == "D") && ((m_pv[i].itemType == 1) || (m_pv[i].itemType == 3)))
                        {
                            dv = m_device->ReadWord(m_pv[i].startAddr, m_pv[i].length);
                            m_pv[i].values = dv;
                        }
                    }
                }
            }
            if(count++ % 4 == 0)
            {
                count = 1;
                m_communicateState = m_device->CanAcess();
                if(!m_communicateState)
                    _log.LOG_DEBUG("【PLC触摸屏】 通信连接【失败】");
            }
            usleep(500 * 1000);
        }
        usleep(100 * 1000);
        if(count++ % 20 == 0)
        {
            count = 1;
            m_communicateState = m_device->CanAcess();
//            _log.LOG_DEBUG("---2  m_communicateState = %d",m_communicateState);
            if(!m_communicateState)
                _log.LOG_DEBUG("【PLC触摸屏】 通信连接【失败】");
        }
    }
}

bool PlcPointBase::GetCommunicateState()
{
    return m_communicateState;
}

/**
 * @brief 自定义button构造函数
 * @param buttonText
 */
MyPushButton::MyPushButton(QString buttonText) : QPushButton(buttonText)
{
    m_buttonNum = 0;
    m_clickedNum = true;
    connect(this,SIGNAL(clicked()),this,SLOT(ChangeColor()));
}

/**
 * @brief 设置button序号
 * @param num
 */
void MyPushButton::SetMyButtonNum(int num)
{
    m_buttonNum = num;
}

/**
 * @brief 获取button序号
 * @return
 */
int MyPushButton::GetMyButtonNum()
{
    return m_buttonNum;
}

/**
 * @brief 触发带有序号的点击信号
 */
void MyPushButton::EmitClickedByNum()
{
    this->setEnabled(false);
    emit MyButtonClicked(m_buttonNum);
    this->setEnabled(true);
}

void MyPushButton::ChangeColor()
{
    if(m_clickedNum)
        setStyleSheet("background-color:rgb(162,181,205);");
    else
        setStyleSheet("background-color:rgb(232,232,232);");
    m_clickedNum = !m_clickedNum;
}

