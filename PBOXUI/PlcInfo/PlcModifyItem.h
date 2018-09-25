#ifndef PLCMODIFYITEM_H
#define PLCMODIFYITEM_H

#include "PlcPointBase.h"
#include "DataHelper/ParseCsv.h"
#include "PlcAddItemDialog.h"
#include "PBOXUI/mainwidget.h"
#include "PlcValue.h"
#include "algorithm"

extern vector<PlcPointInfo> PlcWholeInfoVec;

class ModifyItemWidgetParts : public QObject
{
    Q_OBJECT

public:

    ModifyItemWidgetParts();
    ~ModifyItemWidgetParts();

    QIcon m_unselectIcon;
    QIcon m_selectedIcon;
    QLabel* m_pointNum;
    QPushButton* m_pointCheckBox;
    QHBoxLayout* m_checkBoxLayout;
    QGroupBox* m_checkBoxGroup;

    QLineEdit* m_pointNameEdit;
    QHBoxLayout* m_nameLayout;
    QWidget* m_nameWidget;

    QComboBox* m_pointTypeEdit;
    QHBoxLayout* m_pTypeLayout;
    QWidget* m_pTypeWidget;

    QComboBox* m_registerTypeEdit;
    QHBoxLayout* m_rTypeLayout;
    QWidget* m_rTypeWidget;

    QLineEdit* m_registerAddrEdit;
    QHBoxLayout* m_rAddrLayout;
    QWidget* m_rAddrWidget;

    QLineEdit* m_pointCoefficientEdit;
    QHBoxLayout* m_pointCoefficientLayout;
    QWidget* m_pointCoefficientWidget;

    QLineEdit* m_pointUnitEdit;
    QHBoxLayout* m_pointUnitLayout;
    QWidget* m_pointUnitWidget;

    bool isChecked();

signals:

    void isModified(QString);

public slots:

    void ChangeState();

private:

    bool m_checkFlag;

};

class PlcModifyItem : public PlcPointBase
{
    Q_OBJECT
public:

    PlcModifyItem(mainWidget* parent = 0);
    ~PlcModifyItem();

    void UpdatePlcPoint();

signals:

    void PlcInfoIsChanged();

public slots:

    void GoToNextPage();
    void GoToPreviousPage();
    void AddPointInfo();
    void DeletePointInfo();
    void ReceivePointInfo(PlcPointInfo &info);
    void ConfirmSlots();
    void ReturnSlots();
    void isModified(QString str);
    void SortPlcPoint();

private:

//    bool m_pageModifyFlag;              //当前页修改标志
//    bool m_wholeModifyFlag;             //整体修改标志

    int m_deleteIndex;                  //删除索引
    int m_deleteOffset;                 //删除偏移

    mainWidget* m_parent;

    PlcAddItemDialog* m_addDialog;

    QPushButton* m_addButton;
    QPushButton* m_deleteButton;
    QPushButton* m_confirmButton;

    vector<ModifyItemWidgetParts*> m_partsInfoVec;

    void DeleteTableWidgetItem();
    PlcValue GetPvFromVec(vector<int> vec,int itemType,string registerType);

};

#endif // PLCMODIFYITEM_H
