#ifndef PLCPOINTBASE_H
#define PLCPOINTBASE_H

#include <QWidget>
#include <QObject>
#include <QApplication>
#include <QLayout>
#include <vector>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QScrollBar>
#include <QMessageBox>
#include <QtWidgets/qheaderview.h>
#include <map>
#include <string>
#include <algorithm>
#include <pthread.h>
#include <unistd.h>
#include "DataConvert/DataType.h"
#include "LineDevice/PlcDevice/PlcPointInfo.h"
#include "PlcValue.h"
#include "LineDevice/PlcDevice/Fx5U.h"
#include "LineDevice/PlcDevice/FxPlc.h"

//extern LineInfo gLine;

class MyPushButton;

//PLC界面基类
class PlcPointBase : public QWidget
{
    Q_OBJECT

public:

    explicit PlcPointBase();
    ~PlcPointBase();

    QTimer* m_timer;

    int m_currentPageIndex;

    static PlcBase* m_device;

    static void* CreateThread(void*);
    static void SetPlcValueVec(vector<PlcValue> pv);
    static void GetPlcValueVec(vector<PlcValue> &pv);

    static void SetReadPlcFlag(bool flag);
    static void SetDisplayType(int type);

    void UpdateDisplayItem(int pNum);

    string GetCycleTime();
    string GetProductCount();

protected:

//    static pthread_rwlock_t m_lock;

    int m_wholePageNum;             //表格页数
    int m_currentPageItems;         //每页显示条目数
    int m_widgetType;               //
    int m_tableRowCount;            //表格总行数
    bool m_pageModifyFlag;              //当前页修改标志
    bool m_wholeModifyFlag;             //整体修改标志

    vector<int> m_deleteNumVec;

    QString m_title;

    vector<PlcPointInfo> m_pointInfoVec;

    QLabel* m_titleLabel;
    QHBoxLayout* m_titleHLayout;
    QGroupBox* m_titleGroup;
    QVBoxLayout* m_titleVLayout;

    QPushButton* m_previousPage;
    QPushButton* m_nextPage;
    QLabel* m_pageMessage;
    QLabel* m_currentpageLabel;
    QLabel* m_spiltLabel;
    QLabel* m_totalpageLabel;
    QLabel* m_pageLabel;
    QHBoxLayout* m_pageLayout;
    QHBoxLayout* m_pageHBoxLayout;
    QTableWidget* m_tableWidget;
    QPushButton* m_returnButton;
    QHBoxLayout* m_returnLayout;

    QVBoxLayout* m_pageVBoxLayout;
    QGroupBox* m_pageGroupBox;
    QVBoxLayout* m_vBoxLayout;
    QVBoxLayout* m_wholeLayout;
    QGroupBox* m_mainPartBox;

    map<string,int> m_plcValue;
    QList<QTableWidgetItem *> m_tableItem;
    vector<QTableWidgetItem *> m_tableAlarmItem;

    static FxPlc* m_plcDevice;

    void UpdatePageMessage();
    void UpdatePlcValue();

    bool GetCommunicateState();

    virtual void UpdatePlcPoint() = 0;
    virtual void DeleteTableWidgetItem() = 0;


protected slots:

    virtual void GoToNextPage();
    virtual void GoToPreviousPage();
    virtual void ReturnSlots() = 0;
//    virtual void CheckPlcInfo() = 0;
//    void ShowTableWidgetSlot();

public:

    static vector<PlcValue> m_pv;
    static bool m_readFlag;
    static int m_plcDisplayType;

    int m_cycleTime;
    int m_productCount;
    bool m_communicateState;
    void ReadPlcDisplayValue();

};

//自定义按钮类
class MyPushButton : public QPushButton
{
    Q_OBJECT

public:

    MyPushButton(QString buttonText);
    void SetMyButtonNum(int num);
    int GetMyButtonNum();
signals:

    void MyButtonClicked(int);

public slots:

    void EmitClickedByNum();
    void ChangeColor();

private:

    bool m_clickedNum;
    int m_buttonNum;

};

#endif // PLCPOINTBASE_H
