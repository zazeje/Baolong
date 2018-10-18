#include "PlcControl.h"

/**
 * @brief 自定义部件类构造函数
 * 部件包含：置位及复位按钮，组合框，横向布局器
 */
ControlWidgetParts::ControlWidgetParts()
{
    m_plcState = false;
    m_box = new QGroupBox();
    m_layout = new QHBoxLayout();
    m_pulseButton = new MyPushButton(QObject::tr("脉冲"));
    m_pulseButton->setFixedSize(160,40);
    m_layout->setMargin(0);
    m_layout->addWidget(m_pulseButton);
    m_box->setLayout(m_layout);

    m_timer = new QTimer();
    connect(m_timer,SIGNAL(timeout()),this,SLOT(EnableButton()));

    m_clickNum = 0;

}

/**
 * @brief 自定义部件类构造函数
 */
ControlWidgetParts::~ControlWidgetParts()
{
    if(m_pulseButton != NULL)
        delete m_pulseButton;
    if(m_layout != NULL)
        delete m_layout;
    if(m_box != NULL)
        delete m_box;
    delete m_timer;
}

void ControlWidgetParts::EnableButton()
{
    m_clickNum = 0;
    m_timer->stop();
}

/**
 * @brief PLC动作控制界面构造函数
 * @param parent
 */
PlcControl::PlcControl(mainWidget *parent) : m_parent(parent)
{
    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();

    m_widgetType = 2;

    m_pointInfoVec = PlcControlVec;

    QPalette palette;
    //设置字体颜色
    palette.setColor(QPalette::WindowText,Qt::white);
    //加载背景图片
    palette.setBrush(QPalette::Background, QBrush(QPixmap("./Image/backgroud1.jpg").scaled(QApplication::desktop()->size())));
    //设置字体大小
    QFont font;
    font.setPointSize(20);

    //构造标题栏
    m_titleLabel = new QLabel(tr("机械动作"));
    m_titleLabel->setFont(font);
    m_titleHLayout = new QHBoxLayout();
    m_titleHLayout->addStretch();
    m_titleHLayout->addWidget(m_titleLabel);
    m_titleHLayout->addStretch();
    m_titleGroup = new QGroupBox();
    m_titleGroup->setLayout(m_titleHLayout);
    m_titleVLayout = new QVBoxLayout();
    m_titleVLayout->addWidget(m_titleGroup);

    //构造上下翻页栏
    m_previousPage = new QPushButton(tr("上一页"));
    m_nextPage = new QPushButton(tr("下一页"));
    m_previousPage->setFixedWidth(width / 10);
    m_nextPage->setFixedWidth(width / 10);
    connect(m_nextPage,SIGNAL(clicked()),this,SLOT(GoToNextPage()));
    connect(m_previousPage,SIGNAL(clicked()),this,SLOT(GoToPreviousPage()));
    m_pageMessage = new QLabel(tr("当前第    /    页"));
    font.setPointSize(15);
    m_pageMessage->setFont(font);
    m_pageLayout = new QHBoxLayout();
    m_pageLayout->addWidget(m_previousPage);
    m_pageLayout->addWidget(m_pageMessage);
    m_pageLayout->addWidget(m_nextPage);
    m_pageHBoxLayout = new QHBoxLayout();
    m_pageHBoxLayout->addStretch();
    m_pageHBoxLayout->addLayout(m_pageLayout);

    //构造表格
    m_tableWidget = new QTableWidget();
    m_tableWidget->setColumnCount(4);
    m_tableWidget->setRowCount((m_pointInfoVec.size() / 10 + 1) * 10);
    m_currentPageItems = 10;
    m_currentPageIndex = 1;
    m_tableWidget->setFixedWidth(width - 32);
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
    m_tableWidget->verticalScrollBar()->setHidden(true);
    //设置表格具体行、列的宽度及高度
    m_tableWidget->horizontalHeader()->setFixedHeight(35);
    m_tableWidget->setColumnWidth(0,(width - 10)/4 - 40);
    m_tableWidget->setColumnWidth(1,(width - 10)/4);
    m_tableWidget->setColumnWidth(2,(width - 10)/4);
    m_tableWidget->setColumnWidth(3,(width - 10)/4);
    //隐藏所有的行
    for(int i = 0;i < (m_pointInfoVec.size() / 10 + 1) * 10;i++)
        m_tableWidget->setRowHidden(i,true);
    //构造返回按钮栏
    m_returnButton = new QPushButton(tr("返回"));
    m_returnButton->setFixedSize(130,38);
    connect(m_returnButton,SIGNAL(clicked()),this,SLOT(ReturnSlots()));
    m_returnLayout = new QHBoxLayout();
    m_returnLayout->addStretch();
    m_returnLayout->addWidget(m_returnButton);

    //将主体部分组合后与标题部分进行统一布局
    m_vBoxLayout = new QVBoxLayout();
    m_vBoxLayout->addLayout(m_pageHBoxLayout);
    m_vBoxLayout->addWidget(m_tableWidget);
    m_vBoxLayout->addLayout(m_returnLayout);
    m_mainPartBox = new QGroupBox();
    m_mainPartBox->setLayout(m_vBoxLayout);
    m_wholeLayout = new QVBoxLayout();
    m_wholeLayout->addLayout(m_titleVLayout);
    m_wholeLayout->addWidget(m_mainPartBox);

    UpdatePlcPoint();

    this->setPalette(palette);
    this->setLayout(m_wholeLayout);
}

PlcControl::~PlcControl()
{}


/**
 * @brief 更新表格中显示的PLC项目信息
 */
void PlcControl::UpdatePlcPoint()
{
    DeleteTableWidgetItem();

    //设置列名
    QStringList list;
    list.append("项目序号");
    list.append("项目名称");
    list.append("寄存器地址");
    list.append("控制");
    m_tableWidget->setHorizontalHeaderLabels(list);
    //根据PLC点位绘制显示条目
    if(!m_pointInfoVec.empty())
    {
        UpdatePageMessage();
        for(int i = 0;i < m_pointInfoVec.size();i++)
        {
            if(i > m_pointInfoVec.size() - 1)
                break;
            //每一个PLC点位对应一个控制部件集合
            ControlWidgetParts* part = new ControlWidgetParts();
            part->m_pulseButton->SetMyButtonNum(i);
            connect(part->m_pulseButton,SIGNAL(clicked()),part->m_pulseButton,SLOT(EmitClickedByNum()));
            connect(part->m_pulseButton,SIGNAL(MyButtonClicked(int)),this,SLOT(PlcSetPulse(int)));
            //将每个控制部件集合按顺序加入到m_partsInfoVec数组当中
            m_partsInfoVec.push_back(part);
            //记录每个PLC点位序号
            m_pointInfoVec[i].m_pointNum = i;
            QTableWidgetItem *item1 = new QTableWidgetItem(QString::fromStdString(IntToString(i + 1)));
            item1->setTextAlignment(Qt::AlignCenter);
            m_tableItem.push_back(item1);
            QTableWidgetItem *item2 = new QTableWidgetItem(QString::fromStdString(m_pointInfoVec[i].m_pointName));
            item2->setTextAlignment(Qt::AlignCenter);
            m_tableItem.push_back(item2);
            QTableWidgetItem *item3 = new QTableWidgetItem(QString::fromStdString(m_pointInfoVec[i].m_registerType + IntToString(m_pointInfoVec[i].m_registerAddr)));
            item3->setTextAlignment(Qt::AlignCenter);
            m_tableItem.push_back(item3);
            //显示项目序号列的内容        --- 第1列
            m_tableWidget->setItem(i, 0, item1);
            //显示项目名称列的内容        --- 第2列
            m_tableWidget->setItem(i, 1, item2);
            //显示寄存器地址列的内容       --- 第3列
            m_tableWidget->setItem(i, 2, item3);
            //显示控制列的按钮            --- 第4列
            m_tableWidget->setCellWidget(i, 3, part->m_box);
        }
    }
}

/**
 * @brief 释放表格中动态申请的资源
 */
void PlcControl::DeleteTableWidgetItem()
{
    if(!m_partsInfoVec.empty())
    {
        for(int i = 0;i < m_partsInfoVec.size();i++)
        {
            ControlWidgetParts* part = m_partsInfoVec[i];
            if(part != NULL)
                part->~ControlWidgetParts();
        }
    }
    if(!m_tableItem.empty())
    {
        for(int i = 0;i < m_tableItem.size();i++)
        {
            if(m_tableItem.at(i) != NULL)
                delete m_tableItem.at(i);
        }
    }
    m_partsInfoVec.clear();
    m_tableItem.clear();
    m_tableWidget->clear();
}

void PlcControl::CheckPlcInfo()
{
    m_pointInfoVec = PlcControlVec;
    UpdatePlcPoint();
}


/**
 * @brief 返回按钮槽函数
 */
void PlcControl::ReturnSlots()
{
    this->close();
    SetReadPlcFlag(false);
    SetDisplayType(0);
    m_parent->showPlcDisplay();
}

/**
 * @brief 脉冲按钮槽函数
 * @param num
 */
void PlcControl::PlcSetPulse(int num)
{
    ControlWidgetParts* part = m_partsInfoVec[num];
    if(part->m_clickNum == 0)
    {
        int addr = m_pointInfoVec[num].m_registerAddr;
        part->m_timer->start(1000);
        bool res = m_device->WriteBits(addr, "1", m_pointInfoVec[num].m_registerType);
        _log.LOG_DEBUG("PlcControl WriteBits(1) 向PLC点位【%d】置位 结果为【%d】",addr,res);
        usleep(50 * 1000);//脉冲信号延时50ms
        res = m_device->WriteBits(addr, "0", m_pointInfoVec[num].m_registerType);
        _log.LOG_DEBUG("PlcControl WriteBits(0) 向PLC点位【%d】置位 结果为【%d】",addr,res);
    }
    part->m_clickNum++;
}

/**
 * @brief 复位/置位按钮槽函数
 * @param num
 */
void PlcControl::PlcChangeState(int num)
{
    int value = !m_partsInfoVec[num]->m_plcState;
    m_device->WriteBits(m_pointInfoVec[num].m_registerAddr, IntToString(value), m_pointInfoVec[num].m_registerType);
}

