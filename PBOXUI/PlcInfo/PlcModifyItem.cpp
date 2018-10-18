#include "PlcModifyItem.h"
#include <qobject.h>

extern vector<PlcPointInfo> PlcWholeInfoVec;

/**
 * @brief PLC修改项目界面中表格每一行所包含的部件
 */
ModifyItemWidgetParts::ModifyItemWidgetParts()
{
    QFont font;
    font.setPointSize(10);

    m_checkFlag = false;
    m_unselectIcon = QIcon("./Image/unselect.jpg");
    m_selectedIcon = QIcon("./Image/selected.jpg");

    m_pointNum = new QLabel();
    m_pointNum->setFont(font);
    m_pointCheckBox = new QPushButton();
    m_pointCheckBox->setIconSize(QSize(24,24));
    m_pointCheckBox->setFocusPolicy(Qt::NoFocus);
    m_pointCheckBox->setStyleSheet("QPushButton{background-color:transparent}");
    m_pointCheckBox->setFixedSize(50,30);
    m_pointCheckBox->setIcon(m_unselectIcon);
    connect(m_pointCheckBox,SIGNAL(clicked()),this,SLOT(ChangeState()));
    m_checkBoxLayout = new QHBoxLayout();
    m_checkBoxLayout->setMargin(0);
    m_checkBoxLayout->addStretch();
    m_checkBoxLayout->addWidget(m_pointNum);
    m_checkBoxLayout->addSpacing(10);
    m_checkBoxLayout->addWidget(m_pointCheckBox);
    m_checkBoxLayout->addStretch();
    m_checkBoxGroup = new QGroupBox();
    m_checkBoxGroup->setStyleSheet("QGroupBox{border-width:0;border-style:outset}");
    m_checkBoxGroup->setLayout(m_checkBoxLayout);

    m_pointNameEdit = new QLineEdit();
    connect(m_pointNameEdit,SIGNAL(textChanged(QString)),this,SIGNAL(isModified(QString)));
    m_pointNameEdit->setStyleSheet("QLineEdit{border-width:0;border-style:outset}");
    m_pointNameEdit->setAlignment(Qt::AlignCenter);
    m_nameLayout = new QHBoxLayout();
    m_nameLayout->addStretch();
    m_nameLayout->addWidget(m_pointNameEdit);
    m_nameLayout->addStretch();
    m_nameWidget = new QWidget();
    m_nameWidget->setLayout(m_nameLayout);

    m_pointTypeEdit = new QComboBox();
    connect(m_pointTypeEdit,SIGNAL(currentIndexChanged(QString)),this,SIGNAL(isModified(QString)));
    m_pointTypeEdit->setFixedWidth(160);
    m_pointTypeEdit->addItem("显示");
    m_pointTypeEdit->addItem("机械动作");
    m_pointTypeEdit->addItem("参数");
    m_pointTypeEdit->addItem("报警");
    m_pTypeLayout = new QHBoxLayout();
    m_pTypeWidget = new QWidget();
    m_pTypeLayout->addStretch();
    m_pTypeLayout->addWidget(m_pointTypeEdit);
    m_pTypeLayout->addStretch();
    m_pTypeLayout->setMargin(0);
    m_pTypeWidget->setLayout(m_pTypeLayout);

    m_registerTypeEdit = new QComboBox();
    connect(m_registerTypeEdit,SIGNAL(currentIndexChanged(QString)),this,SIGNAL(isModified(QString)));
    m_registerTypeEdit->setFixedWidth(120);
    m_rTypeLayout = new QHBoxLayout();
    m_rTypeWidget = new QWidget();
    m_rTypeLayout->addStretch();
    m_rTypeLayout->addWidget(m_registerTypeEdit);
    m_rTypeLayout->addStretch();
    m_rTypeLayout->setMargin(0);
    m_rTypeWidget->setLayout(m_rTypeLayout);

    m_registerAddrEdit = new QLineEdit();
    connect(m_registerAddrEdit,SIGNAL(textChanged(QString)),this,SIGNAL(isModified(QString)));
    m_registerAddrEdit->setStyleSheet("QLineEdit{border-width:0;border-style:outset}");
    m_registerAddrEdit->setAlignment(Qt::AlignCenter);
    m_rAddrLayout = new QHBoxLayout();
    m_rAddrWidget = new QWidget();
    m_rAddrLayout->addStretch();
    m_rAddrLayout->addWidget(m_registerAddrEdit);
    m_rAddrLayout->addStretch();
    m_rAddrWidget->setLayout(m_rAddrLayout);

    m_pointCoefficientEdit = new QLineEdit();
    connect(m_pointCoefficientEdit,SIGNAL(textChanged(QString)),this,SIGNAL(isModified(QString)));
    m_pointCoefficientEdit->setStyleSheet("QLineEdit{border-width:0;border-style:outset}");
    m_pointCoefficientEdit->setAlignment(Qt::AlignCenter);
    m_pointCoefficientLayout = new QHBoxLayout();
    m_pointCoefficientWidget = new QWidget();
    m_pointCoefficientLayout->addStretch();
    m_pointCoefficientLayout->addWidget(m_pointCoefficientEdit);
    m_pointCoefficientLayout->addStretch();
    m_pointCoefficientWidget->setLayout(m_pointCoefficientLayout);

    m_pointUnitEdit = new QLineEdit();
    connect(m_pointUnitEdit,SIGNAL(textChanged(QString)),this,SIGNAL(isModified(QString)));
    m_pointUnitEdit->setStyleSheet("QLineEdit{border-width:0;border-style:outset}");
    m_pointUnitEdit->setAlignment(Qt::AlignCenter);
    m_pointUnitLayout = new QHBoxLayout();
    m_pointUnitWidget = new QWidget();
    m_pointUnitLayout->addStretch();
    m_pointUnitLayout->addWidget(m_pointUnitEdit);
    m_pointUnitLayout->addStretch();
    m_pointUnitWidget->setLayout(m_pointUnitLayout);

}

/**
 * @brief 析构函数
 */
ModifyItemWidgetParts::~ModifyItemWidgetParts()
{
    delete m_pointNum;
    delete m_pointCheckBox;
    delete m_checkBoxLayout;
    delete m_checkBoxGroup;
    delete m_pointNameEdit;
    delete m_nameLayout;
    delete m_nameWidget;
    delete m_pointTypeEdit;
    delete m_pTypeLayout;
    delete m_pTypeWidget;
    delete m_registerTypeEdit;
    delete m_rTypeLayout;
    delete m_rTypeWidget;
    delete m_registerAddrEdit;
    delete m_rAddrLayout;
    delete m_rAddrWidget;
    delete m_pointUnitEdit;
    delete m_pointUnitLayout;
    delete m_pointUnitWidget;
    delete m_pointCoefficientEdit;
    delete m_pointCoefficientLayout;
    delete m_pointCoefficientWidget;
}

/**
 * @brief 自定义勾选框状态变化槽函数
 */
void ModifyItemWidgetParts::ChangeState()
{
    m_checkFlag = !m_checkFlag;
    if(m_checkFlag)
    {
        m_pointCheckBox->setIcon(m_selectedIcon);
    }
    else
    {
        m_pointCheckBox->setIcon(m_unselectIcon);
    }
}

/**
 * @brief 判断当前条目是否被选中
 * @return
 */
bool ModifyItemWidgetParts::isChecked()
{
    return m_checkFlag;
}

/**
 * @brief 修改PLC项目信息界面
 */
PlcModifyItem::PlcModifyItem(mainWidget *parent) : m_parent(parent)
{
    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();

    m_pageModifyFlag = false;
    m_wholeModifyFlag = false;

    QPalette palette;
    //设置字体颜色
    palette.setColor(QPalette::WindowText,Qt::white);
    //加载背景图片
    palette.setBrush(QPalette::Background, QBrush(QPixmap("./Image/backgroud1.jpg").scaled(QApplication::desktop()->size())));
    //设置字体大小
    QFont font;
    font.setPointSize(20);

    //构造标题栏
    m_titleLabel = new QLabel(tr("修改项目信息"));
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
    m_tableWidget->setColumnCount(7);
    m_tableWidget->setRowCount(1024);
    m_currentPageItems = 10;
    m_currentPageIndex = 1;
    m_tableWidget->setFixedWidth(width - 50);
    m_tableWidget->setFocusPolicy(Qt::NoFocus);
    //设置表格不可编辑
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //设置表格不可被选中
    m_tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    //设置默认行高
    m_tableWidget->verticalHeader()->setDefaultSectionSize(height / 15);
    //隐藏行号，行、列滚动条、设置tableWidget不可滚动
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_tableWidget->verticalHeader()->setHidden(true);
    m_tableWidget->horizontalScrollBar()->setHidden(true);
    m_tableWidget->verticalScrollBar()->setHidden(true);
    //设置表格具体行、列的宽度及高度
    m_tableWidget->horizontalHeader()->setFixedHeight(35);
    m_tableWidget->setColumnWidth(0,(width - 10)/10);
    m_tableWidget->setColumnWidth(1,(width - 10)/5);
    m_tableWidget->setColumnWidth(2,(width - 10)/4);
    m_tableWidget->setColumnWidth(3,(width - 10)/6);
    m_tableWidget->setColumnWidth(4,(width - 10)/12 - 5);
    m_tableWidget->setColumnWidth(5,(width - 10)/12 - 5);
    m_tableWidget->setColumnWidth(6,(width - 10)/12 - 5);

    //构造添加、删除、确认、返回按钮栏
    m_addButton = new QPushButton(tr("添 加"));
    m_addButton->setFixedSize(130,38);
    connect(m_addButton,SIGNAL(clicked()),this,SLOT(AddPointInfo()));
    m_deleteButton = new QPushButton(tr("删 除"));
    m_deleteButton->setFixedSize(130,38);
    connect(m_deleteButton,SIGNAL(clicked()),this,SLOT(DeletePointInfo()));
    m_confirmButton = new QPushButton(tr("确 认"));
    m_confirmButton->setFixedSize(130,38);
    connect(m_confirmButton,SIGNAL(clicked()),this,SLOT(ConfirmSlots()));
    m_returnButton = new QPushButton(tr("返 回"));
    m_returnButton->setFixedSize(130,38);
    connect(m_returnButton,SIGNAL(clicked()),this,SLOT(ReturnSlots()));
    m_returnLayout = new QHBoxLayout();
    m_returnLayout->addWidget(m_addButton);
    m_returnLayout->addStretch();
    m_returnLayout->addWidget(m_deleteButton);
    m_returnLayout->addStretch();
    m_returnLayout->addWidget(m_confirmButton);
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

    m_pointInfoVec = PlcWholeInfoVec;
    m_tableRowCount = m_pointInfoVec.size();
    UpdatePlcPoint();
    SortPlcPoint();

    m_addDialog = new PlcAddItemDialog();

    connect(m_addDialog,SIGNAL(SubmitNewPlcPoint(PlcPointInfo&)),this,SLOT(ReceivePointInfo(PlcPointInfo&)));

    this->setPalette(palette);
    this->setLayout(m_wholeLayout);
}

/**
 * @brief PLC修改界面析构函数
 */
PlcModifyItem::~PlcModifyItem()
{
    delete m_addButton;
    delete m_deleteButton;
    delete m_confirmButton;
    if(m_addDialog != NULL)
        delete m_addDialog;
}

/**
 * @brief 更新表格中显示的PLC项目信息
 */
void PlcModifyItem::UpdatePlcPoint()
{
    DeleteTableWidgetItem();

    //设置列名
    QStringList list;
    list.append("项目序号");
    list.append("项目名称");
    list.append("项目类型");
    list.append("寄存器类型");
    list.append("寄存器地址");
    list.append("项目系数");
    list.append("项目单位");

    m_tableWidget->setHorizontalHeaderLabels(list);

    if(!m_pointInfoVec.empty())
    {
        UpdatePageMessage();

        for(int i = 0;i < m_pointInfoVec.size();i++)
        {
            if(i > m_pointInfoVec.size() - 1)
                break;
            ModifyItemWidgetParts* part = new ModifyItemWidgetParts();
            connect(part,SIGNAL(isModified(QString)),this,SLOT(isModified(QString)));
            m_partsInfoVec.push_back(part);
            m_pointInfoVec[i].m_pointNum = i;
            part->m_pointNum->setText(QString::fromStdString(IntToString(i)));
            part->m_pointNameEdit->setText(QString::fromStdString(m_pointInfoVec[i].m_pointName));
            part->m_pointTypeEdit->setCurrentIndex(m_pointInfoVec[i].m_pointType - 1);
            part->m_registerTypeEdit->addItem(QString::fromStdString(m_pointInfoVec[i].m_registerType));
            if(part->m_registerTypeEdit->currentText() != "D")
                part->m_registerTypeEdit->addItem("D");
            if(part->m_registerTypeEdit->currentText() != "M")
                part->m_registerTypeEdit->addItem("M");
            part->m_registerAddrEdit->setText(QString::fromStdString(IntToString(m_pointInfoVec[i].m_registerAddr)));
            if(m_pointInfoVec[i].m_pointCoefficient != 0)
                part->m_pointCoefficientEdit->setText(QString::fromStdString(DoubleToString(m_pointInfoVec[i].m_pointCoefficient,"%.2f")));
            part->m_pointUnitEdit->setText(QString::fromStdString(m_pointInfoVec[i].m_pointUnit));
            //显示项目序号列的内容        --- 第1列
            m_tableWidget->setCellWidget(i, 0, part->m_checkBoxGroup);
            //显示项目名称列的内容        --- 第2列
            m_tableWidget->setCellWidget(i, 1, part->m_nameWidget);
            //显示项目类型列的内容        ---第3列
            m_tableWidget->setCellWidget(i, 2, part->m_pTypeWidget);
            //显示寄存器类型列的内容      --- 第4列
            m_tableWidget->setCellWidget(i, 3, part->m_rTypeWidget);
            //显示寄存器地址列的内容      --- 第5列
            m_tableWidget->setCellWidget(i, 4, part->m_rAddrWidget);
            //显示项目对应系数           --- 第6列
            m_tableWidget->setCellWidget(i, 5, part->m_pointCoefficientWidget);
            //显示项目对应单位           --- 第7列
            m_tableWidget->setCellWidget(i, 6, part->m_pointUnitWidget);
        }
    }
}

/**
 * @brief 释放表格中动态申请的资源
 */
void PlcModifyItem::DeleteTableWidgetItem()
{
    if(!m_partsInfoVec.empty())
    {
        for(int i = 0;i < m_partsInfoVec.size();i++)
        {
            ModifyItemWidgetParts* part = m_partsInfoVec[i];
            if(part != NULL)
            {
                delete part;
            }
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

/**
 * @brief 添加按钮点击槽函数
 */
void PlcModifyItem::AddPointInfo()
{
    m_addButton->setEnabled(false);
    m_addDialog->show();
    m_addDialog->activateWindow();
    m_addButton->setEnabled(true);
}

/**
 * @brief 删除按钮点击槽函数
 */
void PlcModifyItem::DeletePointInfo()
{
    m_deleteButton->setEnabled(false);
    if(!m_partsInfoVec.empty())
    {
        for(vector<ModifyItemWidgetParts*>::iterator iv = m_partsInfoVec.begin();iv != m_partsInfoVec.end();)
        {
            int offset = 0;
            if((*iv)->isChecked())
            {
                m_wholeModifyFlag = true;
                int num = (*iv)->m_pointNum->text().toInt();
                if(!m_deleteNumVec.empty())
                {
                    for(int i = 0;i < m_deleteNumVec.size();i++)
                    {
                        if(m_deleteNumVec[i] < num)
                            offset += 1;
                    }
                }
                m_tableWidget->removeRow(num - offset);
                m_deleteNumVec.push_back(num);
                delete (*iv);
                iv = m_partsInfoVec.erase(iv);
                for(vector<PlcPointInfo>::iterator it = m_pointInfoVec.begin();it != m_pointInfoVec.end();it++)
                {
                    if((*it).m_pointNum == num)
                    {
                        m_pointInfoVec.erase(it);
                        break;
                    }
                }
                if(iv == m_partsInfoVec.end())
                    break;
                continue;
            }
            iv++;
        }
    }
    UpdateDisplayItem(m_currentPageIndex);
    m_deleteButton->setEnabled(true);
}

/**
 * @brief 接收一条新添加的PLC项目信息
 * @param info
 */
void PlcModifyItem::ReceivePointInfo(PlcPointInfo& info)
{
    int num = m_pointInfoVec.size();
    info.m_pointNum = num;
    m_pointInfoVec.push_back(info);
    m_tableRowCount = m_pointInfoVec.size();

    ModifyItemWidgetParts* part = new ModifyItemWidgetParts();
    connect(part,SIGNAL(isModified(QString)),this,SLOT(isModified(QString)));
    m_partsInfoVec.push_back(part);
    part->m_pointNum->setText(QString::fromStdString(IntToString(info.m_pointNum)));
    part->m_pointNameEdit->setText(QString::fromStdString(info.m_pointName));
    part->m_pointTypeEdit->setCurrentIndex(info.m_pointType - 1);
    part->m_registerTypeEdit->addItem(QString::fromStdString(info.m_registerType));
    if(part->m_registerTypeEdit->currentText() != "D")
        part->m_registerTypeEdit->addItem("D");
    if(part->m_registerTypeEdit->currentText() != "M")
        part->m_registerTypeEdit->addItem("M");
    part->m_registerAddrEdit->setText(QString::fromStdString(IntToString(info.m_registerAddr)));
    if(info.m_pointCoefficient != 0)
        part->m_pointCoefficientEdit->setText(QString::fromStdString(DoubleToString(info.m_pointCoefficient,"%.2f")));
    part->m_pointUnitEdit->setText(QString::fromStdString(info.m_pointUnit));
    //显示项目序号列的内容        --- 第1列
    m_tableWidget->setCellWidget(m_tableRowCount - 1, 0, part->m_checkBoxGroup);
    //显示项目名称列的内容        --- 第2列
    m_tableWidget->setCellWidget(m_tableRowCount - 1, 1, part->m_nameWidget);
    //显示项目类型列的内容        ---第3列
    m_tableWidget->setCellWidget(m_tableRowCount - 1, 2, part->m_pTypeWidget);
    //显示寄存器类型列的内容      --- 第4列
    m_tableWidget->setCellWidget(m_tableRowCount - 1, 3, part->m_rTypeWidget);
    //显示寄存器地址列的内容      --- 第5列
    m_tableWidget->setCellWidget(m_tableRowCount - 1, 4, part->m_rAddrWidget);
    //显示项目对应系数           --- 第6列
    m_tableWidget->setCellWidget(m_tableRowCount - 1, 5, part->m_pointCoefficientWidget);
    //显示项目对应单位           --- 第7列
    m_tableWidget->setCellWidget(m_tableRowCount - 1, 6, part->m_pointUnitWidget);

    m_wholeModifyFlag = true;
//    UpdatePlcPoint();
    UpdateDisplayItem(m_wholePageNum);
}

/**
 * @brief 确认按钮点击槽函数
 */
void PlcModifyItem::ConfirmSlots()
{
    m_confirmButton->setEnabled(false);
    if(m_pageModifyFlag)
    {
        m_wholeModifyFlag = true;
        m_pageModifyFlag = false;
        int pNum = (m_currentPageIndex - 1) * m_currentPageItems;
        for(int i = pNum;i < pNum + m_currentPageItems;i++)
        {
            if(i < m_pointInfoVec.size())
            {
                PlcPointInfo& pi = m_pointInfoVec[i];
                ModifyItemWidgetParts* mi = m_partsInfoVec[i];
                pi.m_pointName = mi->m_pointNameEdit->text().toStdString();
                pi.m_pointType = mi->m_pointTypeEdit->currentIndex() + 1;
                pi.m_registerType = mi->m_registerTypeEdit->currentText().toStdString();
                pi.m_registerAddr = mi->m_registerAddrEdit->text().toInt();
                if(!mi->m_pointCoefficientEdit->text().isEmpty())
                    pi.m_pointCoefficient = mi->m_pointCoefficientEdit->text().toDouble();
                pi.m_pointUnit = mi->m_pointUnitEdit->text().toStdString();
            }
        }
    }
    m_confirmButton->setEnabled(true);
}

/**
 * @brief 返回按钮点击槽函数
 */
void PlcModifyItem::ReturnSlots()
{
    if(m_wholeModifyFlag)
    {
        QMessageBox m_message;
        QPushButton *ok = m_message.addButton(tr("      不保存     "),QMessageBox::ActionRole);
        QPushButton *cancel = m_message.addButton(tr(" 保存且立即重启 "),QMessageBox::ActionRole);
        m_message.setText(" PLC点位信息已修改，需要重启才能生效，是否返回且不保存当前修改数据?");
        m_message.move(400,280);
        m_message.setIcon(QMessageBox::Question);
        m_message.exec();
        if(m_message.clickedButton() == ok)
        {
            m_wholeModifyFlag = false;
            this->close();
            m_addDialog->close();
            m_parent->showPlcDisplay();
        }
        else if(m_message.clickedButton() == cancel)
        {
            m_message.close();
            ParseCsv csvHelper;
            csvHelper.WriteToCsvFile(m_pointInfoVec, "./PlcPointInfo.csv");
            string command = "sync";
            system(command.data());
            command = "reboot";
            system(command.data());
        }
    }
    else
    {
        this->close();
        m_parent->showPlcDisplay();
    }
}

void PlcModifyItem::isModified(QString str)
{
    m_pageModifyFlag = true;
}

/**
 * @brief 翻到下一页槽函数
 */
void PlcModifyItem::GoToNextPage()
{
    if(m_pageModifyFlag)
    {
        QMessageBox m_message;
        QPushButton *ok = m_message.addButton(tr("  是  "),QMessageBox::ActionRole);
        QPushButton *cancel = m_message.addButton(tr("  否  "),QMessageBox::ActionRole);
        m_message.setText("  当 前 页 有 修 改 操 作，是 否 前 往 下 一 页 且 不 保 存 ?  ");
        m_message.move(400,280);
        m_message.setIcon(QMessageBox::Question);
        m_message.exec();
        if(m_message.clickedButton() == ok)
        {
            m_pageModifyFlag = false;
            if(m_currentPageIndex < m_wholePageNum)
            {
                UpdateDisplayItem(++m_currentPageIndex);
            }
        }
        else if(m_message.clickedButton() == cancel)
        {
            m_message.close();
        }
    }
    else
    {
        if(m_currentPageIndex < m_wholePageNum)
        {
            UpdateDisplayItem(++m_currentPageIndex);
        }
    }
}

/**
 * @brief 翻到上一页槽函数
 */
void PlcModifyItem::GoToPreviousPage()
{
    if(m_pageModifyFlag)
    {
        QMessageBox m_message;
        QPushButton *ok = m_message.addButton(tr("  是  "),QMessageBox::ActionRole);
        QPushButton *cancel = m_message.addButton(tr("  否  "),QMessageBox::ActionRole);
        m_message.setText("  当 前 页 有 修 改 操 作，是 否 前 往 上 一 页 且 不 保 存 ?  ");
        m_message.move(400,280);
        m_message.setIcon(QMessageBox::Question);
        m_message.exec();
        if(m_message.clickedButton() == ok)
        {
            if(m_currentPageIndex > 1)
            {
                m_pageModifyFlag = false;
                UpdateDisplayItem(--m_currentPageIndex);
            }
        }
        else if(m_message.clickedButton() == cancel)
        {
            m_message.close();
        }
    }
    else
    {
        if(m_currentPageIndex > 1)
        {
            UpdateDisplayItem(--m_currentPageIndex);
        }
    }
}


/**
 * @brief 对PLC项目进行排序（升序）
 */
void PlcModifyItem::SortPlcPoint()
{
    vector<int> piD_D,piD_C,piD_S,piM_D,piM_C,piM_S,piM_A,piX_D,piX_C,piX_S,piY_D,piY_C,piY_S;
    //将不同寄存器类型的项目进行分组
    if(!m_pointInfoVec.empty())
    {
        for(int i = 0;i < m_pointInfoVec.size();i++)
        {
            if(m_pointInfoVec[i].m_registerType == "D")
            {
                switch(m_pointInfoVec[i].m_pointType)
                {
                case 1:piD_D.push_back(m_pointInfoVec[i].m_registerAddr);break;
                case 2:piD_C.push_back(m_pointInfoVec[i].m_registerAddr);break;
                case 3:piD_S.push_back(m_pointInfoVec[i].m_registerAddr);break;
                }
            }
            else if(m_pointInfoVec[i].m_registerType == "M")
            {
                switch(m_pointInfoVec[i].m_pointType)
                {
                case 1:piM_D.push_back(m_pointInfoVec[i].m_registerAddr);break;
                case 2:piM_C.push_back(m_pointInfoVec[i].m_registerAddr);break;
                case 3:piM_S.push_back(m_pointInfoVec[i].m_registerAddr);break;
                case 4:piM_A.push_back(m_pointInfoVec[i].m_registerAddr);break;
                }
            }
            else if(m_pointInfoVec[i].m_registerType == "X")
            {
                switch(m_pointInfoVec[i].m_pointType)
                {
                case 1:piX_D.push_back(m_pointInfoVec[i].m_registerAddr);break;
                case 2:piX_C.push_back(m_pointInfoVec[i].m_registerAddr);break;
                case 3:piX_S.push_back(m_pointInfoVec[i].m_registerAddr);break;
                }
            }
            else if(m_pointInfoVec[i].m_registerType == "Y")
            {
                switch(m_pointInfoVec[i].m_pointType)
                {
                case 1:piY_D.push_back(m_pointInfoVec[i].m_registerAddr);break;
                case 2:piY_C.push_back(m_pointInfoVec[i].m_registerAddr);break;
                case 3:piY_S.push_back(m_pointInfoVec[i].m_registerAddr);break;
                }
            }
        }
    }
    vector<PlcValue> pvVec;
    PlcValue pv = GetPvFromVec(piD_D,1,"D");
    pvVec.push_back(pv);
    pv = GetPvFromVec(piD_C,2,"D");
    pvVec.push_back(pv);
    pv = GetPvFromVec(piD_S,3,"D");
    pvVec.push_back(pv);
    pv = GetPvFromVec(piM_D,1,"M");
    pvVec.push_back(pv);
    pv = GetPvFromVec(piM_C,2,"M");
    pvVec.push_back(pv);
    pv = GetPvFromVec(piM_S,3,"M");
    pvVec.push_back(pv);
    pv = GetPvFromVec(piM_A,4,"M");
    pvVec.push_back(pv);
    pv = GetPvFromVec(piX_D,1,"X");
    pvVec.push_back(pv);
    pv = GetPvFromVec(piX_C,2,"X");
    pvVec.push_back(pv);
    pv = GetPvFromVec(piX_S,3,"X");
    pvVec.push_back(pv);
    pv = GetPvFromVec(piY_D,1,"Y");
    pvVec.push_back(pv);
    pv = GetPvFromVec(piY_C,2,"Y");
    pvVec.push_back(pv);
    pv = GetPvFromVec(piY_S,3,"Y");
    pvVec.push_back(pv);

    SetPlcValueVec(pvVec);

}

/**
 * @brief 对每个项目信息组进行排序,然后将每种不同寄存器类型的起始地址、长度保存
 * @param vec
 * @param itemType
 * @param registerType
 * @return
 */
PlcValue PlcModifyItem::GetPvFromVec(vector<int> vec,int itemType,string registerType)
{
    PlcValue pv;
    if(!vec.empty())
    {
        sort(vec.begin(),vec.end());
        pv.itemType = itemType;
        pv.registerType = registerType;
        if(registerType == "D")
            pv.length = (vec[vec.size() - 1] - vec[0]) / 2 + 1;
        else
            pv.length = vec[vec.size() - 1] - vec[0] + 1;
        pv.startAddr = vec[0];
    }
    return pv;
}
