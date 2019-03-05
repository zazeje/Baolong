#ifndef SIMULATORWIDGET_H
#define SIMULATORWIDGET_H

#include <QWidget>
#include "mainwidget.h"
#include <QTimer>
#include "LineDevice/Cnet100Device/Cnet100tcp.h"

class Target{
public:
    char enable;
    char chipNumber;
    char outMode;
    char unConnectCount;
    float SNR;
    float range;
    float velocity;
    float angle;
    Target(){
        enable = 0;
        chipNumber = 0;
        outMode = 0;
        SNR = 0;
        range = 0;
        velocity = 0;
        angle = 0;
        unConnectCount = 0;
    }
};

namespace Ui {
class SimulatorWidget;
}

class SimulatorWidget : public QWidget
{
    Q_OBJECT

public:
    SimulatorWidget(mainWidget *parent = 0);
    ~SimulatorWidget();

private:
    Cnet100Tcp *tcp;
    bool showTarget;
    QTimer *timer;
    QTimer *timerGetMessage;
    mainWidget * m_parent;
    Ui::SimulatorWidget *ui;
    Target target[5];
    void updateTarget();
private slots:
    void updateTargetColor();
    void updateDistance(int);
    void getMessage();
    void on_pushButton_clicked();
};

#endif // SIMULATORWIDGET_H
