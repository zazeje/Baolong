#ifndef PLCSTATE_H
#define PLCSTATE_H

#include <QWidget>
#include <QObject>
#include <vector>
#include <QTimer>
#include "PlcPointBase.h"
#include "LineDevice/PlcDevice/PlcPointInfo.h"
#include "DataConvert/DataType.h"
#include "DataHelper/ParseCsv.h"
#include "PBOXUI/mainwidget.h"

extern vector<PlcPointInfo> PlcDisplayVec;

class MyPushButton;

class StateWidgetParts : public QWidget
{
    Q_OBJECT
public:

    StateWidgetParts();
    ~StateWidgetParts();

    QLabel * m_label;
    int m_lastState;
signals:

    void ChangeStateSignal(bool);

public slots:

    void ChangeStateSlot(bool state);

};



class PlcState : public PlcPointBase
{
    Q_OBJECT

public:
    explicit PlcState(mainWidget *parent = NULL);
    ~PlcState();

    void autoCreatTable();
    void displayTable();
    QTimer *m_timer;

//    QImage* m_redImage;
//    QImage* m_greenImage;

protected:

    QGroupBox *m_tableGroupBox;
    QHBoxLayout *m_tableHBoxLayout;
    QTableWidgetItem *m_PointNameItem;
    QTableWidgetItem *m_RegisterAddrItem;
    vector<PlcPointInfo> m_PlcDisplayVec;    
    vector<QTableWidget*> tableWidget;
    int m_tableNum;
    static int m_pageNum;

    vector<QLabel*> m_dotLableVec;

    int m_width;
    int m_height;

    mainWidget *m_parent;

    void UpdatePlcPoint();
    void DeleteTableWidgetItem();

public slots:

    void ReturnSlots();
    void UpdatePlcState();

private:

    vector<StateWidgetParts*> m_partsInfoVec;
    vector<int> m_lastValue;

};

#endif // PLCSTATE_H
