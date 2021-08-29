#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>

#include <QTcpSocket>

#include <QTcpServer>

#include <QDebug>

#include <QtCore>

#include <QtNetwork>

#include <QString>

#include <iostream>

#include <QDir>



const int HeaderLengthSize=4;
const QString msgHeader = "0022E000100000250125020000";
const QString msgE01 =    "0022E010100000250125020000";

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);

signals:
    void statusChange    (const QString &value);
    void statusTextChange(const QString &value);
    void appendText      (const QString &value);
    void sendData        (const QString &value);
    void statusIp        (const QString &value);
    void enableControls  (const bool &value);
public slots:

    void startStopListen();
    void newConnection();
    void readTcpData();
    void disconnected();
    void writeData(QString data);
    void log(QString data);
    void setPort(QString);

private:

    void PrintConsole(QString message);


    QString ProcessMessage(QByteArray message);
    QTcpServer *server;
    qint16 serverPort;

    QHash<QTcpSocket*, QByteArray*> buffers; //We need a buffer to store data until block has completely received
    QHash<QTcpSocket*, qint32*> sizes; //We need to store the size to verify if a block has received completely
    QHash<QTcpSocket*, QTcpSocket*> sockets;
    int intE6Counter;
    int intE3Counter;
    int intSockCounter;

};



#endif // TCPSERVER_H
