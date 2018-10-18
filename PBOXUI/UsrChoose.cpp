#include "UsrChoose.h"
#include "main.h"

//extern LineInfo gLine;

/**
 * @brief 用户选择窗口构造
 * @param parent
 */
UsrChoose::UsrChoose(mainWidget *parent) : m_parent(parent)
{
    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();
    //设置颜色
    QPalette palette;
    palette.setColor(QPalette::WindowText,Qt::white);
    palette.setBrush(QPalette::Background, Qt::black);
    //设置字号
    QFont font12;
    font12.setPointSize(12);
    QFont font20;
    font20.setPointSize(20);

    m_choseLabel = new QLabel(tr("请选择显示模式"));
    m_choseLabel->setFont(font20);
    m_choseLabel->setAlignment(Qt::AlignCenter);

    m_pboxSettingButton = new QPushButton(tr("PBOX配置界面"));
    m_pboxSettingButton->setFixedSize(width/3,height/8);
    m_pboxSettingButton->setFont(font12);

    m_displayButton = new QPushButton(tr("显示界面"));
    m_displayButton->setFixedSize(width/3,height/8);
    m_displayButton->setFont(font12);

//    m_testButton = new QPushButton(tr("测试界面"));
//    m_testButton->setFixedSize(width/3,height/8);
//    m_testButton->setFont(font12);

    m_plcStateButton = new QPushButton(tr("PLC状态显示"));
    m_plcStateButton->setFixedSize(width/3,height/8);
    m_plcStateButton->setFont(font12);
//    m_plcStateButton->setEnabled(false);

    m_contentVBoxLayout = new QVBoxLayout();
    m_contentVBoxLayout->setAlignment(Qt::AlignCenter);
    m_contentVBoxLayout->addStretch();
    m_contentVBoxLayout->addWidget(m_choseLabel);
    m_contentVBoxLayout->addStretch();
    m_contentVBoxLayout->addWidget(m_plcStateButton);
    m_contentVBoxLayout->addWidget(m_displayButton);
    m_contentVBoxLayout->addWidget(m_pboxSettingButton);
//    m_contentVBoxLayout->addWidget(m_testButton);
    m_contentVBoxLayout->addStretch();

    this->setLayout(m_contentVBoxLayout);

//    connect(m_testButton,SIGNAL(clicked()),this,SLOT(testDisplay()));
    connect(m_pboxSettingButton,SIGNAL(clicked()),this,SLOT(pboxSetting()));
    connect(m_displayButton,SIGNAL(clicked()),this,SLOT(dataDisplay()));
    connect(m_plcStateButton,SIGNAL(clicked()),this, SLOT(plcDisplay()));
}

/**
 * @brief 析构函数
 */
UsrChoose::~UsrChoose()
{
    delete m_choseLabel;
    delete m_pboxSettingButton;
    delete m_displayButton;
//    delete m_testButton;
    delete m_plcStateButton;
    delete m_contentVBoxLayout;
    delete m_parent;
}

/**
 * @brief PBOX配置界面按钮点击触发
 */
void UsrChoose::pboxSetting()
{
    this->close();
    m_parent->slot_pboxconfigclicked();
}

/**
 * @brief 显示界面点击触发
 */
void UsrChoose::dataDisplay()
{
    this->close();
    m_parent->showDisplaywidget();
}

/**
 * @brief 调试界面点击触发
 */
//void UsrChoose::testDisplay()
//{
//    this->close();
//    m_parent->showTestWidget();
//}

void UsrChoose::plcDisplay()
{
    this->close();
    m_parent->showPlcDisplay();
}
