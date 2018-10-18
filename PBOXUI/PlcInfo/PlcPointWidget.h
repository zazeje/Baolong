#ifndef PLCPOINTWIDGET_H
#define PLCPOINTWIDGET_H

#include <QWidget>
#include <QObject>
#include <QLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QTableWidget>


class PlcPointWidget : public QWidget
{
    Q_OBJECT

public:

    explicit PlcPointWidget();
    ~PlcPointWidget();

protected:

    QLabel* m_titleLabel;
    QHBoxLayout* m_titleLayout;
    QGroupBox* m_titleGroup;

    QPushButton* m_previousPage;
    QPushButton* m_nextPage;
    QLabel* m_pageMessage;
    //add 2017-3-31
    QLabel *m_currentpageLabel;
    QLabel *m_totalpageLabel;
    QLabel *m_spiltLabel;
    QLabel *m_pageLabel;
    QGroupBox* m_pageGroupBox;
    QVBoxLayout *m_pageVBoxLayout;


    QHBoxLayout* m_pageLayout;
    QHBoxLayout* m_pageHBoxLayout;
    QTableWidget* m_tableWidget;
    QPushButton* m_returnButton;
    QHBoxLayout* m_returnLayout;
    QVBoxLayout* m_vBoxLayout;

};

#endif // PLCPOINTWIDGET_H
