#include "PlcState.h"

StateWidgetParts::StateWidgetParts()
{
    m_label = new QLabel();
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setPixmap(QPixmap("./Image/redDot.jpg").scaled(55, 55, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    connect(this,SIGNAL(ChangeStateSignal(bool)),this,SLOT(ChangeStateSlot(bool)));
}
StateWidgetParts::~StateWidgetParts()
{
    delete m_label;
}

void StateWidgetParts::ChangeStateSlot(bool state)
{
    m_lastState = state;
    if(!state)
        m_label->setPixmap(QPixmap("./Image/greenDot.jpg").scaled(55, 55, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    else
        m_label->setPixmap(QPixmap("./Image/redDot.jpg").scaled(55, 55, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}


PlcState::PlcState(mainWidget *parent) : m_parent(parent)
{
    m_widgetType = 1;

    m_width = QApplication::desktop()->width();
    m_height = QApplication::desktop()->height();

    QPalette palette;
    palette.setColor(QPalette::WindowText,Qt::white);
    palette.setBrush(QPalette::Background, Qt::black);
    QFont font20;
    font20.setPointSize(25);

    m_titleLabel = new QLabel(tr("PLC信号监控"));
    m_titleHLayout = new QHBoxLayout();
    m_titleHLayout->addWidget(m_titleLabel);
    m_titleHLayout->setAlignment(Qt::AlignHCenter);
    m_titleGroup = new QGroupBox();
    m_titleGroup->setFont(font20);
    m_titleGroup->setLayout(m_titleHLayout);

    m_previousPage = new QPushButton(tr("上一页"));
    m_nextPage = new QPushButton(tr("下一页"));
    m_previousPage->setFixedWidth(m_width / 10);
    m_nextPage->setFixedWidth(m_width / 10);
    connect(m_nextPage,SIGNAL(clicked()),this,SLOT(GoToNextPage()));
    connect(m_previousPage,SIGNAL(clicked()),this,SLOT(GoToPreviousPage()));
    m_pageMessage = new QLabel(tr("当前第    /    页"));
    QFont font;
    font.setPointSize(15);
    m_pageMessage->setFont(font);
    m_pageLayout = new QHBoxLayout();
    m_pageLayout->addWidget(m_previousPage);
    m_pageLayout->addWidget(m_pageMessage);
    m_pageLayout->addWidget(m_nextPage);
    m_pageHBoxLayout = new QHBoxLayout();
    m_pageHBoxLayout->addStretch();
    m_pageHBoxLayout->addLayout(m_pageLayout);

    m_tableWidget = new QTableWidget();

    //设置默认行高
    m_tableWidget->verticalHeader()->setDefaultSectionSize(m_height / 15 + 2);
    m_tableWidget->setColumnCount(9);
    m_tableWidget->setRowCount(100);
    //隐藏所有的行
    for(int i = 0;i < 100;i++)
        m_tableWidget->setRowHidden(i,true);
    m_currentPageItems = 30;
    m_currentPageIndex = 1;
    m_tableWidget->setFixedWidth(m_width - 50);
    m_tableWidget->setFocusPolicy(Qt::NoFocus);
    //设置表格不可编辑
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //设置表格不可被选中
    m_tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    //隐藏行号，行、列滚动条、设置tableWidget不可滚动
    m_tableWidget->verticalHeader()->setHidden(true);
    m_tableWidget->horizontalScrollBar()->setHidden(true);
    m_tableWidget->verticalScrollBar()->setHidden(true);
    //设置表格具体行、列的宽度及高度
    m_tableWidget->setColumnWidth(0,(m_width - 100)/9);
    m_tableWidget->setColumnWidth(1,(m_width - 100)/9);
    m_tableWidget->setColumnWidth(2,(m_width - 100)/9 + 10);
    m_tableWidget->setColumnWidth(3,(m_width - 100)/9);
    m_tableWidget->setColumnWidth(4,(m_width - 100)/9);
    m_tableWidget->setColumnWidth(5,(m_width - 100)/9 + 10);
    m_tableWidget->setColumnWidth(6,(m_width - 100)/9);
    m_tableWidget->setColumnWidth(7,(m_width - 100)/9);
    m_tableWidget->setColumnWidth(8,(m_width - 100)/9 + 10);

//    m_pointInfoVec = PlcDisplayVec;
    m_pointInfoVec = PlcAlarmVec;
    for(int i = 0;i < PlcStateVec.size();i++)
    {
        m_pointInfoVec.push_back(PlcStateVec[i]);
    }
    UpdatePlcPoint();

    m_returnButton = new QPushButton(tr("返回"));
    m_returnButton->setFixedWidth(m_width/8);
    m_returnLayout = new QHBoxLayout();
    m_returnLayout->setAlignment(Qt::AlignRight);
    m_returnLayout->addWidget(m_returnButton);

    m_pageVBoxLayout = new QVBoxLayout();
    m_pageVBoxLayout->addLayout(m_pageHBoxLayout);
    m_pageVBoxLayout->addWidget(m_tableWidget);
    m_pageVBoxLayout->addLayout(m_returnLayout);
    m_pageGroupBox = new QGroupBox();
    m_pageGroupBox->setLayout(m_pageVBoxLayout);
    m_vBoxLayout = new QVBoxLayout();
    m_vBoxLayout->addWidget(m_titleGroup);
    m_vBoxLayout->addWidget(m_pageGroupBox);

    this->setLayout(m_vBoxLayout);
    this->setPalette(palette);

    connect(m_returnButton,SIGNAL(clicked(bool)),this,SLOT(ReturnSlots()));

    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(UpdatePlcState()));
}

PlcState::~PlcState()
{     
    delete m_tableGroupBox;
    delete m_tableHBoxLayout;
    delete m_PointNameItem;
    delete m_RegisterAddrItem;
    delete m_titleLabel;
    delete m_titleGroup;
    delete m_previousPage;
    delete m_nextPage;
    delete m_pageMessage;
    delete m_currentpageLabel;
    delete m_totalpageLabel;
    delete m_spiltLabel;
    delete m_pageLabel;
    delete m_pageGroupBox;
    delete m_pageVBoxLayout;
    delete m_pageLayout;
    delete m_pageHBoxLayout;
    delete m_tableWidget;
    delete m_returnButton;
    delete m_returnLayout;
    delete m_vBoxLayout;
}

void PlcState::UpdatePlcPoint()
{
    DeleteTableWidgetItem();

    //设置列名
    QStringList list;
    list<<"项目名称"<<"寄存器地址"<<"状态"<<"项目名称"<<"寄存器地址"<<"状态"<<"项目名称"<<"寄存器地址"<<"状态";
    m_tableWidget->setHorizontalHeaderLabels(list);
    if(!m_pointInfoVec.empty())
    {
        UpdatePageMessage();
        for(int i = 0;i < m_pointInfoVec.size();i++)
        {
            if(i > m_pointInfoVec.size() - 1)
                break;
            StateWidgetParts* part = new StateWidgetParts();
            m_pointInfoVec[i].m_pointNum = i;
            m_partsInfoVec.push_back(part);
            QTableWidgetItem *item1 = new QTableWidgetItem(QString::fromStdString(m_pointInfoVec[i].m_pointName));
            item1->setTextAlignment(Qt::AlignCenter);
            m_tableItem.push_back(item1);
            QTableWidgetItem *item2 = new QTableWidgetItem(QString::fromStdString(m_pointInfoVec[i].m_registerType + IntToString(m_pointInfoVec[i].m_registerAddr)));
            item2->setTextAlignment(Qt::AlignCenter);
            m_tableItem.push_back(item2);
            //显示项目名称列的内容        --- 第1列
            m_tableWidget->setItem(i / 3, i % 3 * 3, item1);
            //显示寄存器地址列的内容       --- 第2列
            m_tableWidget->setItem(i / 3, i % 3 * 3 + 1, item2);
            //显示状态列                 --- 第3列
            m_tableWidget->setCellWidget(i / 3, i % 3 * 3 + 2, part->m_label);
            m_tableWidget->setRowHidden(i / 3,true);
        }
    }
}

void PlcState::UpdatePlcState()
{
    vector<PlcValue> pv;
    GetPlcValueVec(pv);
    vector<int> dalarmv;
    vector<int> dstatev;

    int dalarmi = -1;
    int dstatei = -1;
    //批量读取寄存器的值，不同寄存器类型的值存入相应类型下的数组
    if(!pv.empty())
    {
        for(int i = 0;i < pv.size();i++)
        {
            if(pv[i].registerType == "M")
            {
                if(pv[i].itemType == 4)
                {
                    dalarmv = pv[i].values;
                    dalarmi = i;
                }
                else if(pv[i].itemType == 1)
                {
                    dstatev = pv[i].values;
                    dstatei = i;
                }
            }
        }
    }
    else
    {
        return;
    }
    int currentPageStart = (m_currentPageIndex - 1) * m_currentPageItems;
    int currentPageEnd = currentPageStart + m_currentPageItems;
    //将从PLC中读取到的值回显至界面
    for(int i = currentPageStart;i < currentPageEnd;i++)
    {
        int index = 0;
        int value = -1;
        if((i >= m_pointInfoVec.size()) || (i >= m_partsInfoVec.size()))
            break;
        if(m_pointInfoVec[i].m_pointType == 4)
        {
            if(dalarmi == -1)
            {
                break;
            }
            index = m_pointInfoVec[i].m_registerAddr - pv[dalarmi].startAddr;
            if(index < dalarmv.size())
            {
                value = dalarmv[index];
            }
        }
        else if(m_pointInfoVec[i].m_pointType == 1)
        {
            if(dstatei == -1)
            {
                break;
            }
            index = m_pointInfoVec[i].m_registerAddr - pv[dstatei].startAddr;
            if(index < dstatev.size())
            {
                value = dstatev[index];
            }
        }
        if(value != -1)
        {
            if(m_partsInfoVec[i]->m_lastState != value)
            {
                m_partsInfoVec[i]->ChangeStateSlot((value == 0)?false:true);
            }
        }
    }
}

void PlcState::DeleteTableWidgetItem()
{
    if(!m_partsInfoVec.empty())
    {
        for(int i = 0;i < m_partsInfoVec.size();i++)
        {
            StateWidgetParts* part = m_partsInfoVec[i];
            if(part != NULL)
                part->~StateWidgetParts();
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

void PlcState::ReturnSlots()
{
    m_timer->stop();
    SetReadPlcFlag(false);
    SetDisplayType(0);
    this->close();
    m_parent->showPlcDisplay();
}


