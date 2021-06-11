#include "widget.h"
#include "ui_widget.h"
#include <QList>
#include <QSerialPortInfo>
#include <QDebug>
#include <QBrush>
#define  CPU_FREQ (64000000/2)
#define  M_LENGTH (400*100)


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setFixedSize(570,270);
    serial = nullptr;
    direction = 1;
    ui->lineEdit->setStyleSheet("background: transparent");
    setControlPanelEnable(false);
    calVel();

    ui->graphicsView->setScene(&qGraphicsScene);
    ui->graphicsView->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    ui->graphicsView->setStyleSheet("background: transparent");

    qGraphicsScene.addItem(&rectItem1);
    qGraphicsScene.addItem(&rectItem2);
    qGraphicsScene.addItem(&rectItem3);
    qGraphicsScene.addItem(&rectItem4);
    qGraphicsScene.addLine(25,0,25,30);
    qGraphicsScene.addLine(425,0,425,30);

    rectItem1.setRect(0,0,450,30);

    rectItem2.setRect(25,0,30,30);

    rectItem3.setRect(0,0,25,25);
    rectItem4.setRect(425,0,25,25);
    rectItem2.setBrush(QBrush(QColor("black"),Qt::Dense2Pattern));
    rectItem3.setBrush(QBrush(QColor("green"),Qt::SolidPattern));
    rectItem4.setBrush(QBrush(QColor("green"),Qt::SolidPattern));
    detectPort();
    ui->label->setText("loading...");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::detectPort()
{
    serial_info_list = QSerialPortInfo::availablePorts();
//  for(int i = 0; i < port_info_list.count(); i++)
//  {
//      qDebug() << "Name : " << port_info_list.at(i).portName();
//      qDebug() << "Description : " << port_info_list.at(i).description();
//      qDebug() << "Manufacturer: " << port_info_list.at(i).manufacturer();
//      qDebug() << "Serial Number: " << port_info_list.at(i).serialNumber();
//      qDebug() << "System Location: " << port_info_list.at(i).systemLocation();
//  }
    ui->comboBox->clear();
    for(int i = 0; i < serial_info_list.count(); i++)
    {
        ui->comboBox->addItem(serial_info_list.at(i).portName());
    }
    ui->lineEdit->setText(\
    serial_info_list.at(ui->comboBox->currentIndex()).description());
}

void Widget::on_comboBox_currentIndexChanged(int index)
{
    ui->lineEdit->setText(serial_info_list.at(index).description());
}

bool Widget::openPort(QString port)
{
    serial = new Serial(this);
    serial->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);
    serial->setDataBits(QSerialPort::Data8);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setParity(QSerialPort::NoParity);
    connect(serial,SIGNAL(readyRead()),this,SLOT(slot_data_recv()));
    serial->setPortName(port);
    if(serial->open(QIODevice::ReadWrite))
        return true;
    else
        return false;
}

void Widget::slot_data_recv()
{
    QByteArray byteArray = serial->readAll();
    for(int i = 0; i < byteArray.size(); i++)
    {
        praseByte(byteArray.at(i));
    }
    if(byteArray.at(1)!='B')
    qDebug()<<"MSG:"<<byteArray<<"\n";
}


void Widget::on_pushButton_clicked()
{
    static bool isStop = false;
    if(isStop)
    {
        ui->pushButton->setEnabled(false);
        if(serial)
        {
            serial->close();
            serial->deleteLater();
            serial = nullptr;
        }
        setControlPanelEnable(false);
        isStop = false;
        ui->pushButton->setText("Start");
        ui->pushButton->setEnabled(true);
        ui->comboBox->setEnabled(true);
    }
    else
    {
        if(ui->comboBox->count())
        {
            ui->pushButton->setEnabled(false);
            if(openPort(serial_info_list.at(ui->comboBox->currentIndex())\
                        .portName()))
            {
                ui->comboBox->setEnabled(false);
                setControlPanelEnable(true);
                isStop = true;
                ui->pushButton->setText("Stop");
                ui->pushButton->setEnabled(true);
            }
            else
            {
                //TODO XXOO
                ui->pushButton->setEnabled(true);
            }
        }
    }
}

void Widget::setControlPanelEnable(bool enable)
{
    ui->positiveButton->setCheckable(enable);
    ui->reverseButton->setCheckable(enable);
    ui->runButton->setEnabled(enable);
    ui->resetButton->setEnabled(enable);
    ui->velLineEdit->setEnabled(enable);

    ui->lineEdit_2->setEnabled(enable);
    ui->lineEdit_3->setEnabled(enable);
    ui->pushButton_2->setEnabled(enable);
    if(enable)
        ui->positiveButton->setChecked(true);
}

void Widget::on_resetButton_clicked()
{
    if(serial)
    {
        ui->resetButton->setEnabled(false);
        serial->setReset(ui->positiveButton->isChecked());
    }
}

void Widget::on_pushButton_2_clicked()
{
    static int count = 0;
    if(serial)
    {
        ui->lineEdit_3->setEnabled(false);
        ui->lineEdit_2->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        serial->setPSCandARR(ui->lineEdit_3->text().toUInt(),\
                             ui->lineEdit_2->text().toUInt());
//        while(1)
//        {
//            serial->setRun(count ++);
//            serial->Sleep(100);
//        }
    }
}

void Widget::on_runButton_clicked()
{
    static uint8_t isStop = false;
    direction = ui->positiveButton->isChecked();
    if(isStop)
    {
        if(serial)
        {
            ui->runButton->setEnabled(false);
            qDebug()<<"DebugInfo:"<<0+direction*2<<"\n";
            serial->setRun(0+direction*2);
            ui->runButton->setText("Run");

            isStop = false;
        }
    }
    else
    {
        if(serial)
        {
            ui->runButton->setEnabled(false);
            qDebug()<<"DebugInfo:"<<1+ui->positiveButton->isChecked()*2<<"\n";
            serial->setRun(1+direction*2);
            ui->runButton->setText("Stop");
            isStop = true;
        }
    }
}

uint32_t Widget::praseByte(uint8_t byte)
{
    static char cmd = 0;
    static uint8_t buffer[10];
    static uint8_t buffer_index = 0;
    static uint8_t payload_length = 0;
    static uint8_t new_msg_flag = 0;
    static enum  PRASE_STATUS prase_status = WAIT_HEADER;

    switch (prase_status) {
    case WAIT_HEADER:
        if(byte == 'X')
        {
            prase_status = WAIT_CMD;
        }
        break;
    case WAIT_CMD:
        cmd = byte;
        prase_status = WAIT_LENGTH;
        break;
    case WAIT_LENGTH:
        if(byte >= '0' && byte <= '9')
        {
            payload_length = byte - '0';
            prase_status = WAIT_PAYLOAD;
        }
        else
            prase_status = WAIT_HEADER;
        break;
    case WAIT_PAYLOAD:
        payload_length --;
        buffer[buffer_index++] = byte;
        if(!payload_length)
        {
            buffer_index = 0;
            prase_status = WAIT_TAIL;
        }
        break;
    case WAIT_TAIL:
        if(byte == 'X')
        {
            prase_status = WAIT_HEADER;
            new_msg_flag = 1;
        }
        break;
    }
    if(new_msg_flag)
    {
        switch (cmd){
        case 'A':
            if(*(buffer) == 'E')
            {
                ui->runButton->setEnabled(true);
            }
            else if(*(buffer) == 'B')
            {
                ui->lineEdit_3->setEnabled(true);
            }
            else if(*(buffer) == 'C')
            {
                ui->lineEdit_2->setEnabled(true);
                ui->pushButton_2->setEnabled(true);
            }
            else if(*(buffer) == 'D')
            {
                ui->resetButton->setEnabled(true);
            }
            qDebug()<<"cmd:a"<<*(buffer)<<"\n";
            break;
        case 'B':

            uint32_t pwm_count = *((uint32_t*)(buffer));
            uint8_t limit_status = *((uint8_t*)(buffer+4));
            qDebug()<<"pwm_count = "<<pwm_count<<"\n";


            if(direction)
            {
                qDebug()<<"px:"<<(int)(25+(pwm_count/2.0)/M_LENGTH*370)<<"\n";
                rectItem2.setRect((int)(25+(pwm_count/2.0)/M_LENGTH*370),0,30,30);
            }
            else
            {
                qDebug()<<"px:"<<(int)(25+(((M_LENGTH*2-pwm_count)/2.0)/M_LENGTH*370))<<"\n";
                rectItem2.setRect((int)(25+(((M_LENGTH*2-pwm_count)/2.0)/M_LENGTH*370)),0,30,30);
            }
            if(pwm_count>= M_LENGTH*2&&ui->runButton->text()=="Stop")
                on_runButton_clicked();
            break;
        }
//        qDebug()<<"cmd:"<<cmd<<"\n";
        new_msg_flag = 0;
    }
    return 1;
}

void Widget::calVel()
{
    uint32_t ARR = ui->lineEdit_3->text().toUInt();
    uint32_t PSC = ui->lineEdit_2->text().toUInt();

    ui->velLineEdit->setText(QString::number(\
                     CPU_FREQ/400.0/(ARR + 1)/(PSC + 1)));
}

bool Widget::calARRandPSC()
{
    double vel = ui->velLineEdit->text().toFloat();
    if(vel < 1e-5)
        return false;
    double AMP = CPU_FREQ/400/vel;

    uint32_t sqrtAMP = sqrt(AMP);
    uint32_t A,P;
    uint32_t count = 0;
    A = sqrtAMP;P = sqrtAMP;
    double ex = fabs(AMP-A*P);
    qDebug()<<"AMP="<<AMP<<"A="<<A<<"e="<<ex<<"\n";
    double e = iter(&A,&P,A,P,AMP,ex,30,30, &count);
    QString str;
    str.append("iter count:");
    str.append(QString::number(count));
    str.append("error:");
    str.append(QString::number(e));
    str.append("/");
    str.append(QString::number(AMP));
    ui->label->setText(str);


    ui->lineEdit_3->setText(QString::number(P - 1));
    ui->lineEdit_2->setText(QString::number(A - 1));
    return 0;

}

double Widget::iter(uint32_t* A_, uint32_t* P_,\
                    uint32_t AO,uint32_t PO,\
                    double target, double e,\
                    uint32_t depth_a, uint32_t depth_b,\
                    uint32_t* count)
{
    static int32_t A1,A2,P1,P2,AB,PB;
    static double E;
    static double E1;
    A1 = AO - depth_a;
    P1 = PO - depth_a;
    A2 = AO + depth_b;
    P2 = PO + depth_b;
    if(A1 < 1)
        A1 = 1;
    if(P1 < 1)
        P1 = 1;
    if(A2 > 65535)
        A2 = 65535;
    if(P2 > 65535)
        P2 = 65535;

    AB = A1,PB = P1;
    E  = fabs(target - A1 * P1);
    for(int i = A1; i <= A2; i++)
    {
        for(int j = P1; j <= P2; j++)
        {
            E1 = fabs(target - i * j);
            if(E1 < E)
            {
                AB = i,PB = j;
                E = E1;
            }
        }
    }

    (*count) ++;
    qDebug()<<"E = "<<E<<"\n";
    if(E < e)
    {
        if(E < 1e-3)
        {
            *A_ = AB;
            *P_ = PB;
            qDebug()<<"11111\n";
            return E;
        }
        else
        {
            *A_ = AB;
            *P_ = PB;
            qDebug()<<"AB:"<<AB<<"PB:"<<PB<<"E="<<E<<"EC="<<fabs(target - AB * PB)<<"\n";
            if(PB*2/65535>0)
                PB = 65535;
            else
                PB = PB * 2;
            AO = AB/2;
            if(AO <1)
                AO = 1;
            PO = PB;
             qDebug()<<"22222\n";
            return iter(A_,P_,AO,PO,target,E,depth_a,depth_a,count);
        }
    }
    else
    {
//        if(depth_b >= 400 && *count < 150)
//        {
//            if(PB*2/65535>0)
//                PB = 65535;
//            else
//                PB = PB * 2;
//            if(AO > 20)
//                 AO = AB/2;
//            PO = PB;
//            qDebug()<<"444444\n";
//            return iter(A_,P_,AO,PO,target,E,depth_a,depth_a,count);
//        }
        if(depth_b > 6000 ||*count > 150)
        {
//            *A_ = AO;
//            *P_ = PO;
            qDebug()<<"4444\n";
            return e;
        }
         qDebug()<<"33333\n";
         return iter(A_,P_,AO,PO,target,e,depth_a,5*depth_b,count);
    }

}


void Widget::on_lineEdit_3_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    calVel();
}

void Widget::on_lineEdit_2_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    calVel();
}

void Widget::on_pushButton_3_clicked()
{
       calARRandPSC();
}
