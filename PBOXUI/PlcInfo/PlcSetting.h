#ifndef PLCSETTING_H
#define PLCSETTING_H

#include "PlcPointBase.h"
#include "PBOXUI/mainwidget.h"
#include "LineDevice/PlcDevice/PlcPointInfo.h"

extern vector<PlcPointInfo> PlcSettingVec;
extern vector<PlcPointInfo> PlcDDisplayVec;

class MyPushButton;

class ParameterWidgetParts
{

public:

    ParameterWidgetParts();
    ~ParameterWidgetParts();

    QLineEdit* m_parameterEdit;
    QLabel* m_unitLabel;
    QHBoxLayout* m_editHLayout;
    QHBoxLayout* m_unitHLayout;
    QHBoxLayout* m_parameterHLayout;
    QVBoxLayout* m_parameterVLayout;
    QWidget* m_parameterBox;

    MyPushButton* m_confirmButton;
    QHBoxLayout* m_buttonHLayout;
    QVBoxLayout* m_buttonLayout;
    QGroupBox* m_buttonBox;

};

class PlcSetting : public PlcPointBase
{
    Q_OBJECT

public:

    explicit PlcSetting(mainWidget *parent = 0);
    ~PlcSetting();

public slots:

    void PlcSetParameter(int);
    void ReturnSlots();
    void ModifySlots();
    void CheckPlcInfo();

private slots:

    void UpdateMachineValue();

private:

    vector<ParameterWidgetParts*> m_partsInfoVec;

    mainWidget* m_parent;
    QPushButton* m_modifyButton;

    void SetParameterEditEnable(bool flag);

    void UpdatePlcPoint();
    void DeleteTableWidgetItem();

//    int GetPlcValue(string registerType, int registerAddr);

};

#endif // PLCSETTING_H
