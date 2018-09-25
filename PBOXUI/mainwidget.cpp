#include "mainwidget.h"
#include "main.h"


extern map<string,DeviceInfo> listDev;

/**
 * @brief 主窗口构造
 */
mainWidget::mainWidget()
{
    frmInput::Instance()->setFont(QFont("Microsoft YaHei", 10));
    frmInput::Instance()->Init("bottom", "silvery", 15, 10);

    startnum = 0;
    widgetFlag = 0;

    QPalette palette;
    palette.setColor(QPalette::WindowText,Qt::white);
    palette.setBrush(QPalette::Background, Qt::black);

    //构造进入配置窗口时的密码输入窗口
    m_passwidget = new PasswordWidget(this);
    m_passwidget->setPalette(palette);

    //构造用户选择窗口
    m_passwidget = new PasswordWidget(this);
    m_passwidget->setPalette(palette);

    //用户选择界面
    m_usrchosewidget = new UsrChoose(this);
    m_usrchosewidget->setPalette(palette);

    //构造显示窗口
    m_displaywidget = new display(this);
    m_displaywidget->setPalette(palette);

    //构造PBOX配置窗口
    m_config = new PBoxConfig(this);
    m_config->setPalette(palette);

    //构造设备调试窗口
//    m_testwidget = new TestFrame(this);
//    m_testwidget->setPalette(palette);

    //PLC显示界面
    m_plcDisplay = new PlcDisplay(this);
    m_plcDisplay->setPalette(palette);

    //PLC状态界面
    m_plcState = new PlcState(this);
    m_plcState->setPalette(palette);

    //构造控制PLC机械动作界面
    m_plcControl = new PlcControl(this);
    m_plcControl->setPalette(palette);

    //构造设置PLC参数界面
    m_plcSetting = new PlcSetting(this);
    m_plcSetting->setPalette(palette);

    //构造修改PLC项目信息界面
    m_plcModify = new PlcModifyItem(this);
    m_plcModify->setPalette(palette);

    //将各窗口添加到窗口堆栈中
    m_stackwidget = new QStackedWidget();
    m_stackwidget->addWidget(m_displaywidget);
    m_stackwidget->addWidget(m_usrchosewidget);
    m_stackwidget->addWidget(m_config);
//    m_stackwidget->addWidget(m_testwidget);
    m_stackwidget->addWidget(m_passwidget);
    m_stackwidget->addWidget(m_plcDisplay);
    m_stackwidget->addWidget(m_plcState);
    m_stackwidget->addWidget(m_passwidget);
    m_stackwidget->addWidget(m_plcControl);
    m_stackwidget->addWidget(m_plcSetting);
    m_stackwidget->addWidget(m_plcModify);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_stackwidget);

    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignHCenter);
    layout->setAlignment(Qt::AlignVCenter);
    layout->setSpacing(0);
    setLayout(layout);
    setPalette(palette);

    connect(m_plcModify,SIGNAL(PlcInfoIsChanged()),m_plcControl,SLOT(CheckPlcInfo()));
    connect(m_plcModify,SIGNAL(PlcInfoIsChanged()),m_plcSetting,SLOT(CheckPlcInfo()));

    connect(m_passwidget->m_confirmButton,SIGNAL(clicked()),this,SLOT(PasswordWidgetConfirm()));
    connect(m_passwidget->m_cancelButton,SIGNAL(clicked()),this,SLOT(PasswordWidgetCancel()));
}

/**
 * @brief 析构函数
 */
mainWidget::~mainWidget()
{
    delete m_stackwidget;
    delete m_usrchosewidget;
    delete m_displaywidget;
    delete m_stackwidget;
    delete m_passwidget;
    delete m_usrchosewidget;
    delete m_displaywidget;
    delete m_config;
//    delete m_testwidget;
    delete m_plcDisplay;
    delete m_plcState;
    delete m_plcControl;
    delete m_plcSetting;
    delete m_plcModify;

}

void mainWidget::showPlcDisplay()
{
    m_plcDisplay->m_timer->start(1000);
    PlcPointBase::SetDisplayType(5);
    PlcPointBase::SetReadPlcFlag(true);
    m_stackwidget->setCurrentWidget(m_plcDisplay);
}

void mainWidget::showPlcState()
{
    m_plcState->m_timer->start(500);
    m_plcState->UpdateDisplayItem(m_plcState->m_currentPageIndex);
    m_stackwidget->setCurrentWidget(m_plcState);
}

/**
 * @brief 显示控制PLC机械动作界面
 */
void mainWidget::showPlcControl()
{
    m_plcControl->UpdateDisplayItem(m_plcControl->m_currentPageIndex);
    m_stackwidget->setCurrentWidget(m_plcControl);
}
/**
 * @brief 显示设置PLC参数界面
 */
void mainWidget::showPlcSetting()
{
    m_plcSetting->m_timer->start(500);
    m_plcSetting->UpdateDisplayItem(m_plcSetting->m_currentPageIndex);
    m_stackwidget->setCurrentWidget(m_plcSetting);
}
/**
 * @brief 显示修改PLC项目信息界面
 */
void mainWidget::showPlcModifyItem()
{
    m_stackwidget->setCurrentWidget(m_plcModify);
}

/**
 * @brief 显示用户选择窗口
 */
void mainWidget::showUsrchosewidget()
{
    m_stackwidget->setCurrentWidget(m_usrchosewidget);
}

/**
 * @brief 显示PBOX配置窗口
 */
void mainWidget::showPBoxConfig()
{
    m_stackwidget->setCurrentWidget(m_config);
}

/**
 * @brief 显示数据展示窗口
 */
void mainWidget::showDisplaywidget()
{
    m_stackwidget->setCurrentWidget(m_displaywidget);
}

/**
 * @brief 显示设备调试窗口
 */
//void mainWidget::showTestWidget()
//{
//    emit addDevItems();
//    m_stackwidget->setCurrentWidget(m_testwidget);
//}

/**
 * @brief 用户选择进入PBOX配置界面时显示密码输入窗口
 */
void mainWidget::slot_pboxconfigclicked()
{
    m_stackwidget->setCurrentWidget(m_config);
}

/**
 * @brief 用户选择进入工艺配置界面时显示密码输入窗口
 */
void mainWidget::slot_configclicked()
{
    widgetFlag = 2;
    m_passwidget->m_dialogPassword->clear();
    m_stackwidget->setCurrentWidget(m_passwidget);
}

/**
 * @brief 密码输入窗口确认按钮点击槽函数
 */
void mainWidget::PasswordWidgetConfirm()
{
    string password = m_passwidget->m_dialogPassword->text().toStdString();
    if(password == "111111")
    {
        m_passwidget->close();
        if(widgetFlag == 1)
        {
            showPBoxConfig();
        }
    }
    else
    {
        m_passwidget->close();
        showUsrchosewidget();
        QMessageBox m_message;
        QPushButton *ok = m_message.addButton(tr("  确  定  "),QMessageBox::ActionRole);
        string text = "    密    码    错    误    ";
        m_message.setText(QString::fromStdString(text));
        m_message.move(380,250);
        m_message.setIcon(QMessageBox::Warning);
        m_message.exec();
        if(m_message.clickedButton() == ok)
        {
            m_message.close();
        }
    }
}

/**
 * @brief 密码输入窗口取消按钮点击槽函数
 */
void mainWidget::PasswordWidgetCancel()
{
    m_passwidget->close();
    showUsrchosewidget();
}

/**********************************************************/
/**
 * @brief 自定义LineEdit类构造函数
 * @param parent    父窗口
 */
MyLineEdit::MyLineEdit(QWidget* parent):QLineEdit(parent)
{
}

/**
 * @brief 自定义LineEdit类析构函数
 */
MyLineEdit::~MyLineEdit()
{}

/**
 * @brief 自定义焦点事件
 * @param event
 */
void MyLineEdit::focusInEvent(QFocusEvent *event)
{
    emit clicked();
    QLineEdit::focusInEvent(event);
}

void mainWidget::ReadPBOXIP()
{
    m_config->m_eth0Edit->setText(QString::fromStdString(m_config->GetEth0IP()));
    m_config->m_eth1Edit->setText(QString::fromStdString(m_config->GetEth1IP()));
}

/**********************************************************/



