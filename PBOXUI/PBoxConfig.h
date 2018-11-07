#ifndef PBOXCONFIG_H
#define PBOXCONFIG_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QLineEdit>
#include <QFile>
#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <QTextStream>
#include "mainwidget.h"
#include <QProgressDialog>

class MyLineEdit;

using namespace std;

class PBoxConfig : public QWidget
{
    Q_OBJECT

public:

    PBoxConfig(mainWidget* parent = NULL);
    ~PBoxConfig();

    MyLineEdit* m_eth0Edit;
    MyLineEdit* m_eth1Edit;

    static QString m_productModel;
    static QString m_craftName;
    static int m_setWorkMode;

    void SetEth0IP(string);
    string GetEth0IP();
    void SetEth1IP(string);
    string GetEth1IP();
    bool showMessageBox(string text,int buttonstyle=0);

    static void SetPCaddr(string);
    static string GetPCaddr();

    static void ReceiveUpdateXML(string productModel, string craftCode);
    static void ReceiveUpdateSource();
    static void ReceiveExportLog(string craftCode);
    static void ReceiveExportXML(string craftCode);
    static void SetExportXmlButtonEnable();
    static void SetExportLogButtonEnable();
    static void SetUpdateXmlButtonEnable();
    static void SetUpdateSourceButtonEnable();
    static bool m_checkJRFlag;
    bool m_checkJRFlagShadow;

private:

    QTimer *checkSetWorkMode;
    QHBoxLayout* m_ipLayout1;
    mainWidget* m_parent;
    QLabel* m_tileLabel;
    QHBoxLayout* m_titleLayout;
    QGroupBox* m_firstGroup;
//    modify by wjl 2017/11/23 start
    QGridLayout* m_secondGridLayout;
    QGridLayout* m_ipGridLayout;
    QLabel* m_eth0Label;
    QLabel* m_eth1Label;
    QLabel* m_workCenterNoLabel;
    QLineEdit* m_workCenterNoLE;
    QLabel* m_workModeLabel;
    QComboBox *m_workmodecomBox;
    QLabel *m_checkPreJRLabel;
    QComboBox *m_checkPreJRCombox;
    QLabel *m_productModelLabel;
    QLineEdit *m_productModelLineEdit;
    QProgressDialog *m_dialog;
    QPushButton* m_confirmButton;
    QPushButton* m_cancelButton;
    QHBoxLayout *m_buttonLayout;
    QGroupBox* m_thirdGroup;
    QVBoxLayout* m_forthLayout;
    QGroupBox* m_forthGroup;
    QVBoxLayout* m_fifthLayout;
    QGroupBox* m_fifthGroup;
    QVBoxLayout* m_wholeLayout;
    string m_eth0IP;
    string m_eth1IP;
    int m_workmode;
    int m_workmodeshadow;
    int m_enableSampleMode;

    static string m_IPaddr;
    static QPushButton* m_exportLogBtn;
    static QPushButton* m_updateSourceBtn;
    QGroupBox* m_secondGroup;

    void processLog();
    void processXML();
    void processexportXML();
    void processSource();
    static void* processExportLog(void*);
    static void* processExportXML(void*);
    static void* processUpdateXML(void* arg);
    static void* processUpdateSouece(void*);

signals:

    void UpdateOrExprotErrorSignal(int);
    void UpdateOrExprotOkSignal(int);

private slots:
    void SetWorkMode();
    void ExportLog();
    void UpdateSource();
    void ConfirmPress();
    void CancelPress();
    void UpdateOrExportOkSlot(int);
    void UpdateOrExportErrorSlot(int flag);
    void TopKeyboard();
    void BottomKeyboard();
    void checkPreJR(QString);

};

#endif //PBOXCONFIG_H
