#include "mainwindow.h"

#include "ui_mainwindow.h"

#include "tcpserver.h"

#include "telegram.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //server = new TcpServer;
    loadOptics();
    connect(ui->comboBox_E, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_EChanged(QString)));
    //connect(ui->lineEdit_Port, SIGNAL(textChanged(QString)),server, SLOT(setPort(QString)));
    ui->pushButton->setStyleSheet(QString("QPushButton {background-color: red;}"));
    label = new QLabel();
    statusText = new QLabel();
    telE << "2807";
    Tel = new telegram;
    label->setStyleSheet(QString("QLabel {background-color: red;}"));
    label->setFixedWidth(25);
    label->setFixedHeight(25);
    ui->statusbar->addPermanentWidget(statusText);
    ui->statusbar->addPermanentWidget(label);
    emit on_EChanged(ui->comboBox_E->currentText());
    emit enableControls(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()   //::on_startButton_clicked()
{
  if(ui->pushButton->isChecked())
    {
      ui->pushButton->setStyleSheet(QString("QPushButton {background-color: green;}"));
      ui->pushButton->setText("Stop server");
      server = new TcpServer;
      connect(server, SIGNAL(statusChange(QString)),label, SLOT(setStyleSheet(QString)));
      connect(server, SIGNAL(statusTextChange(QString)),statusText, SLOT(setText(QString)));
      connect(server, SIGNAL(statusIp(QString)),ui->label_Ip, SLOT(setText(QString)));
      connect(server, SIGNAL(appendText(QString)),ui->textBrowser, SLOT(append(QString)));
      connect(server, SIGNAL(enableControls(bool)),this, SLOT(enableControls(bool)));
      //connect(server, SIGNAL(appendText(QString)),server, SLOT(log(QString)));
      connect(server, SIGNAL(sendData(QString)), server, SLOT(writeData(QString)));
      ui->lineEdit_Port->setDisabled(true);
      emit server->setPort(ui->lineEdit_Port->text());
      server->startStopListen();
    }
  else
    {
     ui->pushButton->setStyleSheet(QString("QPushButton {background-color: red;}"));
      ui->pushButton->setText("Start server");
      server->startStopListen();
      delete server;
      ui->lineEdit_Port->setDisabled(false);
      label->setStyleSheet(QString("QLabel {background-color: red;}"));
      ui->textBrowser->clear();
      //ui->label_Ip->setText("Remote IP:");
    }

}

void MainWindow::on_pushButton_send_clicked()
{
Tel->dataTnumber = ui->lineEdit->text().rightJustified(8,' ');
Tel->dataSopt = ui->comboBox_Sopt->currentText().leftJustified(20,' ');
Tel->dataDopt = ui->comboBox_Dopt->currentText().leftJustified(20,' ');
Tel->dataMsgType = ui->comboBox_M_type->currentText();
Tel->dataDate = QDateTime::currentDateTime().toString("yyyyMMdd");
Tel->dataActTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
Tel->dataState = ui->comboBox_M_type->currentText();
emit server->sendData(Tel->CreateTelegramm(ui->comboBox_T->currentText(),ui->comboBox_E->currentText(),1));
}

void MainWindow::on_EChanged(QString E)
{
  if(E == "E03")
    {
     ui->comboBox_T->addItems(telE);
    }
  else
    {
     ui->comboBox_T->clear();
    }
}

void MainWindow::enableControls(bool enabled)
{
    ui->pushButton_send->setEnabled(enabled);
    ui->comboBox_T->setEnabled(enabled);
    ui->comboBox_E->setEnabled(enabled);
    ui->comboBox_M_type->setEnabled(enabled);
    ui->comboBox_Sopt->setEnabled(enabled);
    ui->comboBox_Dopt->setEnabled(enabled);
    ui->lineEdit->setEnabled(enabled);
}

void MainWindow::loadOptics()
{
  QString string[1];
  string[0] = "/opt.dat";
  //string[1] = "/lvj_opt.dat";
  //string[2] = "/ldi_opt.dat";
  for(int i = 0; (unsigned)i <= sizeof(string)/sizeof(string[0])-1; i++){
      QFile inputFile(QDir::currentPath() + string[i]);

      if (inputFile.open(QIODevice::ReadOnly))
      {
         QTextStream in(&inputFile);
         while (!in.atEnd())
         {
            QString line = in.readLine();
            if(line.at(0) != '-')
              {
                opt<<line.split("\t").value(0);
              }

           }
         inputFile.close();
         ui->comboBox_Sopt->addItems(opt);
         ui->comboBox_Dopt->addItems(opt);
      }
    }

}
