#ifndef PLCDISPLAY_H
#define PLCDISPLAY_H

#include <QWidget>
#include <QObject>
#include <vector>
#include <QTimer>
#include "PlcPointBase.h"
#include "LineDevice/PlcDevice/PlcPointInfo.h"
#include "DataHelper/LogHelper.h"
#include "PBOXUI/mainwidget.h"

extern vector<PlcPointInfo> PlcAlarmVec;
extern vector<PlcPointInfo> PlcStateVec;

#define RowNum 100

class PlcDisplay : public PlcPointBase
{
    Q_OBJECT
public:

    explicit PlcDisplay(mainWidget *parent = NULL);
    ~PlcDisplay();

    QTimer *m_timer;

protected:

    QLabel *m_timeLabel;

    QGroupBox *m_pageGroupBox;

    QLabel *m_judgestateLabel;
    QLabel *m_judgeLabel;

    QLabel *m_devicestateLabel;
    QLabel *m_stateLabel;
    QGroupBox *m_pagecontentGroupBox;

    QLabel *m_rhythmLabel;
    QLabel *m_rhythmValueLabel;
    QLabel *m_rhythmUnitLabel;
    QHBoxLayout *m_rhythmHLayout;

    QLabel *m_productcountLabel;
    QLabel *m_countLabel;
    QPushButton *m_countclearButton;
    QGridLayout *m_stateGridLayout;
    QVBoxLayout *m_stateVBoxLayout;

    QPushButton *m_settingButton;
    QPushButton *m_manualButton;
    MyPushButton *m_startButton;
    MyPushButton *m_resetButton;
    MyPushButton *m_stopButton;
    QPushButton *m_stateButton;
    QPushButton *m_modifyButton;
    QHBoxLayout *m_buttonHLayout;
    QHBoxLayout *m_buttonHLayout_1;
    QVBoxLayout *m_buttonVLayout;
    QGroupBox *m_alarmGroupBox;
    QHBoxLayout *m_alarmHBoxLayout;
    QHBoxLayout *m_secondHBoxLayout;

    QTimer *m_clearButtonTimer;
    QTimer *m_startButtonTimer;
    QTimer *m_stopButtonTimer;
    QTimer *m_resetButtonTimer;

    int m_clearNum;
    int m_startNum;
    int m_stopNum;
    int m_resetNum;

    QString getLocalTime();

    mainWidget *m_parent;
    void updateAlarmInfo();
    vector<int> m_alarmRelease;
    vector<int> m_preValue;
    void UpdatePlcPoint();
    void DeleteTableWidgetItem();


public slots:

    void updateData();
    void clearProductcount();
    void setParameter();
    void manualSetting();
    void startPlc();
    void resetPlc();
    void stopPlc();
    void returnTo();
    void stateDisplay();
    void excelModify();
    void enableClearButton();
    void enableStartButton();
    void enableStopButton();
    void enableResetButton();
    void ReturnSlots();
};

#endif // PLCDISPLAY_H
