#include "PBoxConfig.h"
#include "main.h"


QString PBoxConfig::m_productModel = "";
QString PBoxConfig::m_craftName = "";
string PBoxConfig::m_IPaddr = "";
bool PBoxConfig::m_checkJRFlag = true;
int PBoxConfig::m_setWorkMode = -1;

QPushButton* PBoxConfig::m_exportLogBtn;
QPushButton* PBoxConfig::m_updateSourceBtn;

/**
 * @brief PBOX配置界面构造
 * @param parent
 */
PBoxConfig::PBoxConfig(mainWidget *parent): m_parent(parent),m_checkJRFlagShadow(true)
{
    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::white);
    QFont ft_g;
    ft_g.setPointSize(14);
    QFont ft;
    ft.setPointSize(20);

/*
 *  first layout : title
 */

    m_tileLabel = new QLabel(tr("PBox设置"));
    m_tileLabel->setFont(ft);
    m_tileLabel->setPalette(pa);
    m_titleLayout = new QHBoxLayout();
    m_titleLayout->addStretch();
    m_titleLayout->addWidget(m_tileLabel);
    m_titleLayout->addStretch();
    m_firstGroup = new QGroupBox();
    m_firstGroup->setLayout(m_titleLayout);
    m_firstGroup->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    m_firstGroup->setFixedWidth(width - 20);
    m_firstGroup->setFixedHeight(height/8);

/*
 *  second layout : configfilename_LB, configfilename_LE, updateXML, m_exportLogBtn
 */
    m_secondGridLayout = new QGridLayout();
    m_exportLogBtn = new QPushButton(tr("导 出 日 志 文 件"));
    m_exportLogBtn->setFixedSize(width/6,height/15);
    m_updateSourceBtn = new QPushButton(tr(" 更新PBOX程序 "));
    m_updateSourceBtn->setFixedSize(width/6,height/15);

    m_secondGridLayout->setVerticalSpacing(20);
    m_secondGridLayout->setColumnStretch(0, 1);
    m_secondGridLayout->setColumnStretch(1, 1);
    m_secondGridLayout->setColumnStretch(2, 1);
    m_secondGridLayout->setColumnStretch(3, 1);
    m_secondGridLayout->setColumnStretch(4, 1);
    m_secondGridLayout->setColumnStretch(5, 1);
    m_secondGridLayout->setColumnStretch(6, 1);
    m_secondGridLayout->setColumnStretch(7, 1);
    m_secondGridLayout->setColumnStretch(8, 1);
    m_secondGridLayout->setColumnStretch(9, 1);


    connect(m_exportLogBtn,SIGNAL(clicked()),this,SLOT(ExportLog()));
    connect(m_updateSourceBtn,SIGNAL(clicked()),this,SLOT(UpdateSource()));

    m_workCenterNoLabel = new QLabel(tr("工作中心编号 "));
    m_workCenterNoLE = new QLineEdit();
    m_workCenterNoLE->setFont(ft_g);
    m_workCenterNoLE->setPalette(pa);
    m_workCenterNoLE->setFixedSize(width/8,height/18);

    m_workModeLabel = new QLabel(tr("工作模式"));
    m_workmodecomBox = new QComboBox();
    m_workmodecomBox->setStyleSheet("QComboBox QAbstractItemView::item{height:35px;}");
    m_workmodecomBox->setView(new QListView());
    m_workmodecomBox->setFixedSize(width/8,height/18);
    m_workmodecomBox->addItem(tr("网络模式"));
    m_workmodecomBox->addItem(tr("本地模式"));
    m_workmodecomBox->addItem(tr("样件模式"));

    m_checkPreJRLabel = new QLabel(tr("检测前工位"));
    m_checkPreJRCombox = new QComboBox();
    m_checkPreJRCombox->setStyleSheet("QComboBox QAbstractItemView::item{height:35px;}");
    m_checkPreJRCombox->setView(new QListView());
    m_checkPreJRCombox->setFixedSize(width/8,height/18);
    m_checkPreJRCombox->addItem(tr("是"));
    m_checkPreJRCombox->addItem(tr("否"));
    connect(m_checkPreJRCombox, SIGNAL(currentIndexChanged(QString)),this, SLOT(checkPreJR(QString)));



    m_secondGridLayout->addWidget(m_exportLogBtn,0,1,1,2,Qt::AlignCenter);
    m_secondGridLayout->addWidget(m_updateSourceBtn,1,1,1,2,Qt::AlignCenter);
    m_secondGridLayout->addWidget(m_workCenterNoLabel,0,4,1,1,Qt::AlignCenter);
    m_secondGridLayout->addWidget(m_workModeLabel,1,4,1,1,Qt::AlignCenter);
    m_secondGridLayout->addWidget(m_workCenterNoLE,0,5,1,1,Qt::AlignCenter);
    m_secondGridLayout->addWidget(m_workmodecomBox,1,5,1,1,Qt::AlignCenter);
    m_secondGridLayout->addWidget(m_checkPreJRLabel,0,7,1,1,Qt::AlignCenter);
    m_secondGridLayout->addWidget(m_checkPreJRCombox,0,8,1,1,Qt::AlignCenter);


    m_secondGroup = new QGroupBox();
    m_secondGroup->setLayout(m_secondGridLayout);
    m_secondGroup->setFixedSize(width - 40,height/4);

/*
 *  third layout : m_eth0Label, m_eth1Label, m_eth0Edit, m_eth1Edit
 */

    m_eth0Label = new QLabel(tr("网口1IP "));
    m_eth0Label->setFont(ft_g);
    m_eth0Label->setPalette(pa);
    m_eth1Label = new QLabel(tr("网口2IP "));
    m_eth1Label->setFont(ft_g);
    m_eth1Label->setPalette(pa);
    m_eth0Edit = new MyLineEdit();
    connect(m_eth0Edit,SIGNAL(clicked()),this,SLOT(TopKeyboard()));
    connect(m_eth0Edit,SIGNAL(editingFinished()),this,SLOT(BottomKeyboard()));
    m_eth0Edit->setFixedSize(width/8,height/18);
    m_eth1Edit = new MyLineEdit();
    connect(m_eth1Edit,SIGNAL(clicked()),this,SLOT(TopKeyboard()));
    connect(m_eth1Edit,SIGNAL(editingFinished()),this,SLOT(BottomKeyboard()));
    m_eth1Edit->setFixedSize(width/8,height/18);

    m_ipGridLayout = new QGridLayout();
    m_ipGridLayout->setColumnStretch(0, 1);
    m_ipGridLayout->setColumnStretch(1, 1);
    m_ipGridLayout->setColumnStretch(2, 1);
    m_ipGridLayout->setColumnStretch(3, 1);
    m_ipGridLayout->setColumnStretch(4, 1);
    m_ipGridLayout->setColumnStretch(5, 1);
    m_ipGridLayout->setColumnStretch(6, 1);

    m_ipLayout1 = new QHBoxLayout();
    m_ipLayout1->addStretch();
    m_ipLayout1->addWidget(m_eth0Label);
    m_ipLayout1->addWidget(m_eth0Edit);
    m_ipLayout1->addStretch();
    m_ipGridLayout->addLayout(m_ipLayout1,0,1,1,2,Qt::AlignHCenter);
    m_ipGridLayout->addWidget(m_eth1Label,0,4,1,1,Qt::AlignRight);
    m_ipGridLayout->addWidget(m_eth1Edit,0,5,1,1,Qt::AlignLeft);
    m_confirmButton = new QPushButton(tr(" 确     认 "));
    m_confirmButton->setFixedSize(width/8,height/15);
    connect(m_confirmButton,SIGNAL(clicked()),this,SLOT(ConfirmPress()));
    m_cancelButton = new QPushButton(tr(" 返     回 "));
    m_cancelButton->setFixedSize(width/8,height/15);
    connect(m_cancelButton,SIGNAL(clicked()),this,SLOT(CancelPress()));
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_confirmButton);
    m_buttonLayout->addStretch();
    m_buttonLayout->addStretch();
    m_buttonLayout->addSpacing(30);
    m_buttonLayout->addWidget(m_cancelButton);
    m_buttonLayout->addStretch();

    m_thirdGroup = new QGroupBox();
    m_thirdGroup->setLayout(m_ipGridLayout);
    m_thirdGroup->setFixedSize(width - 40,height/5);

    m_forthLayout = new QVBoxLayout();
    m_forthLayout->addStretch();
    m_forthLayout->addLayout(m_buttonLayout);
    m_forthLayout->addStretch();

    m_forthGroup = new QGroupBox();
    m_forthGroup->setLayout(m_forthLayout);
    m_forthGroup->setFixedSize(width - 20,height/8);

    m_fifthLayout = new QVBoxLayout();
    m_fifthLayout->addWidget(m_secondGroup);
    m_fifthLayout->addWidget(m_thirdGroup);
    m_fifthGroup = new QGroupBox();
    m_fifthGroup->setLayout(m_fifthLayout);

    m_wholeLayout = new QVBoxLayout();
    m_wholeLayout->addWidget(m_firstGroup);
    m_wholeLayout->addWidget(m_fifthGroup);
    m_wholeLayout->addWidget(m_forthGroup);

    m_dialog = new QProgressDialog(this);
    m_dialog->setFont(ft_g);
    m_dialog->setCancelButton(0);
    m_dialog->setRange(0,0);
    m_dialog->cancel();
    m_enableSampleMode = 0;
    if(WorkModeNotify > 0)
    {
        m_workmodeshadow = 1;
        m_workmode = 1;
    }
    else
    {
        m_workmodeshadow = 0;
        m_workmode = 0;
    }
    m_workmodecomBox->setCurrentIndex(m_workmode);

//    modify by wjl 2017/11/20 start
    connect(m_workmodecomBox,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[this](int index)
    {
        if(index == 0)
        {
            m_workmode = 0;
            m_enableSampleMode = 0;
        }
        else
        {
            if(index == 2)
            {
                m_enableSampleMode = 1;
            }
            else
            {
                m_enableSampleMode = 0;
            }
            m_workmode = 1;
        }
    });
    m_workCenterNoLE->setText(QString::fromStdString(g_dbInfo.GetWorkCenterNo()));
    m_eth0IP = getPBoxEthip(0);
    m_eth1IP = getPBoxEthip(1);
    m_eth0Edit->setText(QString::fromStdString(m_eth0IP));
    m_eth1Edit->setText(QString::fromStdString(m_eth1IP));

    this->setWindowOpacity(1);      //设置透明度
    this->setFont(ft_g);
    this->setFixedSize(width,height); //设置窗体大小为屏幕大小
    setAutoFillBackground(true);
    this->setLayout(m_wholeLayout);

    checkSetWorkMode = new QTimer(this);
    checkSetWorkMode->start(1000);
    connect(checkSetWorkMode,SIGNAL(timeout()),this,SLOT(SetWorkMode()));
}

/**
 * @brief 析构函数
 */
PBoxConfig::~PBoxConfig()
{
    delete m_exportLogBtn;
    delete m_updateSourceBtn;
    delete m_eth0Edit;
    delete m_eth1Edit;
    delete m_ipLayout1;
    delete m_parent;
    delete m_tileLabel;
    delete m_titleLayout;
    delete m_firstGroup;
    delete m_eth0Label;
    delete m_eth1Label;
    delete m_workCenterNoLabel;
    delete m_workCenterNoLE;
    delete m_workModeLabel;
    delete m_workmodecomBox;
    delete m_ipGridLayout;
    delete m_dialog;
    delete m_confirmButton;
    delete m_cancelButton;
    delete m_buttonLayout;
    delete m_secondGridLayout;
    delete m_thirdGroup;
    delete m_forthLayout;
    delete m_forthGroup;
    delete m_fifthLayout;
    delete m_fifthGroup;
    delete m_wholeLayout;
    delete m_checkPreJRLabel;
    delete m_checkPreJRCombox;
}

void PBoxConfig::checkPreJR(QString)
{
    QString checkPreJR = m_checkPreJRCombox->currentText();
    qDebug()<<"--- checkPreJR = "<<checkPreJR;
    if(checkPreJR == "是")
    {
        m_checkJRFlagShadow = true;
    }
    else
    {
        m_checkJRFlagShadow = false;
    }
}

/**
 * @brief 导出日志文件槽函数
 */
void PBoxConfig::ExportLog()
{
    QMessageBox message;
    QPushButton *ok = message.addButton(tr("  是  "),QMessageBox::ActionRole);
    QPushButton *cancel = message.addButton(tr("  否  "),QMessageBox::ActionRole);
    message.setText(QString::fromStdString("  是 否 导 出 日 志 文 件  ?  "));
    message.move(340,250);
    message.setIcon(QMessageBox::Question);
    message.exec();
    if(message.clickedButton() == ok)
    {
        m_exportLogBtn->setEnabled(false);
        pthread_t pth;
        pthread_attr_t pth_attr;
        pthread_attr_init(&pth_attr);
        pthread_attr_setdetachstate(&pth_attr,PTHREAD_CREATE_DETACHED);
        pthread_create(&pth,&pth_attr,PBoxConfig::processExportLog,this);
        m_dialog->move(380,250);
        m_dialog->setLabelText(tr("正在导出日志文件，请稍候..."));
        m_dialog->exec();
        m_exportLogBtn->setEnabled(true);
    }
    else if(message.clickedButton() == cancel)
    {
        message.close();
    }

}

/**
 * @brief 导出配置文件槽函数
 */
//void PBoxConfig::slot_exportXML()
//{
//    QMessageBox message;
//    QPushButton *ok = message.addButton(tr("  是  "),QMessageBox::ActionRole);
//    QPushButton *cancel = message.addButton(tr("  否  "),QMessageBox::ActionRole);
//    message.setText("  是 否 导 出 配 置 文 件  ?  ");
//    message.move(340,250);
//    message.setIcon(QMessageBox::Question);
//    message.exec();
//    if(message.clickedButton() == ok)
//    {
//        exportXML->setEnabled(false);
//        pthread_t pth;
//        pthread_attr_t pth_attr;
//        pthread_attr_init(&pth_attr);
//        pthread_attr_setdetachstate(&pth_attr,PTHREAD_CREATE_DETACHED);
//        pthread_create(&pth,&pth_attr,PBoxConfig::processExportXML,this);
//        m_dialog->move(380,250);
//        m_dialog->setLabelText(tr("正在导出配置文件，请稍候..."));
//        m_dialog->exec();
//        exportXML->setEnabled(true);
//    }
//    else if(message.clickedButton() == cancel)
//    {
//        message.close();
//    }
//}

/**
 * @brief 更新PBOX程序槽函数
 */
void PBoxConfig::UpdateSource()
{
    QMessageBox message;
    QPushButton *ok = message.addButton(tr("  是  "),QMessageBox::ActionRole);
    QPushButton *cancel = message.addButton(tr("  否  "),QMessageBox::ActionRole);
    message.setText("  是 否 更 新 PBOX 程 序 ?  ");
    message.move(340,250);
    message.setIcon(QMessageBox::Question);
    message.exec();
    if(message.clickedButton() == ok)
    {
        m_updateSourceBtn->setEnabled(false);
        pthread_t pth;
        pthread_attr_t pth_attr;
        pthread_attr_init(&pth_attr);
        pthread_attr_setdetachstate(&pth_attr,PTHREAD_CREATE_DETACHED);
        pthread_create(&pth,&pth_attr,PBoxConfig::processUpdateSouece,this);
        m_dialog->move(380,250);
        m_dialog->setLabelText(tr("正在更新PBOX程序，请稍候..."));
        m_dialog->exec();
        m_updateSourceBtn->setEnabled(true);
    }
    else if(message.clickedButton() == cancel)
    {
        message.close();
    }
}

/**
 * @brief 确认按钮点击触发函数
 */
void PBoxConfig::ConfirmPress()
{
    string text = "";
    string workcenterno = m_workCenterNoLE->text().toStdString();
    string ip0 = "";
    if(checkIpStr(m_eth0Edit->text(),ip0) != 1)
    {
        text = "     IP0输入不合法，请重新输入 ！    ";
        showMessageBox(text);
        return;
    }
    string ip1 = "";
    if(checkIpStr(m_eth1Edit->text(),ip1) != 1)
    {
        text = "     IP1输入不合法，请重新输入 ！    ";
        showMessageBox(text);
        return;
    }
    if(ip0.compare(ip1) == 0)
    {
        text = "     IP0和IP1不能相同，请重新输入 ！    ";
        showMessageBox(text);
        return;
    }
    if((m_eth0IP.compare(ip0) == 0) && (m_eth1IP.compare(ip1) == 0) &&
            ((g_dbInfo.GetWorkCenterNo().compare(workcenterno)) == 0) &&
            (m_workmode == m_workmodeshadow) && (m_checkJRFlagShadow == m_checkJRFlag) &&
            (m_enableSampleMode == EnableSampleMode))
    {
        text = "     没有改变任何配置选项！    ";
        showMessageBox(text);
//        this->close();
//        m_parent->showUsrchosewidget();
        return;
    }
    else
    {
        text = "";
        if(m_eth0IP.compare(ip0) != 0)
        {
            text += "IP地址0";
        }
        if(m_eth1IP.compare(ip1) != 0)
        {
            if(!(text.empty()))
            {
                text += "、";
            }
            text += "IP地址1";
        }
        if((g_dbInfo.GetWorkCenterNo().compare(workcenterno)) != 0)
        {
            if(!(text.empty()))
            {
                text += "、";
            }
            text += "工作中心编号";
        }
        if((m_workmode != m_workmodeshadow) || (m_enableSampleMode != EnableSampleMode))
        {
            if(!(text.empty()))
            {
                text += "、";
            }
            text += "工作模式";
        }
        if(m_checkJRFlagShadow != m_checkJRFlag)
        {
            if(!(text.empty()))
            {
                text += "、";
            }
            text += "检测前工位模式";
        }
        text += "被改变,是否变更？";
        if(!showMessageBox(text,1))
        {
            return;
        }
    }
    int tmperrno;
    tmperrno = 0;

    if((m_eth0IP.compare(ip0) != 0) || (m_eth1IP.compare(ip1) != 0))
    {
        string filename = "./ipconfig.sh";
        string ip0command = "ifconfig eth0 ";
        string ip1command = "ifconfig eth1 ";
        ip0command += ip0;
        ip0command += "\n";
        ip1command += ip1;
        ip1command += "\n";
        QFile file(QString::fromStdString(filename));
        if(file.open(QFile::WriteOnly | QFile::Truncate))
        {
            file.write(ip0command.data());
            file.write(ip1command.data());
            file.close();
            text = "保存IP地址0和1到ipconfig.txt成功，新IP地址软件需要重启生效 ！    ";
            showMessageBox(text);
            m_eth0Edit->setText(QString::fromStdString(ip0));
            m_eth1Edit->setText(QString::fromStdString(ip1));
            system("reboot");
        }
        else
        {
            text = "     保存IP地址0和1到ipconfig.txt失败 ！    ";
            showMessageBox(text);
            tmperrno = 2;
        }
    }

    if(m_workmode != m_workmodeshadow)
    {
        m_workmodeshadow = m_workmode;
        if(m_workmodeshadow == 1)
        {
            currEditPartNo = "";
            pthread_mutex_lock(&mutexDeviceDriverCond);
            WorkModeNotify = m_workmodeshadow;
            pthread_mutex_unlock(&mutexDeviceDriverCond);
            pthread_cond_signal(&cond);
        }
        else
        {
            if(WorkModeNotify == 2)
            {
                pthread_mutex_lock(&mutexDeviceDriverCond);
                WorkModeNotify = 1;
                pthread_mutex_unlock(&mutexDeviceDriverCond);
                pthread_cond_signal(&cond);
            }
            else
            {
                pthread_mutex_lock(&mutexDeviceDriverCond);
                WorkModeNotify = m_workmodeshadow;
                pthread_mutex_unlock(&mutexDeviceDriverCond);
                pthread_cond_signal(&cond);
            }
        }
        _log.LOG_INFO("生产模式变更：m_workmodeshadow = 【%d】",m_workmodeshadow);
        QProgressDialog *progressDlg=new QProgressDialog(this);
        QFont font;
        font.setPointSize(18);
        progressDlg->setFont(font);
        progressDlg->setFixedHeight(160);
        progressDlg->setWindowModality(Qt::WindowModal);
        progressDlg->setMinimumDuration(0);
        progressDlg->setAttribute(Qt::WA_DeleteOnClose, true);
        progressDlg->setLabelText(tr("正在切换工作模式请等待......"));
        progressDlg->setCancelButton(0);
        progressDlg->setStyleSheet("QProgressBar{border:1px solid #FFFFFF;"
                                                         "height:30;"
                                                         "background:blue;"
                                                         "text-align:center;"
                                                         "color:rgb(255,255,0);"
                                                         "border-radius:10px;}"
                                     "QProgressBar::chunk{"
                                                         "border-radius:0px;"
                                                         "border:0px solid green;"
                                                         "background:green;}"
                                                        );
        progressDlg->setRange(0,13);
        progressDlg->setValue(0);
        int timenum = 0;
        for(;timenum < 13;timenum++)
        {
            sleep(1);
            progressDlg->setValue(timenum + 1);
            if(timenum == 10)
            {
                break;
            }
        }
        if(WorkModeNotify != m_workmodeshadow)
        {
            pthread_mutex_lock(&mutexDeviceDriverCond);
            WorkModeNotify = m_workmodeshadow;
            pthread_mutex_unlock(&mutexDeviceDriverCond);
            pthread_cond_signal(&cond);
        }
        for(;timenum < 13;timenum++)
        {
            sleep(1);
            progressDlg->setValue(timenum + 1);
        }
        progressDlg->close();
    }

    if(m_enableSampleMode != EnableSampleMode)
    {
        EnableSampleMode = m_enableSampleMode;
        text = "       模式切换完成！      ";
        showMessageBox(text);
        _log.LOG_INFO("样件模式切换完成：EnableSampleMode = 【%d】",EnableSampleMode);
    }

    if((g_dbInfo.GetWorkCenterNo().compare(workcenterno)) != 0)
    {
        if(g_dbInfo.SetWorkCenterNo(m_workCenterNoLE->text().toStdString()))
        {
            text = "     保存工作中心编号到config.txt成功 ！    ";
            showMessageBox(text);
        }
        else
        {
            text = "     保存工作中心编号到config.txt失败 ！    ";
            showMessageBox(text);
            tmperrno = 1;
        }
    }

    if(m_checkJRFlagShadow != m_checkJRFlag)
    {
        m_checkJRFlag = m_checkJRFlagShadow;
        text = "       检测前工位模式变更成功！      ";
        showMessageBox(text);
        _log.LOG_INFO("检测前工位选项变更：m_checkJRFlag = 【%d】",m_checkJRFlag?1:0);
    }

    if(tmperrno == 0)
    {
        emit UpdateOrExprotOkSignal(-3);
    }
    else
    {
        return;
    }
}

/**
 * @brief 取消按钮点击触发
 */
void PBoxConfig::CancelPress()
{
    if(m_workmode != m_workmodeshadow)
    {
        m_workmode = m_workmodeshadow;
        m_enableSampleMode = EnableSampleMode;
        if(EnableSampleMode == 0)
        {
            m_workmodecomBox->setCurrentIndex(m_workmodeshadow);
        }
        else
        {
            m_workmodecomBox->setCurrentIndex(2);
        }
    }
    else
    {
        if(m_enableSampleMode != EnableSampleMode)
        {
            m_enableSampleMode = EnableSampleMode;
            if(EnableSampleMode == 1)
            {
                m_workmodecomBox->setCurrentIndex(2);
            }
            else
            {
                m_workmodecomBox->setCurrentIndex(1);
            }
        }
    }
    string workcenterno = m_workCenterNoLE->text().toStdString();
    if((g_dbInfo.GetWorkCenterNo().compare(workcenterno)) != 0)
    {
        m_workCenterNoLE->setText(QString::fromStdString(g_dbInfo.GetWorkCenterNo()));
    }
    string ip0 = "";
    if(checkIpStr(m_eth0Edit->text(),ip0) != 1)
    {
        m_eth0Edit->setText(QString::fromStdString(m_eth0IP));
    }
    else
    {
        if(m_eth0IP.compare(ip0) != 0)
        {
            m_eth0Edit->setText(QString::fromStdString(m_eth0IP));
        }
    }
    string ip1 = "";
    if(checkIpStr(m_eth1Edit->text(),ip1) != 1)
    {
        m_eth1Edit->setText(QString::fromStdString(m_eth1IP));
    }
    else
    {
        if(m_eth1IP.compare(ip1) != 0)
        {
            m_eth1Edit->setText(QString::fromStdString(m_eth1IP));
        }
    }
    if(m_checkJRFlagShadow != m_checkJRFlag)
    {
        if(m_checkJRFlag)
        {
            m_checkPreJRCombox->setCurrentIndex(0);
        }
        else
        {
            m_checkPreJRCombox->setCurrentIndex(1);
        }
        m_checkJRFlagShadow  = m_checkJRFlag;
    }
    this->close();
    m_parent->showUsrchosewidget();
}

/**
 * @brief 导出日志文件接口函数
 * @param arg
 * @return
 */
void* PBoxConfig::processExportLog(void* arg)
{
    PBoxConfig* th = (PBoxConfig*)arg;
    th->processLog();
    return NULL;
}

/**
 * @brief 导出XML配置文件接口函数
 * @param arg
 * @return
 */
//void* PBoxConfig::processExportXML(void* arg)
//{
//    PBoxConfig* th = (PBoxConfig*)arg;
//    th->processexportXML();
//    return NULL;
//}

/**
 * @brief 更新XML配置文件接口函数
 * @param arg
 * @return
 */
//void* PBoxConfig::processUpdateXML(void* arg)
//{
//    PBoxConfig* pth = (PBoxConfig*)arg;
//    pth->processXML();
//    return NULL;
//}

/**
 * @brief 更新源程序接口函数
 * @param arg
 * @return
 */
void* PBoxConfig::processUpdateSouece(void* arg)
{
    PBoxConfig* pth = (PBoxConfig*)arg;
    pth->processSource();
    return NULL;
}

/**
 * @brief 更新源程序
 */
void PBoxConfig::processSource()
{
    string cmd;
    if(opendir("/mnt/PBox") == NULL)
    {
        cmd.append("mount -t cifs -o user=administrator,passwd=123,nounix,noserverino //");
        cmd.append(m_IPaddr);
        cmd.append("/share /mnt/");
        system(cmd.data());
    }
    string filename = "PBOX";
    cmd.clear();
    cmd.append("cp ").append("/mnt/PBox/bin/").append(filename).append(" ./PBOX ");
    system(cmd.data());
    system("sync");
    QFile file("./PBOX");
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        usleep(500 * 1000);
        m_dialog->close();
        emit UpdateOrExprotErrorSignal(4);
    }
    else
    {
        file.close();
        usleep(500 * 1000);
        m_dialog->close();
        emit UpdateOrExprotOkSignal(-2);
    }
}

/**
 * @brief 导出XML配置文件
 */
//void PBoxConfig::processexportXML()
//{
//    string command;
//    string Code = gLine.Si.Code;
//    if(opendir("/mnt/PBox") == NULL)
//    {
//        command.append("mount -t cifs -o user=administrator,passwd=123,nounix,noserverino //");
//        command.append(m_IPaddr);
//        command.append("/share /mnt/");
//        system(command.data());
//    }
//    command.clear();
//    string xmlpath = "/mnt/PBox/Config/" + Code + ".xml";
//    command.append("cp ").append("/root/PBOX.xml ").append(xmlpath);
//    system(command.data());

//    QFile xml(xmlpath.data());
//    if(xml.open(QFile::ReadOnly | QFile::Text))
//    {
//        xml.close();
//        usleep(500 * 1000);
//        m_dialog->close();
//        emit UpdateOrExprotOk(-1);
//    }
//    else
//    {
//        usleep(500 * 1000);
//        m_dialog->close();
//        emit UpdateOrExprotError(3);
//    }
//}

/**
 * @brief 处理日志文件
 */
void PBoxConfig::processLog()
{
    string command;
    string logname = "./Log/";
    char filename[10] = {0};
    unsigned int flag = 0;
    string Code = gLine.Si.Code;
    if(Code.empty())
    {
        Code = "Log";
    }
    string dirname = "./" + Code;
    for(unsigned int i = 0;i < 10;i++)
    {
        sprintf(filename,"Log%d.txt",i);
        QFile file(QString::fromStdString(logname + filename));
        if(file.open(QFile::ReadOnly | QFile::Text))
        {
            file.close();
            flag++;
        }
    }
    if(flag == 0)
    {
        usleep(500 * 1000);
        m_dialog->close();
        emit UpdateOrExprotErrorSignal(2);
    }
    else
    {
        if(Code != "Log")
        {
            command = "cp " + logname + " " + dirname + " -rf";
            system(command.data());
        }
        command.clear();
        command.append("tar -zcvf ").append(Code).append(".tar.gz ").append(dirname);
        system(command.data());
        command.clear();
        if(opendir("/mnt/PBox") == NULL)
        {
            command.append("mount -t cifs -o user=administrator,passwd=123,nounix,noserverino //");
            command.append(m_IPaddr);
            command.append("/share /mnt/");
            system(command.data());
        }
        command.clear();
        command.append("mv ").append(Code).append(".tar.gz ").append("/mnt/PBox/Log/").append(Code).append("_").append("Log.tar.gz");
        system(command.data());
        usleep(500 * 1000);
        m_dialog->close();
        emit UpdateOrExprotOkSignal(flag);
    }
}

/**
 * @brief 更新XML配置文件
 */
//void PBoxConfig::UpdateXML()
//{
//    QMessageBox message;
//    QPushButton *ok = message.addButton(tr("  是  "),QMessageBox::ActionRole);
//    QPushButton *cancel = message.addButton(tr("  否  "),QMessageBox::ActionRole);
//    message.setText("  是  否  更 新 配 置 文 件  ?  ");
//    message.move(340,250);
//    message.setIcon(QMessageBox::Question);
//    message.exec();
//    if(message.clickedButton() == ok)
//    {
//        updateXML->setEnabled(false);
//        pthread_t pth;
//        pthread_create(&pth,NULL,PBoxConfig::processUpdateXML,this);
//        m_dialog->move(370,250);
//        m_dialog->setLabelText(tr("正在更新配置文件，请稍候..."));
//        m_dialog->exec();
//        updateXML->setEnabled(true);
//    }
//    else if(message.clickedButton() == cancel)
//    {
//        message.close();
//    }
//}

/**
 * @brief 导出或更新出错时处理函数
 * @param flag  1：更新配置文件失败  2：导出日志失败    3：导出配置文件失败  4：更新源文件
 */
void PBoxConfig::UpdateOrExportErrorSlot(int flag)
{
    if(flag == 1)
    {
        QMessageBox message;
        QPushButton *cancel = message.addButton(tr("  关 闭  "),QMessageBox::ActionRole);
        message.setText(QString::fromStdString("     更 新 配 置 文 件 失 败 ！    "));
        message.setIcon(QMessageBox::Warning);
        message.move(360,250);
        message.exec();
        if(message.clickedButton() == cancel)
        {
            message.close();
        }
    }
    else if(flag == 2)
    {
        QMessageBox message;
        QPushButton *cancel = message.addButton(tr("  关 闭  "),QMessageBox::ActionRole);
        message.setText(QString::fromStdString(" 导出日志文件失败,本地没有日志文件！ "));
        message.setIcon(QMessageBox::Warning);
        message.move(320,250);
        message.exec();
        if(message.clickedButton() == cancel)
        {
            message.close();
        }
    }
    else if(flag == 3)
    {
        QMessageBox message;
        QPushButton *cancel = message.addButton(tr("  关 闭  "),QMessageBox::ActionRole);
        message.setText(QString::fromStdString("     导 出 配 置 文 件 失 败 ！    "));
        message.setIcon(QMessageBox::Warning);
        message.move(360,250);
        message.exec();
        if(message.clickedButton() == cancel)
        {
            message.close();
        }
    }
    else if(flag == 4)
    {
        QMessageBox message;
        QPushButton *cancel = message.addButton(tr("  关 闭  "),QMessageBox::ActionRole);
        message.setText(QString::fromStdString("     更 新 源 文 件 失 败 ！    "));
        message.setIcon(QMessageBox::Warning);
        message.move(360,250);
        message.exec();
        if(message.clickedButton() == cancel)
        {
            message.close();
        }
    }
}

/**
 * @brief 导出或更新成功处理函数
 * @param flag  0：更新配置文件成功  -1：导出配置文件成功 -2：更新源程序成功  -3：修改IP >0：导出日志文件个数
 */
void PBoxConfig::UpdateOrExportOkSlot(int flag)
{
    if(flag == 0)
    {
        QMessageBox message;
        QPushButton *ok = message.addButton(tr("是"),QMessageBox::ActionRole);
        QPushButton *cancel = message.addButton(tr(" 稍候重启 "),QMessageBox::ActionRole);
        message.setText(QString::fromStdString("更新配置文件完成,是否立即重新启动?"));
        message.move(320,250);
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
    else if(flag == -1)
    {
        QMessageBox message;
        QPushButton *ok = message.addButton(tr(" 确定 "),QMessageBox::ActionRole);
        message.setIcon(QMessageBox::Information);
        message.setText(QString::fromStdString(" 导 出 配 置 文 件 完 成 "));
        message.move(360,250);
        message.exec();
        if(message.clickedButton() == ok)
        {
            message.close();
        }
    }
    else if(flag == -2)
    {
        QMessageBox message;
        QPushButton *ok = message.addButton(tr("是"),QMessageBox::ActionRole);
        QPushButton *cancel = message.addButton(tr(" 稍候重启 "),QMessageBox::ActionRole);
        message.setText(QString::fromStdString("更新源文件完成,是否立即重新启动?"));
        message.move(330,250);
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
    else if(flag == -3)
    {
        QMessageBox message;
        QPushButton *ok = message.addButton(tr(" 立即重启 "),QMessageBox::ActionRole);
        QPushButton *cancel = message.addButton(tr(" 稍候重启 "),QMessageBox::ActionRole);
        message.setText(QString::fromStdString("修改PBOX IP成功,需要重启生效,是否重启？"));
        message.move(330,250);
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
    else
    {
        QMessageBox message;
        QPushButton *ok = message.addButton(tr(" 确定 "),QMessageBox::ActionRole);
        string text;
        text = " 导出日志文件完成,导出文件个数 : ";
        char num[5] = {0};
        sprintf(num,"%d",flag);
        text.append(num).append("   ");
        message.setText(QString::fromStdString(text));
        message.move(320,250);
        message.setIcon(QMessageBox::Information);
        message.exec();
        if(message.clickedButton() == ok)
        {
            message.close();
        }
    }
}

/**
 * @brief 处理XML配置文件
 */
//void PBoxConfig::processXML()
//{
//    string cmd;
//    if(opendir("/mnt/PBox") == NULL)
//    {
//        cmd.append("mount -t cifs -o user=administrator,passwd=123,nounix,noserverino //");
//        cmd.append(m_IPaddr);
//        cmd.append("/share /mnt/");
//        system(cmd.data());
//    }

//    //add by vincent 2017-1-19
//    FILE *fp;
//    char tmp[100]={0};
//    string buff;
//    string productModel;
//    string Code;
//    fp = fopen("/root/PBOX_ProModel.txt","r");
//    if(fp!=NULL)
//    {
//            while(fgets(tmp,sizeof(tmp),fp)!=NULL)
//            {
//                 buff.assign(tmp);
//                 if((strncmp(tmp,"ProductModel:",13)==0) && (strlen(tmp)>13))
//                 {
//                        productModel = buff.substr(13,buff.size()-14);
//                        printf("==== productModel %s \n",productModel.data());
//                 }
//                 if((strncmp(tmp,"Code:",10)==0) && (strlen(tmp) > 10))
//                 {
//                        Code = buff.substr(10,buff.size()-11);
//                        printf("==== Code %s \n",Code.data());
//                 }
//            }
//    }
//    fclose(fp);

//    system("rm /root/PBOX.xml");
//    cmd.clear();
//    cmd.append("cp ").append("/mnt/PBox/Config/").append(Code).append(".xml").append(" /root/PBOX.xml ");
//    system(cmd.data());
//    system("sync");
//    QFile file("/root/PBOX.xml");
//    if(!file.open(QFile::ReadOnly | QFile::Text))
//    {
//        usleep(500 * 1000);
//        m_dialog->close();
//        emit UpdateOrExprotError(1);
//    }
//    else
//    {
//        file.close();
//        usleep(500 * 1000);
//        m_dialog->close();
//        emit UpdateOrExprotOk(0);
//    }
//}

/**
 * @brief 接收到更新配置文件指令
 * @param productModel  产品型号
 * @param Code 工艺代码
 */
void PBoxConfig::ReceiveUpdateXML(string productModel,string Code)
{
    string cmd;
    if(opendir("/mnt/PBox") == NULL)
    {
        cmd.append("mount -t cifs -o user=administrator,passwd=123,nounix,noserverino //");
        cmd.append(m_IPaddr);
        cmd.append("/share /mnt/");
        system(cmd.data());
    }

    system("rm ./PBOX.xml");
    cmd.clear();
    QFile file(QString::fromStdString("/mnt/PBox/Config/" + Code + ".xml"));
    if(!file.open(QFile::ReadOnly))
        return ;
    cmd.append("cp ").append("/mnt/PBox/Config/").append(Code).append(".xml").append(" ./PBOX.xml ");
    system(cmd.data());

    system("sync");
    system("reboot");

}

/**
 * @brief 接收到更新源程序指令
 */
void PBoxConfig::ReceiveUpdateSource()
{
    string cmd;
    if(opendir("/mnt/PBox") == NULL)
    {
        cmd.append("mount -t cifs -o user=administrator,passwd=123,nounix,noserverino //");
        cmd.append(m_IPaddr);
        cmd.append("/share /mnt/");
        system(cmd.data());
    }
    string filename = "PBOX";
    cmd.clear();
    cmd.append("cp ").append("/mnt/PBox/bin/").append(filename).append(" ./PBOX ");
    system(cmd.data());
    system("sync");
    system("reboot");
}

/**
 * @brief 接收到导出日志指令
 * @param Code 工艺代码
 */
void PBoxConfig::ReceiveExportLog(string Code)
{
    string command;
    string logname = "./Log/";
    char filename[10] = {0};
    unsigned int flag = 0;
    if(Code.empty())
    {
        Code = "Log";
    }
    string dirname = "./" + Code;
    if(opendir(dirname.data()) != NULL)
    {
        command = "rm " + dirname + " -rf";
        system(command.data());
    }
    for(unsigned int i = 0;i < 10;i++)
    {
        sprintf(filename,"Log%d.txt",i);
        QFile file(QString::fromStdString(logname + filename));
        if(file.open(QFile::ReadOnly | QFile::Text))
        {
            file.close();
            flag++;
        }
    }
    if(flag == 0)
    {
        return ;
    }
    else
    {
        if(Code != "Log")
        {
            command = "cp " + logname + " " + dirname + " -rf";
            system(command.data());
        }
        command.clear();
        command.append("tar -zcvf ").append(Code).append(".tar.gz ").append(dirname).append("/");
        system(command.data());
        command.clear();
        if(opendir("/mnt/PBox") == NULL)
        {
            command.append("mount -t cifs -o user=administrator,passwd=123,nounix,noserverino //");
            command.append(m_IPaddr);
            command.append("/share /mnt/");
            system(command.data());
        }
        command.clear();
        command.append("mv ").append(Code).append(".tar.gz ").append("/mnt/PBox/Log/").append(Code).append("_").append("Log.tar.gz");
        system(command.data());
        system("reboot");
    }
}

/**
 * @brief 接收到导出配置文件指令
 * @param Code 工艺代码
 */
void PBoxConfig::ReceiveExportXML(string Code)
{
    string command;
    if(opendir("/mnt/PBox") == NULL)
    {
        command.append("mount -t cifs -o user=administrator,passwd=123,nounix,noserverino //");
        command.append(m_IPaddr);
        command.append("/share /mnt/");
        system(command.data());
    }
    command.clear();
    string xmlpath = "/mnt/PBox/Config/" + Code + ".xml";
    command.append("cp ").append("./PBOX.xml ").append(xmlpath);
    system(command.data());
    system("reboot");
}

/**
 * @brief 设置导出配置文件按钮使能
 */
//void PBoxConfig::SetExportXmlButtonEnable()
//{
//    if(exportXML->isEnabled())
//    {
//        exportXML->setEnabled(false);
//    }
//    else
//    {
//        exportXML->setEnabled(true);
//    }
//}

/**
 * @brief 设置导出日志文件按钮使能
 */
void PBoxConfig::SetExportLogButtonEnable()
{
    if(m_exportLogBtn->isEnabled())
    {
        m_exportLogBtn->setEnabled(false);
    }
    else
    {
        m_exportLogBtn->setEnabled(true);
    }
}

/**
 * @brief 设置更新配置文件按钮使能
 */
//void PBoxConfig::SetUpdateXmlButtonEnable()
//{
//    if(updateXML->isEnabled())
//    {
//        updateXML->setEnabled(false);
//    }
//    else
//    {
//        updateXML->setEnabled(true);
//    }
//}

/**
 * @brief 设置更新源程序按钮使能
 */
void PBoxConfig::SetUpdateSourceButtonEnable()
{
    if(m_updateSourceBtn->isEnabled())
    {
        m_updateSourceBtn->setEnabled(false);
    }
    else
    {
        m_updateSourceBtn->setEnabled(true);
    }
}

/**
 * @brief 保存网口一IP
 * @param ipaddr
 */
void PBoxConfig::SetEth0IP(string ipaddr)
{
    m_eth0IP = ipaddr;
}

/**
 * @brief 获取网口一IP
 * @return
 */
string PBoxConfig::GetEth0IP()
{
    return m_eth0IP;
}

/**
 * @brief 显示MessageBox
 * @return
 */
bool PBoxConfig::showMessageBox(string text,int buttonstyle)
{
    QMessageBox message;
    message.setFixedSize(500,90);
    QFont font;
    font.setPointSize(18);
    message.setFont(font);
    QPushButton *ok = NULL;
    QPushButton *cancel = NULL;
    bool ret = false;
    switch (buttonstyle)
    {
        case 0:
            {
                cancel = message.addButton(tr("确定"),QMessageBox::ActionRole);
                message.setIcon(QMessageBox::Warning);
            }
            break;
        case 1:
            {
                ok = message.addButton(tr("是"),QMessageBox::ActionRole);
                cancel = message.addButton(tr("否"),QMessageBox::ActionRole);
                message.setIcon(QMessageBox::Question);
            }
            break;
        default:
            break;
    }
    message.move(480,300);
    message.setText(QString::fromStdString(text));
    message.exec();
    if(message.clickedButton() == cancel)
    {
        ret = false;
    }
    else
    {
        if(ok != NULL)
        {
            if(message.clickedButton() == ok)
            {
                ret = true;
            }
        }
    }
    message.close();
    return ret;
}

/**
 * @brief 保存网口二IP
 * @param ipaddr
 */
void PBoxConfig::SetEth1IP(string ipaddr)
{
    m_eth1IP = ipaddr;
}

/**
 * @brief 获取网口二IP
 * @return
 */
string PBoxConfig::GetEth1IP()
{
    return m_eth1IP;
}

/**
 * @brief 保存上位机IP
 * @param ipaddr
 */
void PBoxConfig::SetPCaddr(string ipaddr)
{
    m_IPaddr = ipaddr;
}

/**
 * @brief 获取上位机IP
 * @return
 */
string PBoxConfig::GetPCaddr()
{
    return m_IPaddr;
}

void PBoxConfig::TopKeyboard()
{
    frmInput::Instance()->Init("top", "silvery", 15, 10);
}
void PBoxConfig::BottomKeyboard()
{
    frmInput::Instance()->Init("bottom", "silvery", 15, 10);
}

void PBoxConfig::SetWorkMode()
{
    if(PBoxConfig::m_setWorkMode != -1){
        this->m_workmodecomBox->setCurrentIndex(PBoxConfig::m_setWorkMode);
        if(PBoxConfig::m_setWorkMode == 0)
        {
            m_workmode = 0;
            m_enableSampleMode = 0;
        }
        else
        {
            if(PBoxConfig::m_setWorkMode == 2)
            {
                m_enableSampleMode = 1;
            }
            else
            {
                m_enableSampleMode = 0;
            }
            m_workmode = 1;
        }

        if(m_workmode != m_workmodeshadow)
        {
            m_workmodeshadow = m_workmode;
            if(m_workmodeshadow == 1)
            {
                currEditPartNo = "";
                pthread_mutex_lock(&mutexDeviceDriverCond);
                WorkModeNotify = m_workmodeshadow;
                pthread_mutex_unlock(&mutexDeviceDriverCond);
                pthread_cond_signal(&cond);
            }
            else
            {
                if(WorkModeNotify == 2)
                {
                    pthread_mutex_lock(&mutexDeviceDriverCond);
                    WorkModeNotify = 1;
                    pthread_mutex_unlock(&mutexDeviceDriverCond);
                    pthread_cond_signal(&cond);
                }
                else
                {
                    pthread_mutex_lock(&mutexDeviceDriverCond);
                    WorkModeNotify = m_workmodeshadow;
                    pthread_mutex_unlock(&mutexDeviceDriverCond);
                    pthread_cond_signal(&cond);
                }
            }
            _log.LOG_INFO("生产模式变更：m_workmodeshadow = 【%d】",m_workmodeshadow);
            QProgressDialog *progressDlg=new QProgressDialog(this);
            QFont font;
            font.setPointSize(18);
            progressDlg->setFont(font);
            progressDlg->setFixedHeight(160);
            progressDlg->setWindowModality(Qt::WindowModal);
            progressDlg->setMinimumDuration(0);
            progressDlg->setAttribute(Qt::WA_DeleteOnClose, true);
            progressDlg->setLabelText(tr("正在切换工作模式请等待......"));
            progressDlg->setCancelButton(0);
            progressDlg->setStyleSheet("QProgressBar{border:1px solid #FFFFFF;"
                                                             "height:30;"
                                                             "background:blue;"
                                                             "text-align:center;"
                                                             "color:rgb(255,255,0);"
                                                             "border-radius:10px;}"
                                         "QProgressBar::chunk{"
                                                             "border-radius:0px;"
                                                             "border:0px solid green;"
                                                             "background:green;}"
                                                            );
            progressDlg->setRange(0,13);
            progressDlg->setValue(0);
            int timenum = 0;
            for(;timenum < 13;timenum++)
            {
                sleep(1);
                progressDlg->setValue(timenum + 1);
                if(timenum == 10)
                {
                    break;
                }
            }
            if(WorkModeNotify != m_workmodeshadow)
            {
                pthread_mutex_lock(&mutexDeviceDriverCond);
                WorkModeNotify = m_workmodeshadow;
                pthread_mutex_unlock(&mutexDeviceDriverCond);
                pthread_cond_signal(&cond);
            }
            for(;timenum < 13;timenum++)
            {
                sleep(1);
                progressDlg->setValue(timenum + 1);
            }
            progressDlg->close();
        }

        if(m_enableSampleMode != EnableSampleMode)
        {
            EnableSampleMode = m_enableSampleMode;
            _log.LOG_INFO("样件模式切换完成：EnableSampleMode = 【%d】",EnableSampleMode);
        }

        PBoxConfig::m_setWorkMode = -1;
    }
}
