#include "display.h"
#include "DataHelper/LocalLineInfo.h"
#include "main.h"

//extern LineInfo gLine;
#define StartInterval 120         //启动生产时等待设备驱动启动完成的时间间隔周期数，每100ms一个周期，120个周期，为12秒，点击启动生产后12秒界面的停止生产按钮使能。
#define StopInterval 60         //停止生产时等待设备驱动停止完成的时间间隔周期数，每100ms一个周期，60个周期，为6秒，点击停止生产后6秒界面的启动生产按钮使能。
string display::m_billnum = "";
string display::m_productmodel = "";
bool display::m_updateFlag = false;
int Flag = 0;

/**
 * @brief 显示界面构造函数
 * @param parent    父窗口
 */
display::display(mainWidget *parent):m_parent(parent)
{
    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::white);
    QFont font;
    font.setPointSize(15);
    QFont ft;
    ft.setPointSize(20);

    m_timeLabel = new QLabel();
    m_timeLabel->setPalette(pa);
    m_timeLabel->setFont(font);

    string version = "保隆科技";
    version += g_dbInfo.m_LineName;
    //version += LineName;
    version += mainVersion;
    m_factoryLabel = new QLabel(tr(version.data()));
    m_factoryLabel->setPalette(pa);
    m_factoryLabel->setFont(font);

    m_craftNameLabel = new QLabel();
    m_craftNameLabel->setPalette(pa);
    m_craftNameLabel->setFont(ft);
    m_localTest = new QPushButton(tr("启动生产"));
    m_localTest->setFixedSize(width/6,height/15);

    m_craftNameLayout = new QGridLayout();
    m_craftNameLayout->setColumnStretch(0, 1);
    m_craftNameLayout->setColumnStretch(1, 1);
    m_craftNameLayout->setColumnStretch(2, 1);
    m_craftNameGroupbox = new QGroupBox();
    m_craftNameLayout->addWidget(m_timeLabel,0,0,1,1,Qt::AlignLeft | Qt::AlignVCenter);
    m_craftNameLayout->addWidget(m_craftNameLabel,0,1,1,1,Qt::AlignCenter);
    m_craftNameLayout->addWidget(m_factoryLabel,0,2,1,1,Qt::AlignRight | Qt::AlignVCenter);
    m_craftNameGroupbox->setLayout(m_craftNameLayout);

    m_mesLabelLayout = new QHBoxLayout();
    m_secPartVLayout = new QVBoxLayout();
    m_secPartBox = new QGroupBox();
    m_productMessage = new QLabel(tr("生产信息"));
    m_productMessage->setPalette(pa);
    m_productMessage->setFont(font);
    m_mesLabelLayout->addWidget(m_productMessage);
    m_mesLabelLayout->addStretch();

    QPalette pa_PB;
    pa_PB.setColor(QPalette::Base,Qt::white);
    pa_PB.setColor(QPalette::Text,Qt::black);

    m_workModela = new QLabel(tr("工作模式:"));
    m_workModela->setFont(font);
    m_workModela->setPalette(pa);
    m_currworkMode = new QLabel(tr("网络模式"));
    m_currworkMode->setFont(font);
    m_currworkMode->setPalette(pa);
    m_currworkMode->setFixedWidth(width/8);

    m_countla = new QLabel(tr("产品计数:"));
    m_countla->setFont(font);
    m_countla->setPalette(pa);
    m_count = new QLabel();
    m_count->setFont(font);
    m_count->setPalette(pa);
    m_count->setFixedWidth(width/8);

    m_resultla = new QLabel(tr("判定结果:"));
    m_resultla->setFont(font);
    m_resultla->setPalette(pa);
    m_result = new QLabel();
    m_result->setFont(font);
    m_result->setPalette(pa);
    m_result->setMaximumWidth(width/8);

    m_projectNola = new QLabel(tr("产品型号:"));
    m_projectNola->setFont(font);
    m_projectNola->setPalette(pa);
    m_projectNoLineEdit = new QLineEdit();
    m_projectNoLineEdit->setFont(font);
    m_projectNoLineEdit->setPalette(pa);
    m_projectNoLineEdit->setFixedWidth(width/8);

    m_partNo = new QLabel(tr("件       号:"));           //产品件号
    m_partNo->setFont(font);
    m_partNo->setPalette(pa);
    m_partNoEdit = new QLineEdit();
    m_partNoEdit->setFont(font);
    m_partNoEdit->setPalette(pa);
    m_partNoEdit->setFixedWidth(width/8);
    currEditPartNo = "";
    connect(m_partNoEdit,SIGNAL(textEdited(QString)),this,SLOT(PartNoChanged(QString)));

    m_dispatchNoLabel = new QLabel(tr("派工单号:"));           //派工单号
    m_dispatchNoLabel->setFont(font);
    m_dispatchNoLabel->setPalette(pa);
    m_dispatchNoComboBox = new QMyComboBox();
    m_dispatchNoComboBox->setStyleSheet("QComboBox QAbstractItemView::item{height:40px;}");
    m_dispatchNoComboBox->setView(new QListView());
    m_dispatchNoComboBox->setFixedWidth(width/8);
//    m_dispatchNoComboBox->setEditable(false);
//    m_dispatchNoComboBox->lineEdit()->setFocusPolicy(Qt::NoFocus);
    connect(m_dispatchNoComboBox, SIGNAL(currentIndexChanged(QString)),this, SLOT(updateDispatchInfo(QString)));
//    connect(m_dispatchNoComboBox, SIGNAL(getCurrPartNo()),this, SLOT(getPartNoString()));
    m_dispatchNoLineEdit = new QLineEdit();
    m_dispatchNoLineEdit->setFont(font);
    m_dispatchNoLineEdit->setPalette(pa);
    m_dispatchNoLineEdit->setFixedWidth(width/8);

    m_productGridLayout = new QGridLayout();
    m_productGridLayout->setColumnStretch(0, 1);
    m_productGridLayout->setColumnStretch(1, 1);
    m_productGridLayout->setColumnStretch(2, 1);
    m_productGridLayout->setColumnStretch(3, 1);
    m_productGridLayout->setColumnStretch(4, 1);
    m_productGridLayout->setColumnStretch(5, 1);
    m_productGridLayout->setColumnStretch(6, 1);
    m_productGridLayout->setColumnStretch(7, 1);
    m_productGridLayout->setColumnStretch(8, 1);
    m_productGridLayout->setColumnStretch(9, 1);
    m_productGridLayout->setColumnStretch(10, 1);

    m_productGridLayout->addWidget(m_projectNola,0,1,1,1,Qt::AlignRight | Qt::AlignVCenter);
    m_productGridLayout->addWidget(m_projectNoLineEdit,0,2,1,2,Qt::AlignLeft | Qt::AlignVCenter);
    m_productGridLayout->addWidget(m_partNo,0,4,1,1,Qt::AlignRight | Qt::AlignVCenter);
    m_productGridLayout->addWidget(m_partNoEdit,0,5,1,2,Qt::AlignLeft | Qt::AlignVCenter);
    m_productGridLayout->addWidget(m_dispatchNoLabel,0,7,1,1,Qt::AlignRight | Qt::AlignVCenter);
    m_productGridLayout->addWidget(m_dispatchNoLineEdit,0,8,1,2,Qt::AlignLeft | Qt::AlignVCenter);
    m_productGridLayout->addWidget(m_workModela,1,1,1,1,Qt::AlignRight | Qt::AlignVCenter);
    m_productGridLayout->addWidget(m_currworkMode,1,2,1,1,Qt::AlignLeft | Qt::AlignVCenter);
    m_productGridLayout->addWidget(m_countla,1,4,1,1,Qt::AlignRight | Qt::AlignVCenter);
    m_productGridLayout->addWidget(m_count,1,5,1,1,Qt::AlignLeft | Qt::AlignVCenter);
    m_productGridLayout->addWidget(m_resultla,1,7,1,1,Qt::AlignRight | Qt::AlignVCenter);
    m_productGridLayout->addWidget(m_result,1,8,1,1,Qt::AlignLeft | Qt::AlignVCenter);

    m_secPartVLayout->addLayout(m_mesLabelLayout);
    m_secPartVLayout->addLayout(m_productGridLayout);
    m_secPartBox->setLayout(m_secPartVLayout);

    m_stationMessage = new QLabel(tr("工位信息"));
    m_stationMessage->setPalette(pa);
    m_stationMessage->setFont(font);

    m_table = new QTableWidget();
    m_table->setColumnCount(3);
    m_table->setFont(font);
    m_table->setShowGrid(true);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);   //设置表格不可编辑
    m_table->resizeRowsToContents();
    m_table->resizeColumnsToContents();
    m_table->setSelectionMode(QAbstractItemView::NoSelection);
    m_table->verticalHeader()->setDefaultSectionSize(height / 15);
    m_table->horizontalHeader()->setHidden(true);
    m_table->verticalHeader()->setHidden(true);
    m_table->setColumnWidth(0,width/4);
    m_table->setColumnWidth(1,width/2);
    m_table->setColumnWidth(2,width/4);
    m_table->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);

    m_tableLabelLayout = new QHBoxLayout();
    m_tableLabelLayout->addWidget(m_stationMessage);
    m_tableWidgetLayout = new QHBoxLayout();
    m_tableWidgetLayout->addWidget(m_table);
    m_thirdPartVLayout = new QVBoxLayout();
    m_thirdPartVLayout->addLayout(m_tableLabelLayout);
    m_thirdPartVLayout->addLayout(m_tableWidgetLayout);
    m_thirdPartBox = new QGroupBox();
    m_thirdPartBox->setLayout(m_thirdPartVLayout);

    m_return = new QPushButton(tr("返回"));
    m_return->setFixedSize(width/6,height/15);
    connect(m_return,SIGNAL(clicked()),this,SLOT(returnback()));

    m_restart = new QPushButton(tr("重启"));
    m_restart->setFixedSize(width/6,height/15);
    connect(m_restart,SIGNAL(clicked()),this,SLOT(reStart()));
    m_localTest->setEnabled(false);
    m_return->setFocus();
    m_partNoEdit->setFocusPolicy(Qt::NoFocus);
    m_dispatchNoLineEdit->setFocusPolicy(Qt::NoFocus);
    m_projectNoLineEdit->setFocusPolicy(Qt::NoFocus);
    m_lastWorkModeNotify = 0;
    m_lastdriverStartFlag = false;
    connect(m_localTest,SIGNAL(clicked()),this,SLOT(localDriverStart()));
    m_BottomGroup = new QGroupBox();
    m_BottomHLayout = new QHBoxLayout();
    m_BottomHLayout->addStretch();
    m_BottomHLayout->addWidget(m_restart);
    m_BottomHLayout->addStretch();
    m_BottomHLayout->addWidget(m_localTest);
    m_BottomHLayout->addStretch();
    m_BottomHLayout->addWidget(m_return);
    m_BottomHLayout->addStretch();
    m_BottomGroup->setLayout(m_BottomHLayout);

    m_wholeVLayout = new QVBoxLayout();
    m_wholeVLayout->addWidget(m_craftNameGroupbox);
    m_wholeVLayout->addWidget(m_secPartBox);
    m_wholeVLayout->addWidget(m_thirdPartBox);
    m_wholeVLayout->addWidget(m_BottomGroup);
    this->setLayout(m_wholeVLayout);

    statustimer = new QTimer(this);
    connect(statustimer,SIGNAL(timeout()),this,SLOT(updatetestdata()));
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(updatedata()));
    statustimer->start(100);
    m_Flag = true;

    m_clickedNum = 0;
    m_startnum = 0;
    m_preWorkModeNotify = 6;
    m_preEnableSampleMode = 6;
    StationInfo si;
    map<string,DeviceInfo> devInfo;
}

/**
 * @brief 析构函数
 */
display::~display()
{
    delete m_timeLabel;
    delete m_factoryLabel;
    delete timer;
    delete statustimer;
    delete m_productMessage;
    delete m_stationMessage;
    delete m_craftNameLabel;
    delete m_countla;
    delete m_count;
    delete m_return;
    delete m_table;
    delete m_craftNameLayout;
    delete m_craftNameGroupbox;
    delete m_productGridLayout;
    delete m_mesLabelLayout;
    delete m_secPartBox;
    delete m_tableLabelLayout;
    delete m_tableWidgetLayout;
    delete m_thirdPartBox;
    delete m_BottomGroup;
    delete m_BottomHLayout;
    delete m_wholeVLayout;
    delete m_dispatchNoLineEdit;
    delete m_dispatchNoComboBox;
    delete m_dispatchNoLabel;
    delete m_projectNola;
    delete m_projectNoLineEdit;
    delete m_workModela;
    delete m_currworkMode;
    delete m_restart;
    delete m_localTest;
    delete m_resultla;
    delete m_result;
}

/**
 * @brief 更新数据
 */
void display::updatetestdata()
{
    if(TcpWorkThread::m_ShowDeviceListFlag)
    {//设备列表参数已经解析好了，可使用设备清单加载到设备列表中了
        if(m_Flag)
        {//设备列表框从来没有被加载过。可以加载
            //设置表格的第一行为标题显示
            item1st = new QTableWidgetItem();
            item1st->setTextAlignment(Qt::AlignCenter);
            item2ec = new QTableWidgetItem();
            item2ec->setTextAlignment(Qt::AlignCenter);
            item3rd = new QTableWidgetItem();
            item3rd->setTextAlignment(Qt::AlignCenter);

            //动态创建表格行数，deviceNum解析配置文件后得到
            //留出一行显示标题栏
            m_table->setRowCount(g_deviceNum + 1);

            item1st->setText(QString::fromStdString("设备名称"));
            item2ec->setText(QString::fromStdString("采集值"));
            item3rd->setText(QString::fromStdString("通信状态"));
            m_table->setItem(0,0, item1st);
            m_table->setItem(0,1, item2ec);
            m_table->setItem(0,2, item3rd);

            //根据设备数目动态创建红绿点图标
            if(g_deviceNum > 0)
            {
                for(int i = 0;i < g_deviceNum;i++)
                {
                    QLabel *label = new QLabel();
                    label->setAlignment(Qt::AlignCenter);
                    label->setPixmap(QPixmap("./Image/redDot.jpg").scaled(55, 55, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
                    m_table->setCellWidget(i + 1,2,label);
                    m_comLabelVec.push_back(label);
                }
            }
            _log.LOG_DEBUG("display updatetestdata 设备列表设备数量【%s】",IntToString(g_deviceNum).data());
            si = gLine.Si;
            devInfo = si.Dis;
            int num = 0;
            //遍历配置文件，显示相应的值到显示界面
            for(map<string,DeviceInfo>::iterator it = devInfo.begin();it != devInfo.end();it++,num++)
            {
                DeviceInfo di = it->second;
                //设置QTableWidgetItem显示，居中显示deviceName
                QTableWidgetItem *item0 = new QTableWidgetItem();
                item0->setTextAlignment(Qt::AlignCenter);
                item0->setText(QString::fromStdString(di.Name));
                m_tableNameItem.push_back(item0);

                //设置QTableWidgetItem显示，居中显示采集值
                QTableWidgetItem *item1 = new QTableWidgetItem();
                item1->setTextAlignment(Qt::AlignCenter);
                m_tableValueItem.push_back(item1);

                //显示deviceName列的内容  --- 第1列
                m_table->setItem(num + 1,0, item0);
                //显示采集值列的内容      --- 第2列
                m_table->setItem(num + 1,1, item1);
            }
//            _log.LOG_DEBUG("设备清单设备数量【%s】",IntToString(num).data());
            if(EnableSampleMode == 1)
            {
                m_craftNameLabel->setText(QString::fromStdString(si.Name + "【样件测试】"));
            }
            else
            {
                m_craftNameLabel->setText(QString::fromStdString(si.Name));
            }
            m_partNoEdit->setText(QString::fromStdString(partNo));
            m_dispatchNoLineEdit->setText(QString::fromStdString(dispatchNo));
            m_projectNoLineEdit->setText(QString::fromStdString(projectNo));
//            _log.LOG_DEBUG("setText【%s】【%s】【%s】",partNo.data(),dispatchNo.data(),projectNo.data());
            m_Flag = false;
        }
    }
    int WorkModeNotifytmp = WorkModeNotify;
    if(m_lastWorkModeNotify != WorkModeNotifytmp)
    {//工作模式通知码改变了，需要更新按键状态和按键文字
        m_clickedNum = 1;
        m_return->setEnabled(false);
        m_localTest->setEnabled(false);
        if(WorkModeNotifytmp == 2)
        {
            m_startnum = m_clickedNum + StartInterval;               //用于设置点击启动生产后12秒界面的停止生产按钮使能。
            m_localTest->setText(tr("停止生产"));
            _log.LOG_INFO("removeWidget 【removeWidget】 command2");
            m_productGridLayout->removeWidget(m_dispatchNoComboBox);
            m_dispatchNoComboBox->setParent(NULL);
            m_productGridLayout->addWidget(m_dispatchNoLineEdit,0,8,1,2,Qt::AlignLeft | Qt::AlignVCenter);
        }
        else
        {
            m_startnum = m_clickedNum + StopInterval;              //用于设置点击停止生产后6秒界面的启动生产按钮使能。
            m_localTest->setText(tr("启动生产"));
            if(WorkModeNotifytmp == 0)
            {
                m_currworkMode->setText(tr("网络模式"));
                _log.LOG_INFO("removeWidget 【removeWidget】 command0");
                m_productGridLayout->removeWidget(m_dispatchNoComboBox);
                m_dispatchNoComboBox->setParent(NULL);
                m_productGridLayout->addWidget(m_dispatchNoLineEdit,0,8,1,2,Qt::AlignLeft | Qt::AlignVCenter);
            }
            else
            {
                if(EnableSampleMode == 0)
                {
                    m_currworkMode->setText(tr("本地模式"));
                }
                else
                {
                    m_currworkMode->setText(tr("样件模式"));
                }
            }
        }
        m_lastWorkModeNotify = WorkModeNotifytmp;
    }
    else
    {//工作模式通知码没有改变，但是启动生产或停止生产按钮需要分别在6秒或12秒启动或停止完成后更新按键状态和按键文字
        if(WorkModeNotifytmp == 0)
        {
            if(m_startnum == m_clickedNum)
            {//切换到网络模式时在6秒更新按键状态
                if(!m_return->isEnabled())
                {
                    m_return->setEnabled(true);
                }
                if(m_localTest->isEnabled())
            	{
                    m_localTest->setEnabled(false);
            	}
                m_return->setFocus();
                m_partNoEdit->setFocusPolicy(Qt::NoFocus);
//                m_dispatchNoLineEdit->setFocusPolicy(Qt::NoFocus);
                m_projectNoLineEdit->setFocusPolicy(Qt::NoFocus);
                m_startnum = 0;
            }
        }
        else
        {
            if(m_startnum == m_clickedNum)
            {//切换到网络模式时在6秒或12秒更新按键状态
                if(!m_return->isEnabled())
                {
                    m_return->setEnabled(true);
                }
                if(!m_localTest->isEnabled())
                {
                    m_localTest->setEnabled(true);
                }
                if(WorkModeNotifytmp == 1)
                {
                    _log.LOG_INFO("removeWidget 【removeWidget】 command1");
                    m_productGridLayout->removeWidget(m_dispatchNoLineEdit);
                    m_dispatchNoLineEdit->setParent(NULL);
                    m_productGridLayout->addWidget(m_dispatchNoComboBox,0,8,1,2,Qt::AlignLeft | Qt::AlignVCenter);
                    m_partNoEdit->setFocusPolicy(Qt::ClickFocus);
//                    m_dispatchNoLineEdit->setFocusPolicy(Qt::ClickFocus);
                    m_projectNoLineEdit->setFocusPolicy(Qt::ClickFocus);
                    m_restart->setFocus();
//                    if(partNo.empty())
//                    {//PBOX开机或重启以来，上位机软件从来Start过PBOX测试，此时使用上一次PBOX启动时的件号、产品型号、派工单号来默认填充对应的文本框
//                        m_partNoEdit->setText(QString::fromStdString(g_locallineInfo.m_localPartNofirst));
//                        m_dispatchNoLineEdit->setText(QString::fromStdString(g_locallineInfo.m_localDispatchNofirst));
//                        m_projectNoLineEdit->setText(QString::fromStdString(g_locallineInfo.m_localProjectNofirst));
//                    }
                }
                m_startnum = 0;
            }
        }
    }
    bool driverStartFlag = TcpWorkThread::m_driverStartFlag;
    bool driverStopOkFlag = TcpWorkThread::m_driverStopOkFlag;
    if(driverStartFlag != m_lastdriverStartFlag)
    {//设备驱动启动标志变量，设备驱动状态改变后执行
        if(driverStartFlag)
        {//设备驱动启动完成
            if(!driverStopOkFlag)
            {//设备驱动完成，但是马上又检测到驱动立即又再次进入到停止流程，立即清除设备列表
                int tmp = WorkModeNotifyshadow;
                if(tmp == 1)
                {//再次验证工作模式为1，本地测试模式的停止状态，清空设备列表
                    if(m_updateFlag)
                    {
                        timer->stop();
                        m_updateFlag = false;
                        m_table->clear();
                        m_table->setRowCount(0);
                        m_tableNameItem.clear();
                        m_tableValueItem.clear();
                        m_comLabelVec.clear();
                        m_Flag = true;
                    }
                }
            }
            else
            {//设备驱动完成，并且没有检查到设备驱动马上要再次进入到停止流程，加载设备列表
                timer->stop();
                if(!m_updateFlag)
                {
                    m_updateFlag = true;
                    timer->start(400);
                }
            }
        }
        else
        {//设备驱动进入到停止流程，不管是否停止完成先清空设备列表
            timer->stop();
            if(m_updateFlag)
            {
                m_updateFlag = false;
//                usleep(410 * 1000);
                m_table->clear();
                m_table->setRowCount(0);
                m_tableNameItem.clear();
                m_tableValueItem.clear();
                m_comLabelVec.clear();
                m_Flag = true;
            }
//            statustimer->start(100);
        }
        m_lastdriverStartFlag = driverStartFlag;
    }
    m_clickedNum++;
    if((m_clickedNum % 10) == 0)
    {//每秒刷新一次时间
        if(m_clickedNum == 100000000)
        {
            m_clickedNum = 10000;
        }
        m_timeLabel->setText(getLocalTime());
        if(onlydispatchChanged == 1)
        {
            m_dispatchNoLineEdit->setText(QString::fromStdString(dispatchNo));
            onlydispatchChanged = 0;
        }
    }

}

/**
 * @brief 更新数据
 */
void display::updatedata()
{
    if(m_updateFlag)
    {
        //设备的个数
        int num = 0;
        if(g_deviceNum > 0)
        {
            string judgeResult = "";
            string count = IntToString(StationInfo::ProductCount);
            if(m_db.Read_TagMValue(si.JudgeResult) == "1")
            {
                judgeResult = "良品";
            }
            else if(m_db.Read_TagMValue(si.JudgeResult) == "0")
            {
                judgeResult = "不良品";
            }
            m_result->setText(QString::fromStdString(judgeResult));
            m_count->setText(QString::fromStdString(count));

            num = 0;
            for(map<string,DeviceInfo>::iterator it = devInfo.begin();it != devInfo.end();it++,num++)
            {
                DeviceInfo di = it->second;

                int machDriverNo = atoi(di.machDriverNo.data());
                string ivalue = "——————";
                //根据machDriverNo，添加显示界面中设备需要显示的值
                switch(machDriverNo)
                {
                    case KeyenceSr: case DataMan60: case  Wenglor: case VC: case MarkView:  case LM:    //扫码器,视觉
                    {
                        ivalue = m_db.Read_TagMValue(di.BarCode);
                        break;
                    }
                    case SerialCyclone: case UdpCyclone:    //Cyclone烧程器,打标机
                    {
                        ivalue = m_db.Read_TagMValue(di.IdFlag);
                        break;
                    }
                    case FXPLC: case DgPLC: case FX5U:  //PLC
                    {
                        ivalue = m_db.Read_TagMValue("采集点名");
                        break;
                    }
                    //万用表,气压表,手柄
                    case AgilMultMet: case GwMultMet: case Xse6: case TpmsHand: case MicroMeter: case TekTronix: case MicroCaliper: case TekDMM6500:
                    {
                        ivalue = m_db.Read_TagMValue(di.iValue);
                        break;
                    }
                    default:
                        break;
                }
                //设备不使能时，不进行显示
                if(num > -1 && num < g_deviceNum/* && di.Enable*/)
                {
                    m_tableValueItem[num]->setText(QString::fromStdString(ivalue));

                    string communicationstate = m_db.Read_TagMValue(di.Alarm);
                    //显示通信状态            --- 第3列
                    if(communicationstate == "1")
                        m_comLabelVec[num]->setPixmap(QPixmap("./Image/greenDot.jpg").scaled(55, 55, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
                    else
                        m_comLabelVec[num]->setPixmap(QPixmap("./Image/redDot.jpg").scaled(55, 55, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
                }
            }
        }
    }
}

/**
 * @brief 返回按钮点击触发函数
 */
void display::returnback()
{
//    timer->stop();
    this->close();
    m_parent->showUsrchosewidget();
}

void display::localDriverStart()
{
    if(WorkModeNotify == 1)
    {
            string manualpartNo = m_partNoEdit->text().toStdString();
            string manualdispatchNo = m_dispatchNoLineEdit->text().toStdString();
            string manualprojectNo = m_projectNoLineEdit->text().toStdString();

            if(manualpartNo.empty())
            {
                QMessageBox message;
                QFont font;
                font.setPointSize(18);
                message.setFont(font);
                QPushButton *ok = message.addButton(tr("  关闭  "),QMessageBox::ActionRole);
                message.setText(tr("  请 输 入 产 品 件 号 !  "));
                message.move(340,250);
                message.setIcon(QMessageBox::Warning);
                message.exec();
                if(message.clickedButton() == ok)
                {
                    return ;
                }
            }
            if(manualdispatchNo.empty())
            {
                QMessageBox message;
                QFont font;
                font.setPointSize(18);
                message.setFont(font);
                QPushButton *ok = message.addButton(tr("  关闭  "),QMessageBox::ActionRole);
                message.setText(tr("  请 输 入 产 品 派 工 单 号 !  "));
                message.move(340,250);
                message.setIcon(QMessageBox::Warning);
                message.exec();
                if(message.clickedButton() == ok)
                {
                    return ;
                }
            }
            if(manualprojectNo.empty())
            {
                QMessageBox message;
                QFont font;
                font.setPointSize(18);
                message.setFont(font);
                QPushButton *ok = message.addButton(tr("  关闭  "),QMessageBox::ActionRole);
                message.setText(tr("  请 输 入 产 品 型 号 !  "));
                message.move(340,250);
                message.setIcon(QMessageBox::Warning);
                message.exec();
                if(message.clickedButton() == ok)
                {
                    return ;
                }
            }
            QMessageBox message;
            message.setFixedSize(QApplication::desktop()->width()/4,QApplication::desktop()->height()/4);
            QFont font;
            font.setPointSize(18);
            message.setFont(font);
            message.addButton(tr("  是  "),QMessageBox::ActionRole);
            QPushButton *cancel = message.addButton(tr("  否  "),QMessageBox::ActionRole);
            message.setText("  您 是 否 确 实 需 要 启 动 生 产?  ");
            message.move(480,300);
            message.setIcon(QMessageBox::Question);
            message.exec();
            if(message.clickedButton() == cancel)
            {
                message.close();
                return ;
            }
            pthread_rwlock_wrlock(&rwlockPartNoDispatchNo);
            TcpWorkThread::m_manualpartNoNotify = manualpartNo;//m_partNoEdit->text().toStdString();
            TcpWorkThread::m_manualdispatchNoNotify = manualdispatchNo;//m_dispatchNoLineEdit->text().toStdString();
            TcpWorkThread::m_manualprojectNoNotify = manualprojectNo;
            pthread_rwlock_unlock(&rwlockPartNoDispatchNo);
            m_restart->setFocus();
            m_partNoEdit->setFocusPolicy(Qt::NoFocus);
//            m_dispatchNoLineEdit->setFocusPolicy(Qt::NoFocus);
            m_projectNoLineEdit->setFocusPolicy(Qt::NoFocus);

//            m_partNoEdit->setEnabled(false);
//            m_dispatchNoLineEdit->setEnabled(false);
//            m_projectNoLineEdit->setEnabled(false);
            pthread_mutex_lock(&mutexDeviceDriverCond);
            WorkModeNotify = 2;
            pthread_mutex_unlock(&mutexDeviceDriverCond);
            pthread_cond_signal(&cond);
    }
    else
    {
            QMessageBox message;
            message.setFixedSize(QApplication::desktop()->width()/4,QApplication::desktop()->height()/4);
            QFont font;
            font.setPointSize(18);
            message.setFont(font);
            message.addButton(tr("  是  "),QMessageBox::ActionRole);
            QPushButton *cancel = message.addButton(tr("  否  "),QMessageBox::ActionRole);
            message.setText("  您 是 否 确 实 需 要 停 止 生 产?  ");
            message.move(480,300);
            message.setIcon(QMessageBox::Question);
            message.exec();
            if(message.clickedButton() == cancel)
            {
                message.close();
                return ;
            }
            pthread_mutex_lock(&mutexDeviceDriverCond);
            WorkModeNotify = 1;
            pthread_mutex_unlock(&mutexDeviceDriverCond);
            pthread_cond_signal(&cond);
    }
    m_localTest->setEnabled(false);
    m_return->setEnabled(false);
}

/**
 * @brief 重启按钮点击触发函数
 */
void display::reStart()
{
    QMessageBox message;
    message.setFixedSize(QApplication::desktop()->width()/4,QApplication::desktop()->height()/4);
    QFont font;
    font.setPointSize(18);
    message.setFont(font);
    QPushButton *ok = message.addButton(tr("  是  "),QMessageBox::ActionRole);
    QPushButton *cancel = message.addButton(tr("  否  "),QMessageBox::ActionRole);
    message.setText("  是  否  立  即  重  新  启  动  ?  ");
    message.move(480,300);
    message.setIcon(QMessageBox::Question);
    message.exec();
    if(message.clickedButton() == ok)
    {
        system("reboot");
    }
    else if(message.clickedButton() == cancel)
    {
        message.close();
    }
}

/**
 * @brief 获取产品型号
 * @return
 */
string display::GetProductModel()
{
    return m_productmodel;
}

/**
 * @brief 设置产品型号
 * @param productmodel
 */
void display::SetProductModel(string productmodel)
{
    m_productmodel = productmodel;
}

/**
 * @brief 获取工单号
 * @return
 */
string display::GetBillNum()
{
    return m_billnum;
}

/**
 * @brief 设置工单号
 * @param billnum
 */
void display::SetBillNum(string billnum)
{
    m_billnum = billnum;
}

void display::SetUpdateFlag(bool flag)
{
//    m_updateFlag = flag;
}

QString display::getLocalTime()
{
    QDateTime now;
    QDate m_date;
    QTime m_time;
    now.setDate(m_date.currentDate());
    now.setTime(m_time.currentTime());
    QString currenttime = now.toString("  yyyy-MM-dd  hh:mm:ss  ");
    return currenttime;
}

void display::updateDispatchInfo(QString dispatchnotmp)
{
    string partno;
//    string projectno;
    if(dispatchnotmp != "")
    {
        size_t num = currDistpatchList.size();
        for(size_t i = 0;i < num;i++)
        {
            if(currDistpatchList[i].Dispatch_No == dispatchnotmp.toStdString())
            {
                partno = currDistpatchList[i].Part_No;
//                projectno = currDistpatchList[i].ProjectNo;
                break;
            }
        }
    }
    m_dispatchNoLineEdit->setText(dispatchnotmp);
    m_partNoEdit->setText(QString::fromStdString(partno));
//    _log.LOG_DEBUG("setText Dispatch_No【%s】partno【%s】",dispatchnotmp.toStdString().data(),partno.data());
//    m_projectNoLineEdit->setText(QString::fromStdString(projectno));
}


void display::paintEvent(QPaintEvent *)
{
//    if(TcpWorkThread::m_ShowDeviceListFlag)
//    {
        int tmpWorkMode = WorkModeNotify;
        int tmpSampleMode = EnableSampleMode;
        if((tmpWorkMode != m_preWorkModeNotify) || (tmpSampleMode != m_preEnableSampleMode))
        {
            if(tmpWorkMode == 0)
            {
                m_currworkMode->setText(tr("网络模式"));
                m_craftNameLabel->setText(QString::fromStdString(gLine.Si.Name));
            }
            else
            {
                if(tmpSampleMode == 1)
                {
                    m_currworkMode->setText(tr("样件模式"));
                    m_craftNameLabel->setText(QString::fromStdString(gLine.Si.Name + "【样件测试】"));
                }
                else
                {
                    m_currworkMode->setText(tr("本地模式"));
                    m_craftNameLabel->setText(QString::fromStdString(gLine.Si.Name));
                }
            }
            m_preWorkModeNotify = tmpWorkMode;
            m_preEnableSampleMode = tmpSampleMode;
        }
//    }
}

void display::PartNoChanged(QString newpartno)
{
    currEditPartNo = newpartno.toStdString();
}

//void display::getPartNoString()
//{
//    currEditPartNo = m_partNoEdit->text().toStdString();
//}
