#ifndef MAINWIDGET
#define MAINWIDGET

#include <QWidget>
#include <QStackedWidget>
#include <QMessageBox>
#include <stdlib.h>
#include "UsrChoose.h"
#include "display.h"
#include "PBoxConfig.h"
#include "TestFrame.h"
#include "passwordwidget.h"
#include "LineModel/ConfigLoader.h"
#include "DataConvert/DataType.h"
#include "PBOXUI/PlcInfo/PlcDisplay.h"
#include "PBOXUI/PlcInfo/PlcState.h"
#include "frminput.h"
#include "PlcInfo/PlcSetting.h"
#include "PlcInfo/PlcControl.h"
#include "PlcInfo/PlcModifyItem.h"


class PlcControl;
class PlcSetting;
class PlcModifyItem;
class UsrChoose;
class display;
class PBoxConfig;
class TestFrame;
class PasswordWidget;
class PlcDisplay;
class PlcState;
class PasswordWidget;

class mainWidget : public QWidget
{
    Q_OBJECT

public:

    mainWidget();
    ~mainWidget();

    QStackedWidget *m_stackwidget;
    PasswordWidget *m_passwidget;
    UsrChoose *m_usrchosewidget;
    display *m_displaywidget;
    PBoxConfig *m_config;
    TestFrame* m_testwidget;
    PlcDisplay *m_plcDisplay;
    PlcState *m_plcState;
    PlcControl* m_plcControl;
    PlcSetting* m_plcSetting;
    PlcModifyItem* m_plcModify;

    int widgetFlag;


signals:

    void checkControlType(const QString &);
    void addDevItems();
//    void UpdatePlcInfo();

public slots:

    void showUsrchosewidget();
    void showDisplaywidget();
//    void showTestWidget();
    void slot_pboxconfigclicked();
    void slot_configclicked();
    void PasswordWidgetConfirm();
    void PasswordWidgetCancel();
    void showPBoxConfig();
    void showPlcControl();
    void showPlcSetting();
    void showPlcModifyItem();

    void showPlcDisplay();
    void showPlcState();

private:

    int startnum;
    void ReadPBOXIP();

};

class MyLineEdit : public QLineEdit
{

    Q_OBJECT

public:

    MyLineEdit(QWidget* parent = 0);
    ~MyLineEdit();

protected:

    virtual void focusInEvent(QFocusEvent *event);

signals:

    void clicked();

};


#endif // MAINWIDGET

