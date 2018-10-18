#ifndef PLCADDITEMDIALOG_H
#define PLCADDITEMDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QLayout>
#include <QGroupBox>
#include <qpushbutton.h>
#include "LineDevice/PlcDevice/PlcPointInfo.h"

class PlcAddItemDialog : public QDialog
{

    Q_OBJECT

public:

    explicit PlcAddItemDialog();
    ~PlcAddItemDialog();

    QLineEdit* m_pointNameEdit;
    QLineEdit* m_registerAddrEdit;

signals:

    void SubmitNewPlcPoint(PlcPointInfo& info);

public slots:

    void ConfirmButtonSlots();
    void CancleButtonSlots();

private:

    QLabel* m_pointNameLabel;

    QLabel* m_pointTypeLabel;
    QComboBox* m_pointTypeEdit;
    QLabel* m_registerTypeLabel;
    QComboBox* m_registerTypeEdit;
    QLabel* m_registerAddrLabel;
    QLabel* m_pointCoeffLabel;
    QLineEdit* m_pointCoeffEdit;
    QLabel* m_pointUnitLabel;
    QLineEdit* m_pointUnitEdit;


    QHBoxLayout* m_lineLayout;
    QHBoxLayout* m_typeLayout;
    QHBoxLayout* m_coeffLayout;
    QGroupBox* m_editBox;
    QVBoxLayout* m_groupLayout;

    QPushButton* m_confirmButton;
    QPushButton* m_cancleButton;
    QHBoxLayout* m_buttonLayout;
    QGroupBox* m_buttonGroup;
    QVBoxLayout* m_wholeLayout;

};

#endif // PLCADDITEMDIALOG_H
