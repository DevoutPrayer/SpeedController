#ifndef SERIAL_H
#define SERIAL_H

#include <QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QList>
#include <QTime>
#include <QCoreApplication>

class Serial : public QSerialPort
{
    Q_OBJECT
public:
    explicit Serial(QObject *parent = nullptr);
    bool openPort(QString port);
    bool setPSCandARR(uint16_t PSC,uint16_t ARR);
    bool setPSC(uint16_t PSC);
    bool setARR(uint16_t ARR);
    bool setReset(uint8_t direction);
    bool setRun(uint8_t enable);
    void Sleep(int msec)
    {
        QTime dieTime = QTime::currentTime().addMSecs(msec);
        while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

signals:
    void signal_report_error(int);
};

#endif // SERIAL_H
