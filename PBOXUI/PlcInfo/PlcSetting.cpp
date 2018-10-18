#include "PlcSetting.h"

/**
 * @brief 自定义部件类构造函数
 */
ParameterWidgetParts::ParameterWidgetParts()
{
    int width = QApplication::desktop()->width(); //1280
    int height = QApplication::desktop()->height(); //720

    QFont font;
    font.setPointSize(15);
    QPalette palette;
    palette.setColor(QPalette::Text,Qt::black);
    m_parameterEdit = new QLineEdit();
    m_parameterEdit->setPalette(palette);
    m_parameterEdit->setStyleSheet("background-color: gray;");
    m_parameterEdit->setFixedSize(width / 6, height / 20);
    m_parameterEdit->setAlignment(Qt::AlignCenter);
    m_parameterEdit->setFont(font);
    m_editHLayout = new QHBoxLayout();
    m_editHLayout->addWidget(m_parameterEdit);

    m_unitLabel = new QLabel(/*QObject::tr("ms")*/);
    m_unitLabel->setFont(font);
    m_unitHLayout = new QHBoxLayout();
    m_unitHLayout->addWidget(m_unitLabel);

    m_parameterHLayout = new QHBoxLayout();
    m_parameterHLayout->addStretch();
    m_parameterHLayout->addLayout(m_editHLayout);
    m_parameterHLayout->addLayout(m_unitHLayout);
    m_parameterHLayout->addStretch();
    m_parameterVLayout = new QVBoxLayout();
    m_parameterVLayout->addStretch();
    m_parameterVLayout->addLayout(m_parameterHLayout);
    m_parameterVLayout->addStretch();
    m_parameterVLayout->setMargin(0);
    m_parameterBox = new QWidget();
    m_parameterBox->setLayout(m_parameterVLayout);

    m_confirmButton = new MyPushButton(QObject::tr("确认"));
    m_confirmButton->setFixedSize((width - 10)/8,38);
    m_buttonHLayout = new QHBoxLayout();
    //m_buttonHLayout->setAlignment(Qt::AlignCenter);
    m_buttonHLayout->setMargin(0);
    m_buttonHLayout->addSpacing(width / 100);
    m_buttonHLayout->addWidget(m_confirmButton);
    m_buttonHLayout->addStretch();
    m_buttonLayout = new QVBoxLayout();
    m_buttonLayout->setMargin(0);
    m_buttonHLayout->addStretch();
//    m_buttonLayout->setAlignment(Qt::AlignCenter);
    m_buttonLayout->addLayout(m_buttonHLayout);
    m_buttonHLayout->addStretch();
    m_buttonBox = new QGroupBox();
    m_buttonBox->setLayout(m_buttonLayout);
    m_buttonBox->setStyleSheet("QGroupBox{border-width:0;border-style:outset}");
}

/**
 * @brief 自定义部件类构造函数
 */
ParameterWidgetParts::~ParameterWidgetParts()
{
    delete m_parameterEdit;
    delete m_unitLabel;
    delete m_parameterHLayout;
    delete m_parameterVLayout;
    delete m_parameterBox;
    delete m_confirmButton;
}

/**
 * @brief PLC参数设置界面构造函数
 * @param parent
 */
PlcSetting::PlcSetting(mainWidget *parent) : m_parent(parent)
{

    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();

    m_widgetType = 3;

    QPalette palette;
    //设置字体颜色
    palette.setColor(QPalette::WindowText,Qt::white);
    //加载背景图片
    palette.setBrush(QPalette::Background, QBrush(QPixmap("./Image/backgroud1.jpg").scaled(QApplication::desktop()->size())));
    //设置字体大小
    QFont font;
    font.setPointSize(20);
    //构造标题栏
    m_titleLabel = new QLabel(tr("参数设置"));
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
    m_tableWidget->setColumnCount(5);
    m_tableWidget->setRowCount(100);
    m_currentPageItems = 10;
    m_currentPageIndex = 1;
    m_tableWidget->setFixedWidth(width - 50);
    m_tableWidget->setFocusPolicy(Qt::NoFocus);
    //设置表格不可编辑
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //设置表格不可被选中
    m_tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    //设置默认行高
    m_tableWidget->verticalHeader()->setDefaultSectionSize(height / 15 + 1);
    //隐藏行号，行、列滚动条、设置tableWidget不可滚动
//    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
//    m_tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_tableWidget->verticalHeader()->setHidden(true);
    m_tableWidget->horizontalScrollBar()->setHidden(true);
    m_tableWidget->verticalScrollBar()->setHidden(true);
    //设置表格具体行、列的宽度及高度
    m_tableWidget->horizontalHeader()->setFixedHeight(height/17);
    m_tableWidget->setColumnWidth(0,(width - 10)/12);
    m_tableWidget->setColumnWidth(1,(width - 10)/4);
    m_tableWidget->setColumnWidth(2,(width - 10)/7);
    m_tableWidget->setColumnWidth(3,(width - 10)/3);
    m_tableWidget->setColumnWidth(4,(width - 10)/8 + 25);
    //隐藏所有的行
    for(int i = 0;i < 100;i++)
        m_tableWidget->setRowHidden(i,true);

    m_modifyButton = new QPushButton(tr("修改"));
    m_modifyButton->setFixedSize(130,38);
    connect(m_modifyButton,SIGNAL(clicked()),this,SLOT(ModifySlots()));
    //构造返回按钮栏
    m_returnButton = new QPushButton(tr("返回"));
    m_returnButton->setFixedSize(130,38);
    connect(m_returnButton,SIGNAL(clicked()),this,SLOT(ReturnSlots()));
    m_returnLayout = new QHBoxLayout();
    m_returnLayout->addSpacing(20);
    m_returnLayout->addWidget(m_modifyButton);
    m_returnLayout->addStretch();
    m_returnLayout->addWidget(m_returnButton);
    m_returnLayout->addSpacing(20);

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

    m_pointInfoVec = PlcSettingVec;
    for(int i = 0;i < PlcDDisplayVec.size();i++)
    {
        m_pointInfoVec.push_back(PlcDDisplayVec[i]);
    }
    UpdatePlcPoint();

    m_timer = new QTimer();
    connect(m_timer,SIGNAL(timeout()),this,SLOT(UpdateMachineValue()));

    this->setPalette(palette);
    this->setLayout(m_wholeLayout);
}

PlcSetting::~PlcSetting()
{}

/**
 * @brief 更新表格中显示的PLC项目信息
 */
void PlcSetting::UpdatePlcPoint()
{
    DeleteTableWidgetItem();

    //设置列名
    QStringList list;
    list.append("项目序号");
    list.append("项目名称");
    list.append("寄存器地址");
    list.append("默认值");
    list.append("控制");
    m_tableWidget->setHorizontalHeaderLabels(list);

    if(!m_pointInfoVec.empty())
    {
        UpdatePageMessage();

        for(int i = 0;i < m_pointInfoVec.size();i++)
        {
            if(i > m_pointInfoVec.size() - 1)
                break;
            PlcPointInfo& pi = m_pointInfoVec[i];
            string plcAddr = pi.m_registerType + IntToString(pi.m_registerAddr);
            pi.m_pointNum = i;
            QTableWidgetItem *item1 = new QTableWidgetItem(QString::fromStdString(IntToString(i + 1)));
            item1->setTextAlignment(Qt::AlignCenter);
            m_tableItem.push_back(item1);
            QTableWidgetItem *item2 = new QTableWidgetItem(QString::fromStdString(pi.m_pointName));
            item2->setTextAlignment(Qt::AlignCenter);
            m_tableItem.push_back(item2);
            QTableWidgetItem *item3 = new QTableWidgetItem(QString::fromStdString(plcAddr));
            item3->setTextAlignment(Qt::AlignCenter);
            m_tableItem.push_back(item3);
            //显示项目序号列的内容        --- 第1列
            m_tableWidget->setItem(i, 0, item1);
            //显示项目名称列的内容        --- 第2列
            m_tableWidget->setItem(i, 1, item2);
            //显示寄存器地址列的内容       --- 第3列
            m_tableWidget->setItem(i, 2, item3);

            ParameterWidgetParts* part = new ParameterWidgetParts();
            part->m_confirmButton->SetMyButtonNum(i);
            part->m_unitLabel->setText(QString::fromStdString(pi.m_pointUnit));
            connect(part->m_confirmButton,SIGNAL(clicked()),part->m_confirmButton,SLOT(EmitClickedByNum()));
            connect(part->m_confirmButton,SIGNAL(MyButtonClicked(int)),this,SLOT(PlcSetParameter(int)));
            m_partsInfoVec.push_back(part);
            part->m_parameterEdit->setEnabled(false);

            //显示参数列的输入框            --- 第4列
            m_tableWidget->setCellWidget(i, 3, part->m_parameterBox);
            //显示控制列的确认按钮
            m_tableWidget->setCellWidget(i, 4, part->m_buttonBox);
        }
    }
}

/**
 * @brief 释放表格中动态申请的资源
 */
void PlcSetting::DeleteTableWidgetItem()
{
    if(!m_partsInfoVec.empty())
    {
        for(int i = 0;i < m_partsInfoVec.size();i++)
        {
            ParameterWidgetParts* part = m_partsInfoVec[i];
            if(part != NULL)
                part->~ParameterWidgetParts();
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

void PlcSetting::UpdateMachineValue()
{
    vector<PlcValue> pv;
    GetPlcValueVec(pv);
    vector<int> dsettingv;
    vector<int> ddisplayv;
    int dsettingi = 0;
    int ddisplayi = 0;
    if(!pv.empty())
    {
        for(int i = 0;i < pv.size();i++)
        {
            if(pv[i].registerType == "D")
            {
                if(pv[i].itemType == 3)
                {
                    dsettingv = pv[i].values;
                    dsettingi = i;
                }
                else if(pv[i].itemType == 1)
                {
                    ddisplayv = pv[i].values;
                    ddisplayi = i;
                }
            }
        }
    }
    int currentPageStart = (m_currentPageIndex - 1) * m_currentPageItems;
    int currentPageEnd = currentPageStart + m_currentPageItems;
    //将从PLC中读取到的值回显至界面
    for(int i = currentPageStart;i < currentPageEnd;i++)
    {
        string value = "";
        if((i >= m_pointInfoVec.size()) || (i >= m_partsInfoVec.size()))
            break;
        //D区使用两个寄存器地址表示一个32位数据，索引值为地址差值除以2
        int index = 0;
        if(m_pointInfoVec[i].m_pointType == 3)
        {
            index = (m_pointInfoVec[i].m_registerAddr - pv[dsettingi].startAddr) / 2;
            if(index < dsettingv.size())
            {
                //由于PLC内部单位与界面显示单位可能存在差异，需要乘以系数
                if(dsettingv[index] != -1)
                    value = IntToString(dsettingv[index] * m_pointInfoVec[i].m_pointCoefficient);
            }
        }
        else if(m_pointInfoVec[i].m_pointType == 1)
        {
            index = (m_pointInfoVec[i].m_registerAddr - pv[ddisplayi].startAddr) / 2;
            if(index < ddisplayv.size())
            {
                //由于PLC内部单位与界面显示单位可能存在差异，需要乘以系数
                if(ddisplayv[index] != -1)
                    value = IntToString(ddisplayv[index] * m_pointInfoVec[i].m_pointCoefficient);
            }
        }

        if(!m_partsInfoVec[i]->m_parameterEdit->isEnabled())
        {
            m_partsInfoVec[i]->m_parameterEdit->setText(QString::fromStdString(value));
        }
    }
}

void PlcSetting::CheckPlcInfo()
{
    m_pointInfoVec = PlcSettingVec;
    UpdatePlcPoint();
}

/**
 * @brief 修改按钮槽函数
 */
void PlcSetting::ModifySlots()
{
    m_modifyButton->setEnabled(false);
    SetParameterEditEnable(true);
    m_modifyButton->setEnabled(true);
}

/**
 * @brief 设置参数输入框的使能
 * @param flag
 */
void PlcSetting::SetParameterEditEnable(bool flag)
{
    if(!m_partsInfoVec.empty())
    {
        for(int i = 0;i < m_partsInfoVec.size();i++)
        {
            if(i > m_pointInfoVec.size() - 1)
            {
                    break;
            }
            else
            {
                if(m_pointInfoVec[i].m_pointType == 3)
                {
                    m_partsInfoVec[i]->m_parameterEdit->setEnabled(flag);
                    if(flag)
                    {
                        m_partsInfoVec[i]->m_parameterEdit->setStyleSheet("background-color: white;");
                    }
                    else
                    {
                        m_partsInfoVec[i]->m_parameterEdit->setStyleSheet("background-color: gray;");
                    }
                }
            }
        }
    }
}

/**
 * @brief 返回按钮槽函数
 */
void PlcSetting::ReturnSlots()
{
    m_timer->stop();
    SetReadPlcFlag(false);
    SetDisplayType(0);
    this->close();
    m_parent->showPlcDisplay();
}

/**
 * @brief 将界面参数值写入PLC中
 * @param num 条目编号，通过条目编号确定部件上的值
 */
void PlcSetting::PlcSetParameter(int num)
{
    if(m_partsInfoVec[num]->m_parameterEdit->isEnabled())
    {
        vector<int> vec;
        //由于PLC内部单位为100ms，界面单位为ms，所以界面的值写入到PLC需要除以100
        vec.push_back(m_partsInfoVec[num]->m_parameterEdit->text().toInt() / m_pointInfoVec[num].m_pointCoefficient);
        bool res = m_device->WriteWord(m_pointInfoVec[num].m_registerAddr, vec);
        _log.LOG_DEBUG("PlcSetting 【WriteWord】 向PLC点位【%d】置位，值为【%d】，结果为【%d】",m_pointInfoVec[num].m_registerAddr,vec.at(0),res);
        SetParameterEditEnable(false);
    }
}

