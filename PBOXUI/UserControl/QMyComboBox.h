#ifndef QMYCOMBOBOX_H
#define QMYCOMBOBOX_H
#include <QComboBox>

class QMyComboBox : public QComboBox
{
    Q_OBJECT
public:
    QMyComboBox();
    ~QMyComboBox();
    void showPopup();
//    void focusInEvent(QFocusEvent *event);
//signals:
//    void getCurrPartNo();
};

#endif // QMYCOMBOBOX_H
