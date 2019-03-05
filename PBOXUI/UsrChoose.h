#ifndef USRCHOOSE_H
#define USRCHOOSE_H

#include<QWidget>
#include<QLabel>
#include<QPushButton>
#include<QGridLayout>
#include<QDesktopWidget>
#include<QSplitter>
#include<QApplication>
#include <cstdlib>
#include <QTranslator>
#include <QBoxLayout>
#include "mainwidget.h"
#include "LineModel/ConfigLoader.h"
#include "LineDriver/DeviceDriver.h"
#include <QInputDialog>

class mainWidget;
class DeviceDriver;
class Dialog;

class UsrChoose : public QWidget
{

    Q_OBJECT

public:

    UsrChoose(mainWidget *parent = NULL);
    ~UsrChoose();

    QLabel *m_choseLabel;
    QPushButton *m_pboxSettingButton;
    QPushButton *m_displayButton;
    QPushButton *m_simulatorButton;
    QPushButton *m_plcStateButton;
    QVBoxLayout *m_contentVBoxLayout;

private slots:

    void pboxSetting();
    void dataDisplay();
//    void testDisplay();
    void simulatorDisplay();
    void plcDisplay();

private:

    mainWidget *m_parent;
    bool startflag;

};
#endif // USRCHOOSE_H
