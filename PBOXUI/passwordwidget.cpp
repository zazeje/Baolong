#include "passwordwidget.h"

/**
 * @brief 密码输入窗口
 * @param parent
 */
PasswordWidget::PasswordWidget(mainWidget *parent) : m_parent(parent)
{
    QFont ft_g;
    ft_g.setPointSize(12);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::white);
    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();
    this->setFixedSize(width,height);
    m_dialogLabel = new QLabel(tr("请输入密码"));
    m_dialogLabel->setFont(ft_g);
    m_dialogLabel->setPalette(pa);
    m_dialogPassword = new QLineEdit();
    m_dialogPassword->setFont(ft_g);
    m_dialogPassword->setPalette(pa);
    m_dialogPassword->setFocusPolicy(Qt::StrongFocus);
    m_dialogLayout1 = new QHBoxLayout();
    m_dialogLayout1->addStretch();
    m_dialogLayout1->addWidget(m_dialogLabel);
    m_dialogLayout1->addStretch();
    m_dialogLayout2 = new QHBoxLayout();
    m_dialogLayout2->addStretch();
    m_dialogLayout2->addWidget(m_dialogPassword);
    m_dialogLayout2->addStretch();
    m_dialogLayout3 = new QVBoxLayout();
    m_dialogLayout3->addStretch();
    m_dialogLayout3->addLayout(m_dialogLayout1);
    m_dialogLayout3->addStretch();
    m_dialogLayout3->addLayout(m_dialogLayout2);
    m_dialogLayout3->addStretch();
    m_confirmButton = new QPushButton(tr("确定"));
    m_confirmButton->setFixedWidth(width/8);
    m_cancelButton = new QPushButton(tr("取消"));
    m_cancelButton->setFixedWidth(width/8);
    m_dialogLayout4 = new QHBoxLayout();
    m_dialogLayout4->addStretch();
    m_dialogLayout4->addWidget(m_confirmButton);
    m_dialogLayout4->addStretch();
    m_dialogLayout4->addWidget(m_cancelButton);
    m_dialogLayout4->addStretch();
    m_dialogLayout5 = new QVBoxLayout();
    m_dialogLayout5->addStretch();
    m_dialogLayout5->addLayout(m_dialogLayout3);
    m_dialogLayout5->addStretch();
    m_dialogLayout5->addLayout(m_dialogLayout4);
    m_dialogLayout5->addStretch();
    m_dialogLayout5->setAlignment(Qt::AlignCenter);
    m_dialogLayout6 = new QHBoxLayout();
    m_dialogLayout6->addStretch();
    m_dialogLayout6->addLayout(m_dialogLayout5);
    m_dialogLayout6->addStretch();
    m_dialogLayout6->setAlignment(Qt::AlignCenter);
    this->setLayout(m_dialogLayout6);
    this->setAutoFillBackground(true);
}

/**
 * @brief 析构函数
 */
PasswordWidget::~PasswordWidget()
{
    delete m_dialogPassword;
    delete m_confirmButton;
    delete m_cancelButton;
    delete m_dialogLabel;
    delete m_dialogLayout1;
    delete m_dialogLayout2;
    delete m_dialogLayout3;
    delete m_dialogLayout4;
    delete m_dialogLayout5;
    delete m_dialogLayout6;
}
