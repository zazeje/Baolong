#ifndef DISPLAY_H
#define DISPLAY_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QPixmap>
#include <QDesktopWidget>
#include <QApplication>
#include <QTimer>
#include <QPushButton>
#include <QPainter>
#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>
#include <QListView>
#include <QStandardItem>
#include "DataConvert/DataType.h"
#include "LineModel/ProductInfo.h"
#include "PBOXUI/mainwidget.h"
#include "LineDevice/Device/TcpDevice.h"
#include "LineModel/DeviceConfig.h"
#include "UserControl/QMyComboBox.h"

class DeviceDriver;
class mainWidget;
class MyPushButton;
class QMyComboBox;

class display : public QWidget
{

    Q_OBJECT

public:

    display(mainWidget *parent = 0);
    ~display();

    QLabel *m_timeLabel;
    QLabel *m_factoryLabel;
    QLabel *m_productMessage;
    QLabel *m_stationMessage;

    QLabel *m_craftNameLabel;        //工位名称
//    MyPushButton* m_localTest;
    QPushButton* m_localTest;

    QGridLayout *m_craftNameLayout;
    QGroupBox *m_craftNameGroupbox;
    QGridLayout* m_productGridLayout;
    QHBoxLayout *m_mesLabelLayout;
    QVBoxLayout *m_secPartVLayout;
    QGroupBox *m_secPartBox;
    QHBoxLayout *m_tableLabelLayout;
    QHBoxLayout *m_tableWidgetLayout;
    QVBoxLayout *m_thirdPartVLayout;
    QGroupBox *m_thirdPartBox;
    QGroupBox* m_BottomGroup;
    QHBoxLayout *m_BottomHLayout;
    QVBoxLayout *m_wholeVLayout;
    QLabel *m_projectNola;           //产品型号
    QLineEdit *m_projectNoLineEdit;
    QLabel *m_partNo;           //产品件号
    QLineEdit *m_partNoEdit;
    QLabel *m_dispatchNoLabel;           //产品流水号
    QLineEdit *m_dispatchNoLineEdit;
    QMyComboBox *m_dispatchNoComboBox;
    QLabel *m_workModela;          //工作模式
    QLabel *m_currworkMode;
    QLabel *m_countla;          //产品计数
    QLabel *m_count;
    QLabel *m_resultla;         //判定结果
    QLabel *m_result;

    QTableWidget *m_table;
    QTableWidgetItem *item;

    QPushButton *m_return;
    QPushButton *m_restart;

    mainWidget *m_parent;

    QTableWidgetItem *item1st;
    QTableWidgetItem *item2ec;
    QTableWidgetItem *item3rd;

    static string GetProductModel();
    static void SetProductModel(string);
    static string m_productmodel;

    static string GetBillNum();
    static void SetBillNum(string billnum);
    static string m_billnum;

    static void SetUpdateFlag(bool);

    vector<QLabel*> m_comLabelVec;

    vector<QTableWidgetItem *> m_tableNameItem;
    vector<QTableWidgetItem *> m_tableValueItem;

    StationInfo si;
    map<string,DeviceInfo> devInfo;

    QString getLocalTime();

protected:
    void paintEvent(QPaintEvent *);


private slots:

    void updatedata();
    void updatetestdata();
    void returnback();
    void reStart();
    void localDriverStart();
    void updateDispatchInfo(QString dispatchnotmp);
    void PartNoChanged(QString newpartno);
//    void getPartNoString();

private:

    int m_clickedNum;                      //用于100ms的定时器计数，用于延时处理，譬如每计数10次刷新一次当前时间以及启停时设置按键文字与使能状态
    QTimer *timer;                         //用于负责界面生产采集值的刷新工作
    QTimer *statustimer;                   //用于负责对工作模式状态变量，设备驱动启动或停止状态变量进行解析，从而决定界面设备列表的加载、清除，生产启动和停止按钮的使能以及按钮文字更新
    QStandardItemModel* Num;
    QStandardItem* Item;

    int m_startnum;                         //配合来延时计算，点击启动生产按钮后12秒使能停止生产按钮以及点击停止生产按钮后6秒使能启动生产按钮
    bool m_Flag;                            //用于表示当前设备列表框中是否加载了设备列表，为true，当前列表框没有加载设备列表，为false，当前列表框已经加载设备列表完成。

    int m_lastWorkModeNotify;               //上一次工作模式通知码
    bool m_lastdriverStartFlag;             //上一次设备驱动启动或停止状态标识通知码

    static bool m_updateFlag;               //用于控制界面对生产测试时采集值刷新标志变量
    int m_preWorkModeNotify;
    int m_preEnableSampleMode;
};

#endif // DISPLAY_H
