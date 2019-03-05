#include "SimulatorWidget.h"
#include "ui_SimulatorWidget.h"

SimulatorWidget::SimulatorWidget(mainWidget *parent) :
    m_parent(parent),
    ui(new Ui::SimulatorWidget)
{
    ui->setupUi(this);
    //设置颜色
    QPalette palette;
    palette.setColor(QPalette::WindowText,Qt::white);
    palette.setBrush(QPalette::Background, Qt::black);
    this->setPalette(palette);
    timer = new QTimer(this);
    showTarget = true;
    connect(timer,SIGNAL(timeout()),this,SLOT(updateTargetColor()));
    timer->start(50);
    ui->com_selectDistance->setEditable(false);
    connect(ui->com_selectDistance,SIGNAL(currentIndexChanged(int)),this,SLOT(updateDistance(int)));

    timerGetMessage = new QTimer(this);
    connect(timerGetMessage,SIGNAL(timeout()),this,SLOT(getMessage()));
    timerGetMessage->start(100);

    ui->lab_backimage->setPixmap(QPixmap("./Image/back.jpg").scaled(541, 521, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    tcp = new Cnet100Tcp(5000,"192.168.10.15");
}

SimulatorWidget::~SimulatorWidget()
{
    delete ui;
}

void SimulatorWidget::updateTargetColor()
{
    showTarget = !showTarget;
    if(!showTarget){
        ui->lab_target1->setStyleSheet("QLabel {background-color:red;}");
        ui->lab_target2->setStyleSheet("QLabel {background-color:yellow;}");
        ui->lab_target3->setStyleSheet("QLabel {background-color:green;}");
        ui->lab_target4->setStyleSheet("QLabel {background-color:gray;}");
        ui->lab_target5->setStyleSheet("QLabel {background-color:blue;}");
    }else{
        ui->lab_target1->setStyleSheet("QLabel {background-color:white;}");
        ui->lab_target2->setStyleSheet("QLabel {background-color:white;}");
        ui->lab_target3->setStyleSheet("QLabel {background-color:white;}");
        ui->lab_target4->setStyleSheet("QLabel {background-color:white;}");
        ui->lab_target5->setStyleSheet("QLabel {background-color:white;}");
    }
}

void SimulatorWidget::updateDistance(int index)
{
    ui->lab_distance1->setText(QString::number((index+1)*50*10/50));
    ui->lab_distance2->setText(QString::number((index+1)*50*20/50));
    ui->lab_distance3->setText(QString::number((index+1)*50*30/50));
    ui->lab_distance4->setText(QString::number((index+1)*50*40/50));
    ui->lab_distance5->setText(QString::number((index+1)*50*50/50));
}

void SimulatorWidget::getMessage()
{
    string str = tcp->getMessage();
    target[0].unConnectCount ++;
    target[1].unConnectCount ++;
    target[2].unConnectCount ++;
    target[3].unConnectCount ++;
    target[4].unConnectCount ++;
    if(!str.empty()){
        int count = str.length()/26;
        for(int j = 0;j < count; j++){
            string t = str.substr(j*26,26);
            if(!t.substr(0,10).compare("0800000500"))
            {
                vector<float> ret = tcp->analysis77G500(HexToBin(t));
                if(ret.size() > 4){
                    //cout<<ret[0]<<" "<<ret[1]<<" "<<setw(10)<<ret[2]<<" "<<setw(10)<<ret[3]<<" "<<setw(20)<<ret[4]<<" "<<setw(10)<<ret[5]<<" "<<setw(10)<<ret[6]<<endl;
                   for(int i = 0;i < 5; i++){
                       if(target[i].enable == 0){
                           target[i].enable = 1;
                           target[i].chipNumber = ret[0];
                           target[i].outMode = ret[1];
                           target[i].SNR = ret[2];
                           target[i].range = ret[3];
                           target[i].velocity = ret[4];
                           target[i].angle = ret[5];
                           target[i].unConnectCount = 0;
                           break;
                       }else if(target[i].enable == 1 && target[i].chipNumber == ret[0]){
                           target[i].outMode = ret[1];
                           target[i].SNR = ret[2];
                           target[i].range = ret[3];
                           target[i].velocity = ret[4];
                           target[i].angle = ret[5];
                           target[i].unConnectCount = 0;
                           break;
                       }
                    }
                }
            }
            if(!t.substr(0,10).compare("0800000323"))
            {
                vector<unsigned int> ret = tcp->analysis24G1T1R323(HexToBin(t));
                    cout<<"zz"<<ret[0]<<" "<<ret[1]<<" "<<ret[2]<<endl;
                //usleep(100*1000);
            }
        }
    }
    for(int i = 0 ;i < 5; i ++)
    {
        if(target[i].unConnectCount > 20)
        {
            target[i].enable = 0;
            target[i].unConnectCount = 0;
        }
    }
    updateTarget();
}

void SimulatorWidget::updateTarget()
{
//    target[0].enable == 0 ? ui->lab_target1->hide() : ui->lab_target1->show();
//    target[1].enable == 0 ? ui->lab_target2->hide() : ui->lab_target2->show();
//    target[2].enable == 0 ? ui->lab_target3->hide() : ui->lab_target3->show();
//    target[3].enable == 0 ? ui->lab_target4->hide() : ui->lab_target4->show();
//    target[4].enable == 0 ? ui->lab_target5->hide() : ui->lab_target5->show();

    for(int i = 0;i < 5; i++){
        int targetY2 = target[i].range * 2 *(5 - (ui->com_selectDistance->currentIndex()));
        if(i == 0){
            ui->lab_target1->setGeometry(250,110 - targetY2,40,15);
        }else if(i == 1){
            ui->lab_target2->setGeometry(250,210 - targetY2,40,15);
        }else if(i == 2){
            ui->lab_target3->setGeometry(250,310 - targetY2,40,15);
        }else if(i == 3){
            ui->lab_target4->setGeometry(250,410 - targetY2,40,15);
        }else if(i == 4){
            ui->lab_target5->setGeometry(250,510 - targetY2,40,15);
        }
    }
}

void SimulatorWidget::on_pushButton_clicked()
{
    this->close();
    m_parent->showUsrchosewidget();
}
