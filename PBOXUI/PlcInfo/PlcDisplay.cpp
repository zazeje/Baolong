#include "PlcDisplay.h"
#include "main.h"


PlcDisplay::PlcDisplay(mainWidget *parent) : m_parent(parent)
{
    m_pointInfoVec = PlcAlarmVec;

    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();

    QPalette palette;
    palette.setColor(QPalette::WindowText,Qt::white);
    palette.setBrush(QPalette::Background, Qt::black);
    QFont font25;
    font25.setPointSize(25);
    QFont font17;
    font17.setPointSize(17);

    m_timeLabel = new QLabel();
    m_timeLabel->setFixedHeight(25);
    m_timeLabel->setAlignment(Qt::AlignRight);

    m_titleLabel = new QLabel();
    m_judgestateLabel = new QLabel(tr("良品判定："));
    m_judgeLabel = new QLabel();
    m_judgeLabel->setAlignment(Qt::AlignCenter);
    m_titleHLayout = new QHBoxLayout();
    m_titleHLayout->addWidget(m_judgestateLabel);
    m_titleHLayout->addWidget(m_judgeLabel);
    m_titleHLayout->addStretch();
    m_titleHLayout->addWidget(m_titleLabel);
    m_titleHLayout->addStretch();
    m_titleHLayout->addStretch();
    m_titleHLayout->addWidget(m_timeLabel);
    m_titleGroup = new QGroupBox();
    m_titleGroup->setFont(font25);
    m_timeLabel->setFont(font17);
    m_titleGroup->setFixedSize(width-20,height/7);
    m_titleGroup->setLayout(m_titleHLayout);

    m_devicestateLabel = new QLabel(tr("设备状态"));
    m_devicestateLabel->setFont(font17);
    m_stateLabel = new QLabel(tr("待机中"));
    m_stateLabel->setFont(font17);

    m_rhythmLabel = new QLabel(tr("节拍时间"));
    m_rhythmLabel->setFont(font17);
    m_rhythmValueLabel = new QLabel();
    m_rhythmValueLabel->setFont(font17);
    m_rhythmUnitLabel = new QLabel(tr("s"));
    m_rhythmUnitLabel->setFont(font17);

    m_productcountLabel = new QLabel(tr("产品计数"));
    m_productcountLabel->setFont(font17);
    m_countLabel = new QLabel();
    m_countLabel->setFont(font17);
    m_countclearButton = new QPushButton(tr("产品计数清零"));
    m_countclearButton->setFont(font17);


    /***************************************
     *
     *
     *  报警列表界面
     *
     * **************************************/

    m_tableWidget = new QTableWidget();
    m_tableWidget->setColumnCount(1);
    m_tableWidget->setRowCount(RowNum);
    //设置表格整体宽度
    m_tableWidget->setFixedWidth(width/3 - 10);
    //设置列宽
    m_tableWidget->setColumnWidth(0,width/3 - 10);
    QStringList list;
    list<<"报警列表";
    m_tableWidget->setHorizontalHeaderLabels(list);
    //设置表头字体
    QFont font = m_tableWidget->horizontalHeader()->font();
    font.setBold(true);
    m_tableWidget->horizontalHeader()->setFont(font);
    //设置表头字体颜色(白色)
    m_tableWidget->horizontalHeader()->setStyleSheet("color: rgb(255, 255, 255)");
    //设置QTableWidget表格背景颜色（透明）
    m_tableWidget->setStyleSheet("background-color:rgba(0,0,0,0)");
    m_tableWidget->setFocusPolicy(Qt::NoFocus);
    //设置表格不可编辑
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //设置表格不可被选中
    m_tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    //设置默认行高
    m_tableWidget->verticalHeader()->setDefaultSectionSize(height / 15 + 1);
    //隐藏行号，行、列滚动条、设置tableWidget不可滚动
    m_tableWidget->verticalHeader()->setHidden(true);
    m_tableWidget->horizontalScrollBar()->setHidden(true);


    QFont ft;
    ft.setBold(true);
    //默认设置100列
    for(int i=0; i<RowNum; i++)
    {
        //设置QTableWidgetItem显示，居中显示
        QTableWidgetItem *item0 = new QTableWidgetItem();
        item0->setFont(ft);
        //设置表格里面字体颜色（红色）
        item0->setForeground(QBrush(QColor(255, 0, 0)));
        item0->setTextAlignment(Qt::AlignCenter);
        m_tableAlarmItem.push_back(item0);

        //显示报警内容   --- 第1列
        m_tableWidget->setItem(i,0, item0);
    }

    m_alarmHBoxLayout = new QHBoxLayout();
    m_alarmHBoxLayout->setAlignment(Qt::AlignCenter);
    m_alarmHBoxLayout->addWidget(m_tableWidget);

    m_alarmGroupBox = new QGroupBox();
    m_alarmGroupBox->setAlignment(Qt::AlignCenter);
    m_alarmGroupBox->setFixedWidth(width/3);
    m_alarmGroupBox->setLayout(m_alarmHBoxLayout);


    m_settingButton = new QPushButton(tr("PLC参数设置"));
    m_settingButton->setFixedHeight(height/12);
//    m_settingButton->setStyleSheet("background-color:rgb(0,255,255);");
    m_manualButton = new QPushButton(tr("PLC控制"));
    m_manualButton->setFixedHeight(height/12);
//    m_manualButton->setStyleSheet("background-color:rgb(0,255,255);");
    m_stateButton = new QPushButton(tr("PLC状态"));
    m_stateButton->setFixedHeight(height/12);
//    m_stateButton->setStyleSheet("background-color:rgb(0,255,255);");
    m_modifyButton = new QPushButton(tr("PLC修改"));
    m_modifyButton->setFixedHeight(height/12);
//    m_modifyButton->setStyleSheet("background-color:rgb(0,255,255);");
    m_startButton = new MyPushButton(tr("启动"));
    m_startButton->setFixedHeight(height/12);
//    m_startButton->setStyleSheet("background-color:rgb(0,255,0);");
    m_resetButton = new MyPushButton(tr("复位"));
    m_resetButton->setFixedHeight(height/12);
//    m_resetButton->setStyleSheet("background-color:rgb(255,255,0);");
    m_stopButton = new MyPushButton(tr("停止"));
    m_stopButton->setFixedHeight(height/12);
//    m_stopButton->setStyleSheet("background-color:rgb(255,0,0);");
    m_returnButton = new QPushButton(tr("返回"));
    m_returnButton->setFixedHeight(height/12);
//    m_returnButton->setStyleSheet("background-color:rgb(255,255,0);");


    m_buttonHLayout = new QHBoxLayout();
    m_buttonHLayout->addWidget(m_settingButton);
    m_buttonHLayout->addWidget(m_manualButton);
    m_buttonHLayout->addWidget(m_stateButton);
    m_buttonHLayout->addWidget(m_modifyButton);

    m_buttonHLayout_1 = new QHBoxLayout();
    m_buttonHLayout_1->addWidget(m_startButton);
    m_buttonHLayout_1->addWidget(m_resetButton);
    m_buttonHLayout_1->addWidget(m_stopButton);
    m_buttonHLayout_1->addWidget(m_returnButton);

    m_stateGridLayout = new QGridLayout();
    m_stateGridLayout->setColumnStretch(0, 1);
    m_stateGridLayout->setColumnStretch(1, 1);
    m_stateGridLayout->setColumnStretch(2, 1);
    m_stateGridLayout->setColumnStretch(3, 1);
    m_stateGridLayout->setColumnStretch(4, 1);
    m_stateGridLayout->setColumnStretch(5, 1);
    m_stateGridLayout->setColumnStretch(6, 1);
    m_stateGridLayout->addWidget(m_devicestateLabel,0,1,1,1,Qt::AlignLeft | Qt::AlignVCenter);
    m_stateGridLayout->addWidget(m_stateLabel,0,3,1,2,Qt::AlignLeft | Qt::AlignVCenter);
    m_stateGridLayout->addWidget(m_rhythmLabel,1,1,1,1,Qt::AlignLeft | Qt::AlignVCenter);
    m_rhythmHLayout = new QHBoxLayout();
    m_rhythmHLayout->addWidget(m_rhythmValueLabel);
    m_rhythmHLayout->addSpacing(10);
    m_rhythmHLayout->addWidget(m_rhythmUnitLabel);
    m_rhythmHLayout->addStretch();
    m_stateGridLayout->addLayout(m_rhythmHLayout,1,3,1,2,Qt::AlignLeft | Qt::AlignVCenter);
    m_stateGridLayout->addWidget(m_productcountLabel,2,1,1,1,Qt::AlignLeft | Qt::AlignVCenter);
    m_stateGridLayout->addWidget(m_countLabel,2,3,1,1,Qt::AlignLeft | Qt::AlignVCenter);
    m_stateGridLayout->addWidget(m_countclearButton,2,5,1,2,Qt::AlignHCenter | Qt::AlignVCenter);

    m_stateVBoxLayout = new QVBoxLayout();
    m_stateVBoxLayout->addSpacing(25);
    m_stateVBoxLayout->addLayout(m_stateGridLayout);
    m_stateVBoxLayout->addSpacing(30);

    m_pagecontentGroupBox = new QGroupBox();
    m_pagecontentGroupBox->setLayout(m_stateVBoxLayout);

    m_secondHBoxLayout = new QHBoxLayout();
    m_secondHBoxLayout->setAlignment(Qt::AlignCenter);
    m_secondHBoxLayout->addWidget(m_pagecontentGroupBox);
    m_secondHBoxLayout->addWidget(m_alarmGroupBox);

    m_buttonVLayout = new QVBoxLayout();
//    m_buttonVLayout->addWidget(m_pagecontentGroupBox);
    m_buttonVLayout->addLayout(m_secondHBoxLayout);
    m_buttonVLayout->addLayout(m_buttonHLayout);
    m_buttonVLayout->addLayout(m_buttonHLayout_1);

    m_pageGroupBox = new QGroupBox();
    m_pageGroupBox->setFixedSize(width-20,height*6/7 - 20);
    m_pageGroupBox->setLayout(m_buttonVLayout);

    m_vBoxLayout = new QVBoxLayout();
    m_vBoxLayout->setAlignment(Qt::AlignCenter);
    m_vBoxLayout->addWidget(m_titleGroup);
    m_vBoxLayout->addWidget(m_pageGroupBox);

    this->setLayout(m_vBoxLayout);
    this->setPalette(palette);

    connect(m_countclearButton,SIGNAL(clicked()),this,SLOT(clearProductcount()));
    connect(m_settingButton,SIGNAL(clicked()),this,SLOT(setParameter()));
    connect(m_manualButton,SIGNAL(clicked()),this,SLOT(manualSetting()));
    connect(m_stateButton,SIGNAL(clicked()),this,SLOT(stateDisplay()));
    connect(m_modifyButton,SIGNAL(clicked()),this,SLOT(excelModify()));
    connect(m_startButton,SIGNAL(clicked()),this,SLOT(startPlc()));
    connect(m_resetButton,SIGNAL(clicked()),this,SLOT(resetPlc()));
    connect(m_stopButton,SIGNAL(clicked()),this,SLOT(stopPlc()));
    connect(m_returnButton,SIGNAL(clicked()),this,SLOT(returnTo()));

    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(updateData()));
    m_clearButtonTimer = new QTimer(this);
    connect(m_clearButtonTimer,SIGNAL(timeout()),this,SLOT(enableClearButton()));
    m_startButtonTimer = new QTimer(this);
    connect(m_startButtonTimer,SIGNAL(timeout()),this,SLOT(enableStartButton()));
    m_stopButtonTimer = new QTimer(this);
    connect(m_stopButtonTimer,SIGNAL(timeout()),this,SLOT(enableStopButton()));
    m_resetButtonTimer = new QTimer(this);
    connect(m_resetButtonTimer,SIGNAL(timeout()),this,SLOT(enableResetButton()));
    m_clearNum = 0;
    m_startNum = 0;
    m_stopNum = 0;
    m_resetNum = 0;
}

PlcDisplay::~PlcDisplay()
{
    delete m_timeLabel;
    delete m_pageGroupBox;
    delete m_judgestateLabel;
    delete m_judgeLabel;
    delete m_devicestateLabel;
    delete m_stateLabel;
    delete m_productcountLabel;
    delete m_countLabel;
    delete m_countclearButton;
    delete m_settingButton;
    delete m_manualButton;
    delete m_startButton;
    delete m_resetButton;
    delete m_stopButton;
    delete m_buttonHLayout;
    delete m_parent;
    delete m_stateButton;
    delete m_modifyButton;
    delete m_rhythmLabel;
    delete m_rhythmValueLabel;
    delete m_rhythmUnitLabel;
    delete m_rhythmHLayout;
    delete m_stateVBoxLayout;
    delete m_stateGridLayout;
    delete m_clearButtonTimer;
    delete m_startButtonTimer;
    delete m_stopButtonTimer;
    delete m_resetButtonTimer;
    delete m_alarmGroupBox;
    delete m_secondHBoxLayout;

}

QString PlcDisplay::getLocalTime()
{
    QDateTime now;
    QDate m_date;
    QTime m_time;
    now.setDate(m_date.currentDate());
    now.setTime(m_time.currentTime());
    QString currenttime = now.toString("  yyyy-MM-dd  hh:mm:ss  ");
    return currenttime;
}

/**
 * @brief PlcDisplay::updateAlarmInfo   界面中显示报警信息
 */
void PlcDisplay::updateAlarmInfo()
{
    int count=0;

    for(int i=0; i<RowNum; i++)
        m_tableAlarmItem[i]->setText("");

    //表格中显示PLC报警点
    for(int i=0; i<m_pointInfoVec.size(); i++)
    {
        string alarmValue = m_db.Read_TagMValue(m_pointInfoVec[i].m_pointName);

        if(!alarmValue.compare("1"))
        {
            m_tableAlarmItem[count++]->setText(QString::fromStdString(m_pointInfoVec[i].m_pointName));
        }
    }

    //表格中显示设备参数报警
    for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++)
    {
        DeviceInfo di = it->second;
        string devContent = m_db.Read_TagMValue(di.devParaAlarm);
        string testContent = m_db.Read_TagMValue(di.testParaAlarm);
        string communiContent = m_db.Read_TagMValue(di.Alarm);
        string devInitContent = m_db.Read_TagMValue(di.devInitAlarm);
        //加工参数、测试项参数不为空时，证明参数不全
        if(!devContent.empty())
        {
            string item1 = "【" + di.Name + "】获取加工参数【失败】";
            m_tableAlarmItem[count++]->setText(QString::fromStdString(item1));
        }
        if(!testContent.empty())
        {
            string item2 = "【" + di.Name + "】获取测试项参数【失败】";
            m_tableAlarmItem[count++]->setText(QString::fromStdString(item2));
        }
        //通信参数值为“0”时，说明通信异常
        if(communiContent == "0")
        {
            string item3 = "【" + di.Name + "】通信连接【失败】";
//            cout<<"----- count = "<<count<<endl;
            m_tableAlarmItem[count++]->setText(QString::fromStdString(item3));
        }
        //设备初始化报警
        if(!devInitContent.empty())
        {
            string item4 = "【" + di.Name + "】设备初始化【失败】";
            m_tableAlarmItem[count++]->setText(QString::fromStdString(item4));
        }
    }

#if 0
    for(map<string,DeviceInfo>::iterator it = gLine.Si.Dis.begin(); it != gLine.Si.Dis.end();it++)
    {
        DeviceInfo di = it->second;
        //报警界面更新显示加工参数信息
        for(int i=0;i<di.parameterNo.size();i++)
        {
            string devContent = m_db.Read_TagMValue(di.Name + di.parameterNo.at(i) + "dev");
            if(!devContent.empty())
            {
                string item1 = "【" + di.Name + "】获取【加工参数】" + devContent + "【失败】";
                m_tableAlarmItem[count++]->setText(QString::fromStdString(item1));
            }
        }

        //报警界面更新显示测试项参数信息
        for(int j=0;j<di.testItemCode.size();j++)
        {
            //标准值
            string StandardValue = m_db.Read_TagMValue(di.Name + di.testItemCode.at(j) + "SV");
            if(!StandardValue.empty())
            {
                string item2 = "【" + di.Name + "】获取【测试项参数】" + StandardValue + "【失败】";
                m_tableAlarmItem[count++]->setText(QString::fromStdString(item2));
            }
            //最大值
            string MaxValue = m_db.Read_TagMValue(di.Name + di.testItemCode.at(j) + "MAV");
            if(!MaxValue.empty())
            {
                string item3 = "【" + di.Name + "】获取【测试项参数】" + MaxValue + "【失败】";
                m_tableAlarmItem[count++]->setText(QString::fromStdString(item3));
            }
            //最小值
            string MinValue = m_db.Read_TagMValue(di.Name + di.testItemCode.at(j) + "MIV");
            if(!MinValue.empty())
            {
                string item4 = "【" + di.Name + "】获取【测试项参数】" + MinValue + "【失败】";
                m_tableAlarmItem[count++]->setText(QString::fromStdString(item4));
            }
            //偏差范围
            string Deviation = m_db.Read_TagMValue(di.Name + di.testItemCode.at(j) + "DT");
            if(!Deviation.empty())
            {
                string item5 = "【" + di.Name + "】获取【测试项参数】" + Deviation + "【失败】";
                m_tableAlarmItem[count++]->setText(QString::fromStdString(item5));
            }
            //最低合格率
            string MinPassRate = m_db.Read_TagMValue(di.Name + di.testItemCode.at(j) + "MP");
            if(!MinPassRate.empty())
            {
                string item6 = "【" + di.Name + "】获取【测试项参数】" + MinPassRate + "【失败】";
                m_tableAlarmItem[count++]->setText(QString::fromStdString(item6));
            }
            //判定使能
            string JudgeEnable = m_db.Read_TagMValue(di.Name + di.testItemCode.at(j) + "JE");
            if(!JudgeEnable.empty())
            {
                string item7 = "【" + di.Name + "】获取【测试项参数】" + JudgeEnable + "【失败】";
                m_tableAlarmItem[count++]->setText(QString::fromStdString(item7));
            }
        }
    }
#endif
}


void PlcDisplay::updateData()
{
    m_timeLabel->setText(getLocalTime());
    string data = GetProductCount();
    if(!data.empty())
        m_countLabel->setText(QString::fromStdString(data));
    data = GetCycleTime();
    if(!data.empty())
        m_rhythmValueLabel->setText(QString::fromStdString(data));
    string judgeResult = m_db.Read_TagMValue(gLine.Si.JudgeResult);
    if(judgeResult == "1")
        m_judgeLabel->setPixmap(QPixmap("./Image/judgeGreen.jpg").scaled(55, 55, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    else
        m_judgeLabel->setPixmap(QPixmap("./Image/judgeRed.jpg").scaled(55, 55, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    m_titleLabel->setText(QString::fromStdString(gLine.Si.Name));
    if(!GetCommunicateState())
        m_stateLabel->setText(tr("触摸屏通信失败"));
    else
        m_stateLabel->setText(tr("待机中..."));

    updateAlarmInfo();
}


void PlcDisplay::clearProductcount()
{
    if(m_clearNum == 0)
    {
        m_clearButtonTimer->start(1000);
        SetReadPlcFlag(false);
        m_countclearButton->setEnabled(false);
        m_device->WriteBits(501,"1","M");
        usleep(50 * 1000);
        m_device->WriteBits(501,"0","M");
        m_countclearButton->setEnabled(true);
        SetReadPlcFlag(true);
    }
    m_clearNum++;
}

void PlcDisplay::setParameter()
{
    m_timer->stop();
    this->close();
    SetDisplayType(3);
    m_parent->showPlcSetting();
}

void PlcDisplay::manualSetting()
{
    m_timer->stop();
    this->close();
    SetDisplayType(0);
    m_parent->showPlcControl();
}

void PlcDisplay::startPlc()
{
    if(m_startNum == 0)
    {
        m_startButtonTimer->start(1000);
        SetReadPlcFlag(false);
        m_resetButton->setEnabled(false);
        m_device->WriteBits(502,"1","M");
        usleep(50 * 1000);
        m_device->WriteBits(502,"0","M");
        m_resetButton->setEnabled(true);
        SetReadPlcFlag(true);
    }
    m_startNum++;
}

void PlcDisplay::stopPlc()
{
    if(m_stopNum == 0)
    {
        m_stopButtonTimer->start(1000);
        SetReadPlcFlag(false);
        m_resetButton->setEnabled(false);
        m_device->WriteBits(503,"1","M");
        usleep(50 * 1000);
        m_device->WriteBits(503,"0","M");
        m_resetButton->setEnabled(true);
        SetReadPlcFlag(true);
    }
    m_stopNum++;
}

void PlcDisplay::resetPlc()
{
    if(m_resetNum == 0)
    {
        m_resetButtonTimer->start(1000);
        SetReadPlcFlag(false);
        m_resetButton->setEnabled(false);
        m_device->WriteBits(500,"1","M");
        usleep(50 * 1000);
        m_device->WriteBits(500,"0","M");
        m_resetButton->setEnabled(true);
        SetReadPlcFlag(true);
    }
    m_resetNum++;
}

void PlcDisplay::returnTo()
{
    m_timer->stop();
    SetReadPlcFlag(false);
    SetDisplayType(0);
    this->close();
    m_parent->showUsrchosewidget();
}

void PlcDisplay::stateDisplay()
{
    m_timer->stop();
    this->close();
    SetDisplayType(1);
    m_parent->showPlcState();
}

void PlcDisplay::excelModify()
{
    m_timer->stop();
    this->close();
    m_parent->showPlcModifyItem();
}

void PlcDisplay::enableClearButton()
{
    m_clearNum = 0;
    m_clearButtonTimer->stop();
}

void PlcDisplay::enableStartButton()
{
    m_startNum = 0;
    m_startButtonTimer->stop();
}

void PlcDisplay::enableStopButton()
{
    m_stopNum = 0;
    m_stopButtonTimer->stop();
}

void PlcDisplay::enableResetButton()
{
    m_resetNum = 0;
    m_resetButtonTimer->stop();
}

void PlcDisplay::UpdatePlcPoint()
{}
void PlcDisplay::DeleteTableWidgetItem()
{}
void PlcDisplay::ReturnSlots()
{}


