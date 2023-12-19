#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    QWidget::setWindowTitle("Comport Port GUI");

    // Checking available Ports
    QList<QSerialPortInfo> ports = info.availablePorts();
    QList<QString> stringPorts;
    for(int i = 0 ; i < ports.size() ; i++){
        stringPorts.append(ports.at(i).portName());
    }
    ui->comboBox->addItems(stringPorts);

    // Baud Rate Ratios
    QList<qint32> baudRates = info.standardBaudRates();
    QList<QString> stringBaudRates;
    for(int i = 0 ; i < baudRates.size() ; i++){
        stringBaudRates.append(QString::number(baudRates.at(i)));
    }
    ui->comboBox_2->addItems(stringBaudRates);

    // Data Bits
    ui->comboBox_3->addItem("7");
    ui->comboBox_3->addItem("8");

    // Parities
    ui->comboBox_5->addItem("No Parity");
    ui->comboBox_5->addItem("Even Parity");
    ui->comboBox_5->addItem("Odd Parity");
    ui->comboBox_5->addItem("Mark Parity");

}
//CONNECTION!!
void Widget::on_pushButton_clicked()
{

    QString portName = ui->comboBox->currentText();
    serialPort.setPortName(portName);

    serialPort.open(QIODevice::ReadWrite);

    if(!serialPort.isOpen()){
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->append("!!!! Something went Wrong !!!!");
    }
    else {

        QString stringbaudRate = ui->comboBox_2->currentText();
        int intbaudRate = stringbaudRate.toInt();
        serialPort.setBaudRate(intbaudRate);
        ui->textBrowser->setTextColor(Qt::blue);
        ui->textBrowser->append("!CONNECTED!");

        QString dataBits = ui->comboBox_3->currentText();
        if(dataBits == "7 Bits") {
            serialPort.setDataBits(QSerialPort::Data7);
        }
        else if(dataBits == "8 Bits"){
            serialPort.setDataBits(QSerialPort::Data8);
        }

        QString parity = ui->comboBox_5->currentText();
        if(parity == "No Parity"){
            serialPort.setParity(QSerialPort::NoParity);
        }
        else if(parity == "Even Parity"){
            serialPort.setParity(QSerialPort::EvenParity);
        }
        else if(parity == "Odd Parity"){
            serialPort.setParity(QSerialPort::OddParity);
        }
        else if(parity == "Mark Parity"){
            serialPort.setParity(QSerialPort::MarkParity);
        }

        connect(&serialPort,SIGNAL(readyRead()),this,SLOT(receiveMessage()));
    }


}


void Widget::receiveMessage()
{
    QByteArray dataBA = serialPort.readAll();
    QString data(dataBA);
    buffer.append(data);
    int index = buffer.indexOf(code);
    if(index != -1){
        QString message = buffer.mid(0,index);
        ui->textBrowser->append(message);
        buffer.remove(0,index+codeSize);
    }

}
//Button of Send
void Widget::on_pushButton_5_clicked()
{
    QString message = ui->lineEdit_2->text();
    ui->textBrowser->setTextColor(Qt::darkGreen);
    ui->textBrowser->append(message);
    serialPort.write(message.toUtf8());
}


// Button of Disconnect
void Widget::on_pushButton_2_clicked()
{
    ui->textBrowser->setTextColor(Qt::red);
    ui->textBrowser->append("!!DISCONNECTED!!");
    serialPort.close();
}

// Button of Clear
void Widget::on_pushButton_4_clicked()
{
    ui->textBrowser->clear();
}
Widget::~Widget()
{
    delete ui;
}
