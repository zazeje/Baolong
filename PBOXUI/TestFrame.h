#ifndef TESTFRAME_H
#define TESTFRAME_H

#include <QtGui>
#include <QTextEdit>
#include "./LineDevice/Device/ComDevice.h"
#include "./LineDevice/Device/TcpDevice.h"
#include "./LineDevice/PlcDevice/FxPlc.h"
#include "./LineDevice/DigitalmeterDevice/XSE6.h"
#include "./LineDevice/TpmsHandleDevice/TpmsHandle.h"
//#include "./LineDevice/LightAndRainDevice/LightAndRainDevice.h"
#include "./LineDevice/ScannerDevice/KeyenceSR.h"
#include "./LineDevice/MarkerDevice/LaserMarker.h"
#include "./LineDevice/SpectrumAnalyzerDevice/AgilentN9010A.h"
#include "./LineDevice/SpectrumAnalyzerDevice/RohdeSchwarz.h"
#include "./LineDevice/MultimeterDevice/AgilentMultimeterTcp.h"
#include "./LineDevice/MultimeterDevice/AgilentMultimeterSerial.h"
#include "./LineDevice/ScannerDevice/DataManSR.h"
#include "./LineDevice/ProgramDevice/CycloneProgram.h"
#include "./LineDevice/ProgramDevice/CycloneProUdp.h"
#include "mainwidget.h"

class TestFrame : public QWidget
{

    Q_OBJECT

public:

    TestFrame(mainWidget *parent = 0);
    ~TestFrame();

    QComboBox *m_deviceBox;

private:

    QLabel *m_topLable;
    QGroupBox *m_firstGroup;
    QHBoxLayout *m_topLayout;

    QLabel *m_comLabel;
    QComboBox *m_comBox;
    QHBoxLayout *m_comHLayout;
    QLabel *m_IPaddrLabel;
    QLineEdit *m_IPaddrEdit;
    QHBoxLayout *m_IPHLayout;
    QHBoxLayout *m_firstHLayout;

    QLabel *m_baudrateLabel;
    QComboBox *m_baudrateBox;
    QHBoxLayout *m_baudHLayout;
    QLabel *m_portLabel;
    QLineEdit *m_portEdit;
    QHBoxLayout *m_portHLayout;
    QHBoxLayout *m_secondHLayout;

    QLabel *m_databitLabel;
    QComboBox *m_databitBox;
    QHBoxLayout *m_databitHLayout;
    QLabel *m_stopbitLabel;
    QComboBox *m_stopbitBox;
    QHBoxLayout *m_stopbitHlayout;
    QHBoxLayout *m_thirdHLayout;

    QLabel *m_parityLabel;
    QComboBox *m_parityBox;
    QHBoxLayout *m_parityHLayout;

    QLabel *m_deviceLabel;
    QHBoxLayout *m_deviceHLayout;
    QHBoxLayout *m_fourthHLayout;

    QVBoxLayout* m_secondVlayout;
    QGroupBox* m_secondGroup;

    QLabel* m_infoLabel;
    QTextEdit* m_infoEdit;
    QVBoxLayout* m_fifthVLayout;
    QGroupBox* m_thirdGroup;

    QPushButton* m_testButton;
    QPushButton* m_backButton;
    QHBoxLayout* m_sixHLayout;
    QGroupBox* m_fourthGroup;

    QVBoxLayout* m_totalLayout;

    mainWidget *m_parent;

private slots:

    void doback();
    void dotest();
    void addItem();

};

#endif //TESTFRAME_H
