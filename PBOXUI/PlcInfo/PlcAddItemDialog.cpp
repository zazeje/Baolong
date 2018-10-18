#include "PlcAddItemDialog.h"

/**
 * @brief Plc添加项目信息对话框
 */
PlcAddItemDialog::PlcAddItemDialog()
{
    QPalette palette;
    //设置字体颜色
    palette.setColor(QPalette::WindowText,Qt::white);
    //加载背景图片
    palette.setBrush(QPalette::Background, QBrush(QPixmap("./Image/backgroud1.jpg")));

    QFont ft;
    ft.setPointSize(18);
    m_pointNameLabel = new QLabel(tr("项目名称"));
    m_pointNameLabel->setFont(ft);
    m_pointNameEdit = new QLineEdit();
    m_pointNameEdit->setFocusPolicy(Qt::StrongFocus);
    m_pointNameEdit->setFixedSize(200,40);

    m_registerAddrLabel = new QLabel(tr("寄存器地址"));
    m_registerAddrLabel->setFont(ft);
    m_registerAddrEdit = new QLineEdit();
    m_registerAddrEdit->setFixedSize(200,40);
    m_registerAddrEdit->setFocusPolicy(Qt::StrongFocus);

    m_pointTypeLabel = new QLabel(tr("项目类型"));
    m_pointTypeLabel->setFont(ft);
    m_pointTypeEdit = new QComboBox();
    m_pointTypeEdit->setFont(ft);
    m_pointTypeEdit->setFixedSize(200,40);
    m_pointTypeEdit->setStyleSheet("QComboBox QAbstractItemView::item{height:40px;}");
    m_pointTypeEdit->addItem("显示");
    m_pointTypeEdit->addItem("机械动作");
    m_pointTypeEdit->addItem("参数");
    m_pointTypeEdit->addItem("报警");

    m_registerTypeLabel = new QLabel(tr("寄存器类型"));
    m_registerTypeLabel->setFont(ft);
    m_registerTypeEdit = new QComboBox();
    m_registerTypeEdit->setFont(ft);
    m_registerTypeEdit->setFixedSize(200,40);
    m_registerTypeEdit->setStyleSheet("QComboBox QAbstractItemView::item{height:40px;}");
    m_registerTypeEdit->addItem("D");
    m_registerTypeEdit->addItem("M");

    m_pointCoeffLabel = new QLabel(tr("项目系数"));
    m_pointCoeffLabel->setFont(ft);
    m_pointCoeffEdit = new QLineEdit();
    m_pointCoeffEdit->setFocusPolicy(Qt::StrongFocus);
    m_pointCoeffEdit->setFixedSize(200,40);

    m_pointUnitLabel = new QLabel(tr("项 目 单 位"));
    m_pointUnitLabel->setFont(ft);
    m_pointUnitEdit = new QLineEdit();
    m_pointUnitEdit->setFocusPolicy(Qt::StrongFocus);
    m_pointUnitEdit->setFixedSize(200,40);

    m_lineLayout = new QHBoxLayout();
    m_lineLayout->addStretch();
    m_lineLayout->addWidget(m_pointNameLabel);
    m_lineLayout->addSpacing(5);
    m_lineLayout->addWidget(m_pointNameEdit);
    m_lineLayout->addStretch();
    m_lineLayout->addWidget(m_registerAddrLabel);
    m_lineLayout->addSpacing(5);
    m_lineLayout->addWidget(m_registerAddrEdit);
    m_lineLayout->addStretch();

    m_typeLayout = new QHBoxLayout();
    m_typeLayout->addStretch();
    m_typeLayout->addWidget(m_pointTypeLabel);
    m_typeLayout->addSpacing(10);
    m_typeLayout->addWidget(m_pointTypeEdit);
    m_typeLayout->addStretch();
    m_typeLayout->addStretch();
    m_typeLayout->addWidget(m_registerTypeLabel);
    m_typeLayout->addSpacing(10);
    m_typeLayout->addWidget(m_registerTypeEdit);
    m_typeLayout->addStretch();

    m_coeffLayout = new QHBoxLayout();
    m_coeffLayout->addStretch();
    m_coeffLayout->addWidget(m_pointCoeffLabel);
    m_coeffLayout->addSpacing(10);
    m_coeffLayout->addWidget(m_pointCoeffEdit);
    m_coeffLayout->addStretch();
    m_coeffLayout->addStretch();
    m_coeffLayout->addWidget(m_pointUnitLabel);
    m_coeffLayout->addSpacing(10);
    m_coeffLayout->addWidget(m_pointUnitEdit);
    m_coeffLayout->addStretch();

    m_groupLayout = new QVBoxLayout();
    m_groupLayout->addLayout(m_lineLayout);
    m_groupLayout->addLayout(m_typeLayout);
    m_groupLayout->addLayout(m_coeffLayout);

    m_editBox = new QGroupBox();
    m_editBox->setLayout(m_groupLayout);

    m_confirmButton = new QPushButton(tr("确 定"));
    m_confirmButton->setFixedSize(100,40);
    m_confirmButton->setFont(ft);
    connect(m_confirmButton,SIGNAL(clicked()),this,SLOT(ConfirmButtonSlots()));
    m_cancleButton = new QPushButton(tr("取 消"));
    m_cancleButton->setFixedSize(100,40);
    m_cancleButton->setFont(ft);
    connect(m_cancleButton,SIGNAL(clicked()),this,SLOT(CancleButtonSlots()));
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_confirmButton);
    m_buttonLayout->addStretch();
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_cancleButton);
    m_buttonLayout->addStretch();

    m_buttonGroup = new QGroupBox();
    m_buttonGroup->setLayout(m_buttonLayout);

    m_wholeLayout = new QVBoxLayout();
    m_wholeLayout->addWidget(m_editBox);
    m_wholeLayout->addWidget(m_buttonGroup);

    this->setPalette(palette);
    this->setLayout(m_wholeLayout);
}

/**
 * @brief 析构函数
 */
PlcAddItemDialog::~PlcAddItemDialog()
{
    delete m_pointNameLabel;
    delete m_pointNameEdit;
    delete m_pointTypeLabel;
    delete m_pointTypeEdit;
    delete m_registerTypeLabel;
    delete m_registerTypeEdit;
    delete m_registerAddrLabel;
    delete m_registerAddrEdit;
    delete m_lineLayout;
    delete m_typeLayout;
    delete m_editBox;
    delete m_groupLayout;

    delete m_confirmButton;
    delete m_cancleButton;
    delete m_buttonLayout;
    delete m_buttonGroup;
    delete m_wholeLayout;
}

/**
 * @brief 确认按钮点击槽函数
 */
void PlcAddItemDialog::ConfirmButtonSlots()
{
    PlcPointInfo info;
    info.m_pointName = m_pointNameEdit->text().toStdString();
    info.m_registerAddr = m_registerAddrEdit->text().toInt();
    info.m_pointType = m_pointTypeEdit->currentIndex() + 1;
    info.m_registerType = m_registerTypeEdit->currentText().toStdString();
    if(m_pointCoeffEdit->text().isEmpty())
        info.m_pointCoefficient = 0;
    else
        info.m_pointCoefficient = m_pointCoeffEdit->text().toInt();
    info.m_pointUnit = m_pointUnitEdit->text().toStdString();

    m_pointNameEdit->clear();
    m_registerAddrEdit->clear();
    m_pointTypeEdit->setCurrentIndex(0);
    m_registerTypeEdit->setCurrentIndex(0);
    m_pointCoeffEdit->clear();
    m_pointUnitEdit->clear();

    this->close();
    //将新添加的一条PLC项目信息上传到PLC显示界面
    emit SubmitNewPlcPoint(info);
}

/**
 * @brief 取消按钮点击槽函数
 */
void PlcAddItemDialog::CancleButtonSlots()
{
    m_pointNameEdit->clear();
    m_registerAddrEdit->clear();
    m_pointTypeEdit->setCurrentIndex(0);
    m_registerTypeEdit->setCurrentIndex(0);
    m_pointUnitEdit->clear();
    m_pointCoeffEdit->clear();

    this->close();
}
