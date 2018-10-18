#ifndef PASSWORDWIDGET_H
#define PASSWORDWIDGET_H

#include <QtGui>
#include "PBOXUI/mainwidget.h"

class PasswordWidget : public QWidget
{

    Q_OBJECT

public:

    PasswordWidget(mainWidget *parent = NULL);
    ~PasswordWidget();
    QLineEdit* m_dialogPassword;
    QPushButton* m_confirmButton;
    QPushButton* m_cancelButton;

    QHBoxLayout* m_dialogLayout1;
    QHBoxLayout* m_dialogLayout2;
    QVBoxLayout* m_dialogLayout3;
    QHBoxLayout* m_dialogLayout4;
    QVBoxLayout* m_dialogLayout5;
    QHBoxLayout* m_dialogLayout6;

    mainWidget* m_parent;

private:

    QLabel* m_dialogLabel;

};



#endif // PASSWORDWIDGET_H
