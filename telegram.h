#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QtCore>
#include <QtNetwork>
#include <QString>
#include <iostream>
#include <QBitArray>

class telegram : public QObject
{
    Q_OBJECT

public:
    explicit telegram(QObject *parent = 0);

  QString dataState;
  QString dataSopt;
  QString dataDopt;
  QString dataTnumber;
  QString dataDate;
  QString dataSchTime;
  QString dataActTime;
  QString dataMsgType;
  QString dataSSignal;
  QString dataDSignal;
  QString dataInfra;
  QString dataDelay;
  QString dataRefNum;
  QString dataKindofReq;
      QString CreateTelegramm(QString dataHdrID,  QString msgType, int msgState);

signals:

public slots:

private:
  const QString hdrLength = "0022";
        QString hdrMsgType;
        QString hdrVers;
        int hdrE03SeqNo;
        int hdrE06SeqNo;
        QString hdrSrcID;
        QString hdrTargID;
        QString hdrDataLength;

        QByteArray bitsToBytes(QBitArray bits);


  const QString dataHdrVers = "0";
        QString dataHdrTopoL;

};

#endif // TELEGRAM_H
