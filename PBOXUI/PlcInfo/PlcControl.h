#ifndef PLCCONTROL_H
#define PLCCONTROL_H

#include "PlcPointBase.h"
#include "PBOXUI/mainwidget.h"
#include "DataHelper/LogFile.h"

class QHeaderView;

extern vector<PlcPointInfo> PlcControlVec;

//表格中每一行所包含的部件集合
class ControlWidgetParts : public QWidget
{
    Q_OBJECT
public:

    ControlWidgetParts();
    ~ControlWidgetParts();

    QLabel* m_stateLabel;
    QHBoxLayout* m_stateHLayout;
    QGroupBox* m_stateBox;

    MyPushButton* m_setButton;
    MyPushButton* m_pulseButton;
    QGroupBox* m_box;
    QHBoxLayout* m_layout;

    QTimer* m_timer;

    bool m_plcState;
    int m_clickNum;

public slots:

    void EnableButton();

};


class PlcControl : public PlcPointBase
{
    Q_OBJECT

public:

    explicit PlcControl(mainWidget* parent = 0);
    ~PlcControl();

public slots:

    void PlcChangeState(int);
    void PlcSetPulse(int);
    void ReturnSlots();
    void CheckPlcInfo();

private:

    bool plcPointState;

    vector<ControlWidgetParts*> m_partsInfoVec;

    mainWidget* m_parent;

    void UpdatePlcPoint();
    void DeleteTableWidgetItem();

};

#endif // PLCCONTROL_H
