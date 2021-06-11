#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

#include "serial.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

enum PRASE_STATUS
{
    WAIT_HEADER,
    WAIT_CMD,
    WAIT_LENGTH,
    WAIT_PAYLOAD,
    WAIT_TAIL
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private slots:
    void on_comboBox_currentIndexChanged(int index);
    void slot_data_recv();
    void on_pushButton_clicked();
    void on_resetButton_clicked();

    void on_pushButton_2_clicked();

    void on_runButton_clicked();




    void on_lineEdit_3_textChanged(const QString &arg1);

    void on_lineEdit_2_textChanged(const QString &arg1);

    void on_pushButton_3_clicked();

private:
    Ui::Widget             *ui;
    Serial                 *serial;
    QList<QSerialPortInfo> serial_info_list;
    QGraphicsScene         qGraphicsScene;
    QGraphicsRectItem      rectItem1;
    QGraphicsRectItem      rectItem2;
    QGraphicsRectItem      rectItem3;
    QGraphicsRectItem      rectItem4;
    uint8_t                direction;
    void detectPort();
    bool openPort(QString port);
    void setControlPanelEnable(bool);
    void calVel();
    bool calARRandPSC();
    double iter(uint32_t* A_, uint32_t* P_, uint32_t AO,uint32_t PO,double target, double e,uint32_t depth_a, uint32_t depth_b,uint32_t* count);
    uint32_t praseByte(uint8_t);
};
#endif // WIDGET_H
