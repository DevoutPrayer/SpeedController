#include "serial.h"
#include <QDebug>

Serial::Serial(QObject *parent) : QSerialPort(parent)
{

}

bool Serial::openPort(QString port)
{
    this->setPortName(port);
    this->close();
    if(this->open(QIODevice::ReadWrite))
        return true;
    else
        return false;
}

bool Serial::setPSCandARR(uint16_t PSC,uint16_t ARR)
{
    if(!setPSC(PSC))
        return false;
    if(!setARR(ARR))
        return false;
    return true;
}

bool Serial::setPSC(uint16_t PSC)
{
    QByteArray byteArray;
    byteArray.append('X');
    byteArray.append('B');
    byteArray.append('2');
    byteArray.append((const char*)&PSC,2);
    byteArray.append('X');
    write(byteArray);

    return true;
}
bool Serial::setARR(uint16_t ARR)
{
    QByteArray byteArray;
    byteArray.append('X');
    byteArray.append('C');
    byteArray.append('2');
    byteArray.append((const char*)&ARR,2);
    byteArray.append('X');
    write(byteArray);
//    qDebug()<<"debug:"<<byteArray<<"\n";

    return true;
}

bool Serial::setReset(uint8_t direction)
{

    QByteArray byteArray;
    byteArray.append('X');
    byteArray.append('D');
    byteArray.append('1');
    byteArray.append((const char*)&direction,1);
    byteArray.append('X');
    write(byteArray);
//    qDebug()<<"debug:"<<byteArray<<"\n";

    return true;
}

bool Serial::setRun(uint8_t enable)
{

    QByteArray byteArray;
    byteArray.append('X');
    byteArray.append('E');
    byteArray.append('1');
    byteArray.append((const char*)&enable,1);
    byteArray.append('X');
    writeData(byteArray.data(),byteArray.size());

//    qDebug()<<"debug:"<<byteArray<<"\n";
    return true;
}
