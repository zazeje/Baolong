#include "TestFrame.h"

/**
 * @brief 设备调试窗口构造
 * @param parent
 */
TestFrame::TestFrame(mainWidget* parent) : m_parent(parent)
{
    QFont ft;
    ft.setPointSize(20);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap("./Image/backgroud1.jpg").scaled(QApplication::desktop()->size())));
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::white);

    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();

    m_topLable = new QLabel(tr("测试界面"));
    m_topLable->setAlignment(Qt::AlignCenter);
    m_topLable->setPalette(pa);
    m_topLable->setFont(ft);
    m_topLayout = new QHBoxLayout();
    m_topLayout->addWidget(m_topLable);
    m_firstGroup = new QGroupBox();
    m_firstGroup->setLayout(m_topLayout);

    ft.setPointSize(12);
    m_comLabel = new QLabel(tr(" COM "));
    m_comLabel->setPalette(pa);
    m_comLabel->setFont(ft);
    m_comBox = new QComboBox();
    m_comBox->setStyleSheet("QComboBox QAbstractItemView::item{height:35px;}");
    m_comBox->setView(new QListView());
    m_comBox->setFixedWidth(width/5);
    m_comBox->addItem(tr("Com1"));
    m_comBox->addItem(tr("Com2"));
    m_comBox->addItem(tr("Com3"));
    m_comBox->addItem(tr("Com4"));
    m_comHLayout = new QHBoxLayout();
    m_comHLayout->addStretch();
    m_comHLayout->addWidget(m_comLabel);
    m_comHLayout->addWidget(m_comBox);
    m_comHLayout->addStretch();

    m_IPaddrLabel = new QLabel(tr(" IP地址 "));
    m_IPaddrLabel->setPalette(pa);
    m_IPaddrLabel->setFont(ft);
    m_IPaddrEdit = new QLineEdit(tr("192.168.10."));
    m_IPaddrEdit->setFixedWidth(width / 5);
    m_IPaddrEdit->setFont(ft);
    m_IPaddrEdit->setPalette(pa);
    m_IPHLayout = new QHBoxLayout();
    m_IPHLayout->addStretch();
    m_IPHLayout->addWidget(m_IPaddrLabel);
    m_IPHLayout->addWidget(m_IPaddrEdit);
    m_IPHLayout->addStretch();

    m_firstHLayout = new QHBoxLayout();
    m_firstHLayout->addLayout(m_comHLayout);
    m_firstHLayout->addStretch();
    m_firstHLayout->addLayout(m_IPHLayout);

    m_baudrateLabel = new QLabel(tr("波特率 "));
    m_baudrateLabel->setPalette(pa);
    m_baudrateLabel->setFont(ft);
    m_baudrateBox = new QComboBox();
    m_baudrateBox->setFixedWidth(width/5);
    m_baudrateBox->setStyleSheet("QComboBox QAbstractItemView::item{height:35px;}");
    m_baudrateBox->setView(new QListView());
    m_baudrateBox->addItem(tr("9600"));
    m_baudrateBox->addItem(tr("14400"));
    m_baudrateBox->addItem(tr("19200"));
    m_baudrateBox->addItem(tr("38400"));
    m_baudrateBox->addItem(tr("56000"));
    m_baudrateBox->addItem(tr("57600"));
    m_baudrateBox->addItem(tr("115200"));
    m_baudrateBox->addItem(tr("256000"));
    m_baudHLayout = new QHBoxLayout();
    m_baudHLayout->addStretch();
    m_baudHLayout->addWidget(m_baudrateLabel);
    m_baudHLayout->addWidget(m_baudrateBox);
    m_baudHLayout->addStretch();

    m_portLabel = new QLabel(tr(" 端  口  "));
    m_portLabel->setPalette(pa);
    m_portLabel->setFont(ft);
    m_portEdit = new QLineEdit();
    m_portEdit->setPalette(pa);
    m_portEdit->setFont(ft);
    m_portEdit->setFixedWidth(width/5);
    m_portHLayout = new QHBoxLayout();
    m_portHLayout->addStretch();
    m_portHLayout->addWidget(m_portLabel);
    m_portHLayout->addWidget(m_portEdit);
    m_portHLayout->addStretch();

    m_secondHLayout = new QHBoxLayout();
    m_secondHLayout->addLayout(m_baudHLayout);
    m_secondHLayout->addStretch();
    m_secondHLayout->addLayout(m_portHLayout);

    m_databitLabel = new QLabel(tr("数据位 "));
    m_databitLabel->setPalette(pa);
    m_databitLabel->setFont(ft);
    m_databitBox = new QComboBox();
    m_databitBox->setPalette(pa);
    m_databitBox->setFont(ft);
    m_databitBox->setStyleSheet("QComboBox QAbstractItemView::item{height:35px;}");
    m_databitBox->setView(new QListView());
    m_databitBox->addItem(tr("8"));
    m_databitBox->addItem(tr("7"));
    m_databitBox->addItem(tr("6"));
    m_databitBox->addItem(tr("5"));
    m_databitBox->addItem(tr("4"));
    m_databitBox->setFixedWidth(width/5);
    m_databitHLayout = new QHBoxLayout();
    m_databitHLayout->addStretch();
    m_databitHLayout->addWidget(m_databitLabel);
    m_databitHLayout->addWidget(m_databitBox);
    m_databitHLayout->addStretch();

    m_stopbitLabel = new QLabel(tr("停 止 位 "));
    m_stopbitLabel->setPalette(pa);
    m_stopbitLabel->setFont(ft);
    m_stopbitBox = new QComboBox();
    m_stopbitBox->setPalette(pa);
    m_stopbitBox->setFont(ft);
    m_stopbitBox->setStyleSheet("QComboBox QAbstractItemView::item{height:35px;}");
    m_stopbitBox->setView(new QListView());
    m_stopbitBox->setFixedWidth(width/5);
    m_stopbitBox->addItem(tr("1"));
    m_stopbitBox->addItem(tr("1.5"));
    m_stopbitBox->addItem(tr("2"));
    m_stopbitHlayout = new QHBoxLayout();
    m_stopbitHlayout->addStretch();
    m_stopbitHlayout->addWidget(m_stopbitLabel);
    m_stopbitHlayout->addWidget(m_stopbitBox);
    m_stopbitHlayout->addStretch();

    m_thirdHLayout = new QHBoxLayout();
    m_thirdHLayout->addLayout(m_databitHLayout);
    m_thirdHLayout->addStretch();
    m_thirdHLayout->addLayout(m_stopbitHlayout);

    m_parityLabel = new QLabel(tr("校验位 "));
    m_parityLabel->setPalette(pa);
    m_parityLabel->setFont(ft);
    m_parityBox = new QComboBox();
    m_parityBox->setPalette(pa);
    m_parityBox->setFont(ft);
    m_parityBox->setStyleSheet("QComboBox QAbstractItemView::item{height:35px;}");
    m_parityBox->setView(new QListView());
    m_parityBox->setFixedWidth(width/5);
    m_parityBox->addItem(tr("None"));
    m_parityBox->addItem(tr("奇校验"));
    m_parityBox->addItem(tr("偶校验"));
    m_parityHLayout = new QHBoxLayout();
    m_parityHLayout->addStretch();
    m_parityHLayout->addWidget(m_parityLabel);
    m_parityHLayout->addWidget(m_parityBox);
    m_parityHLayout->addStretch();

    m_deviceLabel = new QLabel(tr("设备名称 "));
    m_deviceLabel->setPalette(pa);
    m_deviceLabel->setFont(ft);
    m_deviceBox = new QComboBox();
    m_deviceBox->setPalette(pa);
    m_deviceBox->setFont(ft);
    m_deviceBox->setStyleSheet("QComboBox QAbstractItemView::item{height:35px;}");
    m_deviceBox->setView(new QListView());
    m_deviceBox->setFixedWidth(width/5);
    m_deviceHLayout = new QHBoxLayout();
    m_deviceHLayout->addStretch();
    m_deviceHLayout->addWidget(m_deviceLabel);
    m_deviceHLayout->addWidget(m_deviceBox);
    m_deviceHLayout->addStretch();

    m_fourthHLayout = new QHBoxLayout();
    m_fourthHLayout->addLayout(m_parityHLayout);
    m_fourthHLayout->addStretch();
    m_fourthHLayout->addLayout(m_deviceHLayout);

    m_secondVlayout = new QVBoxLayout();
    m_secondVlayout->addLayout(m_firstHLayout);
    m_secondVlayout->addLayout(m_secondHLayout);
    m_secondVlayout->addLayout(m_thirdHLayout);
    m_secondVlayout->addLayout(m_fourthHLayout);
    m_secondGroup = new QGroupBox();
    m_secondGroup->setLayout(m_secondVlayout);

    m_infoLabel = new QLabel(tr("测试信息"));
    m_infoLabel->setPalette(pa);
    m_infoLabel->setFont(ft);
    m_infoEdit = new QTextEdit();
    m_infoEdit->setPalette(pa);
    m_infoEdit->setFont(ft);
    m_infoEdit->setFixedSize(width * 96 / 100,height * 7 / 18);
    m_fifthVLayout = new QVBoxLayout();
    m_fifthVLayout->addWidget(m_infoLabel);
    m_fifthVLayout->addWidget(m_infoEdit);

    m_thirdGroup = new QGroupBox();
    m_thirdGroup->setLayout(m_fifthVLayout);

    m_testButton = new QPushButton(tr("  测  试  "));
    m_testButton->setFont(ft);
    m_testButton->setFixedSize(width / 8,height / 15);
    connect(m_testButton,SIGNAL(clicked()),this,SLOT(dotest()));
    m_backButton = new QPushButton(tr("  返  回  "));
    m_backButton->setFont(ft);
    m_backButton->setFixedSize(width / 8,height / 15);
    connect(m_backButton,SIGNAL(clicked()),this,SLOT(doback()));

    m_sixHLayout = new QHBoxLayout();
    m_sixHLayout->addStretch();
    m_sixHLayout->addWidget(m_testButton);
    m_sixHLayout->addStretch();
    m_sixHLayout->addWidget(m_backButton);
    m_sixHLayout->addStretch();

    m_fourthGroup = new QGroupBox();
    m_fourthGroup->setLayout(m_sixHLayout);

    m_totalLayout = new QVBoxLayout();
    m_totalLayout->addWidget(m_firstGroup);
    m_totalLayout->addWidget(m_secondGroup);
    m_totalLayout->addWidget(m_thirdGroup);
    m_totalLayout->addWidget(m_fourthGroup);

    connect(m_parent,SIGNAL(addDevItems()),this,SLOT(addItem()));

    this->setWindowOpacity(1);      //设置透明度
    this->setFont(ft);
    this->setFixedSize(width,height); //设置窗体大小为屏幕大小
    this->setAutoFillBackground(true);
    this->setLayout(m_totalLayout);
}

/**
 * @brief 析构函数
 */
TestFrame::~TestFrame()
{
    delete m_deviceBox;
    delete m_topLable;
    delete m_firstGroup;
    delete m_topLayout;

    delete m_comLabel;
    delete m_comBox;
    delete m_comHLayout;
    delete m_IPaddrLabel;
    delete m_IPaddrEdit;
    delete m_IPHLayout;
    delete m_firstHLayout;

    delete m_baudrateLabel;
    delete m_baudrateBox;
    delete m_baudHLayout;
    delete m_portLabel;
    delete m_portEdit;
    delete m_portHLayout;
    delete m_secondHLayout;

    delete m_databitLabel;
    delete m_databitBox;
    delete m_databitHLayout;
    delete m_stopbitLabel;
    delete m_stopbitBox;
    delete m_stopbitHlayout;
    delete m_thirdHLayout;

    delete m_parityLabel;
    delete m_parityBox;
    delete m_parityHLayout;

    delete m_deviceLabel;
    delete m_deviceHLayout;
    delete m_fourthHLayout;

    delete m_secondVlayout;
    delete m_secondGroup;

    delete m_infoLabel;
    delete m_infoEdit;
    delete m_fifthVLayout;
    delete m_thirdGroup;

    delete m_testButton;
    delete m_backButton;
    delete m_sixHLayout;
    delete m_fourthGroup;

    delete m_totalLayout;

    delete m_parent;
}

/**
 * @brief 向设备下拉框列表添加设备信息
 */
void TestFrame::addItem()
{
//    m_deviceBox->clear();
//    for(int i = 0;i < GetInfo::deviceMessage.count();i++)
//    {
//        m_deviceBox->addItem(GetInfo::deviceMessage.at(i).devicename);
//    }
}

/**
 * @brief 返回按钮点击触发函数
 */
void TestFrame::doback()
{
    this->close();
    m_parent->showUsrchosewidget();
}

/**
 * @brief 测试按钮点击触发函数
 */
void TestFrame::dotest()
{
    QString Port = m_portEdit->text();
    QString devName = m_deviceBox->currentText();
    QString text;
    if(Port.isEmpty())
    {
        string portname = StringToUpper(m_comBox->currentText().toStdString());
        string baudrate = m_baudrateBox->currentText().toStdString();
        string databit = m_databitBox->currentText().toStdString();
        string parity = m_parityBox->currentText().toStdString();
        string stopbit = m_stopbitBox->currentText().toStdString();
        if(parity == "None")
        {
            parity = "0";
        }
        else if(parity == "奇校验")
        {
            parity = "1";
        }
        else if(parity == "偶校验")
        {
            parity = "2";
        }
        SerialPortParameters* spp = new SerialPortParameters(portname,baudrate,databit,stopbit,parity);
//        GetInfo m_info;
//        int devID = atoi(m_info.getdevID(devName).toStdString().data());
//        ComDevice* myDevice;
//        switch(devID)
//        {
//            case 2: myDevice = new DataManSR(spp,devName.toStdString());break;
//            case 3: myDevice = new CycloneProgram(spp,devName.toStdString());break;
//            case 4: myDevice = new FxPlc(spp,devName.toStdString(),dedicatedProtocol);break;
//            case 8: case 10: myDevice = new AgilentMultimeterSerial(spp,devName.toStdString());break;
//            case 12: myDevice = new XSE6(spp,devName.toStdString());break;
//            case 18: myDevice = new TpmsHandle(spp,devName.toStdString());break;
//            case 19: myDevice = new LightAndRainDevice(spp,devName.toStdString());break;
//        }
//        if(myDevice->CanAcess())
//        {
//            text = "与设备【" + devName + "】连接成功，通信测试成功！";
//        }
//        else
//        {
//            text = "与设备【" + devName + "】连接失败，通信测试失败！";
//        }
//        delete myDevice;
//        myDevice = NULL;
//    }
//    else
//    {
//        bool canAcessResult = false;
//        string IP = m_IPaddrEdit->text().toStdString();
//        int port = atoi(m_portEdit->text().toStdString().data());
//        TcpDevice* myTcpDevice;
//        GetInfo m_info;
//        int devID = atoi(m_info.getdevID(devName).toStdString().data());
//        switch(devID)
//        {
//            case 1: myTcpDevice = new KeyenceSR(port,IP,devName.toStdString());break;
//            //case 6: myTcpDevice = new DagongPlc(port,IP,devName.toStdString());break;
//            case 7: myTcpDevice = new LaserMarker(port,IP,devName.toStdString());break;
//            case 8: case 10:myTcpDevice = new AgilentMultimeterTcp(port,IP,devName.toStdString());break;
//            case 16:myTcpDevice = new AgilentN9010A(port,IP,devName.toStdString());break;
//            case 17:myTcpDevice = new RohdeSchwarz(port,IP,devName.toStdString());break;
//        }
//        if(devID != 3)
//        {
//            canAcessResult = myTcpDevice->CanAcess();
//            delete myTcpDevice;
//            myTcpDevice = NULL;
//        }
//        else
//        {
//            UdpDevice* myUdpDevice = new CycloneProUdp(port,IP,devName.toStdString());
//            canAcessResult = myUdpDevice->CanAcess();
//            delete myUdpDevice;
//            myUdpDevice = NULL;
//        }
//        if(canAcessResult)
//        {
//            text = "与设备【" + devName + "】连接成功，通信测试成功！";
//        }
//        else
//        {
//            text = "与设备【" + devName + "】连接失败，通信测试失败！";
//        }
//    }
//    m_infoEdit->setText(text);
}
}

