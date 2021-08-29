#include "tcpserver.h"

#include "telegram.h"

#include <QThread>

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    intSockCounter = 0;
}

void TcpServer::startStopListen()
{
    connect(server, SIGNAL(newConnection()),
            this,   SLOT(newConnection()));

    bool isListen = server->isListening();
    if(isListen == true)
    {
        for(QHash<QTcpSocket*,QTcpSocket*>::iterator i=sockets.begin();i!=sockets.end();++i)
          {
              i.value()->deleteLater();
          }
        server->close();
        PrintConsole("Server stoped");
        emit statusTextChange("Server stoped");
        log("Server stoped");
    }
    else
    {
        if(!server->listen(QHostAddress::Any, serverPort))
        {
            PrintConsole("Server could not start");
            emit statusTextChange("Server could not start");
            log("Server could not start");
        }
        else
        {
            PrintConsole("Server is waiting for client");
            emit statusTextChange("Server is waiting for client");
            log("Server is waiting for client");
        }
    }
}

void TcpServer::newConnection()
{
    QTcpSocket *socket = server->nextPendingConnection();

    connect(socket, SIGNAL(readyRead()), SLOT(readTcpData()));
    connect(socket, SIGNAL(disconnected()), SLOT(disconnected()));

    log("Client connected:" + QHostAddress(socket->peerAddress().toIPv4Address()).toString());
    PrintConsole("Client connected:" + QHostAddress(socket->peerAddress().toIPv4Address()).toString());
    emit statusIp("Remote IP: "+QHostAddress(socket->peerAddress().toIPv4Address()).toString());

    {
        QByteArray *buffer = new QByteArray();
        qint32 *s = new qint32(0);
        buffers.insert(socket, buffer);
        sizes.insert(socket, s);
        ++intSockCounter;
        sockets.insert(socket,socket);
    }

}

void TcpServer::readTcpData()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
       QByteArray *buffer = buffers.value(socket);
       int *s = sizes.value(socket);
       int size = *s;
       int dataSize;
       while (socket->bytesAvailable() > 0)
       {
           buffer->append(socket->readAll());
           while ((size == 0 && buffer->size() >= HeaderLengthSize) || (size > 0 && buffer->size() >= size)) //While can process data, process it
           {
               if (size == 0 && buffer->size() >= HeaderLengthSize) //receive header size
               {
                   size = QString(buffer->mid(0, HeaderLengthSize)).toInt();
                   *s = size;
               }
               if (size > 0 && buffer->size() >= size + HeaderLengthSize) // If header has received completely, receive telegram
               {
                   dataSize = QString(buffer->mid(22, 4)).toInt();
                   QString msgToSent = ProcessMessage(buffer->mid(0,size + HeaderLengthSize + dataSize));
                      if(msgToSent.length() > 0)
                       {
                         writeData(msgToSent);
                          //if(socket->write(msgToSent.toLatin1())){
                           //  QString msgType=msgToSent.toLatin1().mid(4, 3);
                           //  emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\tsnd:\t" + msgType);
                           //  PrintConsole(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\tsnd:\t" + msgToSent);
                           //  log(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\tsnd:\t" + msgToSent);
                           //}
                       }
                    buffer->remove(0, size + HeaderLengthSize + dataSize);
                    size = 0;
                    dataSize =0;
                    *s = size;
                   }
               }
           }
       }

void TcpServer::disconnected()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = buffers.value(socket);
    qint32 *s = sizes.value(socket);
    socket->deleteLater();
    sockets.remove(socket);
    delete buffer;
    delete s;
    emit statusChange(QString("QLabel {background-color: red;}"));
    emit statusTextChange("Client disconnected:"+QHostAddress(socket->peerAddress().toIPv4Address()).toString());
    emit enableControls(false);
    PrintConsole("Client disconnected:"+QHostAddress(socket->peerAddress().toIPv4Address()).toString());
    log("Client disconnected:"+QHostAddress(socket->peerAddress().toIPv4Address()).toString());
}

QString TcpServer::ProcessMessage(QByteArray message)
// creates message for auto respond to the client
{
    QRegExp rx("E0[1-7]");
    QString msgType=message.mid(4, 3);
    int msgCounter=message.mid(9, 5).toInt();
    int intMsgType = message.mid(5, 2).toInt();
    if (rx.exactMatch(msgType))
    {

        switch(intMsgType)
        {
        case 1:
            PrintConsole(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\trcv:\t" + message);
            emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\trcv:\t" + msgType + "\tLogin");
            log("rcv:\t" + message);
            return msgE01;
        case 2:
            PrintConsole(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\trcv:\t" + message);
            emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\trcv:\t" + msgType + "\tLogin Ack");
            log("rcv:\t" + message);
            PrintConsole("Interface connection establisched");
            log("Interface connection establisched");
            emit statusTextChange("Interface connection establisched");
            emit statusChange(QString("QLabel {background-color: green;}"));
            emit enableControls(true);
            intE6Counter =0;
            intE3Counter =0;
            return message;
        case 5:
             PrintConsole(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\trcv:\t" + message);
             emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\trcv:\t" + msgType + "\tData Ack");
             log("rcv:\t" + message);
             return "";
        case 6:
            PrintConsole(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\trcv:\t" + message);
            emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\trcv:\t" + msgType + "\tLife");
            log("rcv:\t" + message);
            return "0022E0701" +  QString("%1").arg(msgCounter, 5, 10, QChar('0')) + "250125020000";
        case 7:
              PrintConsole(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\trcv:\t" + message);
              emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\trcv:\t" + msgType + "\tLife Ack");
              log("rcv:\t" + message);
              return "";
        case 3:
        case 4:
            QString topoL;
            QBitArray bits(message.mid(31,4).count()*8);
            for(int i=0; i<message.mid(31,4).count(); ++i) {
               for(int b=0; b<8;b++) {
                   bits.setBit( i*8+b, message.mid(31,4).at(i)&(1<<(7-b)) );
                   topoL += QString::number(bits.testBit(i*8+b));
               }
            }
            if(message.mid(28,2)=="07")
            {
                PrintConsole(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\trcv:\t" + message.mid(0,31) + "|" + topoL.remove( QRegExp("^[0]*") ) + "|" + message.mid(35,message.count()-31));
                log("rcv:\t" + message);
                emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") +
                                "\trcv:\t" +
                                msgType + "\t" +
                                message.mid(26,4) + "\t" +
                                message.mid(102,2) + "\t" +
                                message.mid(36,20).trimmed() + "\t" +
                                message.mid(56,20).trimmed() + "\t"
                                );
            }
            else if(message.mid(28,2)=="06")
            {
                emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") +
                                "\trcv:\t" +
                                msgType + "\t" +
                                message.mid(26,4) + "\t" +
                                message.mid(40,2) + "\t" +
                                message.mid(75,20).trimmed() + "\t" +
                                message.mid(42,20).trimmed() + "\t"
                                );
            }
            else
            {
                PrintConsole(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\trcv:\t" + message.mid(0,31) + "|" + topoL.remove( QRegExp("^[0]*") ) + "|" + message.mid(35,message.count()-31));
                log("rcv:\t" + message);
                emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\trcv:\t" + msgType + "\t" + message.mid(26,4));
            }

            return "0022E0501" + QString("%1").arg(msgCounter, 5, 10, QChar('0')) + "250125020000";
        }

    }
    else
    {
      PrintConsole("Err: Uknown Telegramm Type '" + msgType + "'");
    }
    return "";
}

void TcpServer::PrintConsole(QString message)
{
    qInfo()<<qPrintable(QDateTime::currentDateTime().time().toString("HH:mm:ss"))<<" "<<qPrintable(message);
}

void TcpServer::writeData(QString  data)
{
  for(QHash<QTcpSocket*,QTcpSocket*>::iterator i=sockets.begin();i!=sockets.end();++i)
    {
          if(i.value()->write(data.toLatin1()))
          {
              QRegExp rx("E0[1-7]");
              QString msgType=data.mid(4, 3);
              int intMsgType = data.mid(5, 2).toInt();
              if (rx.exactMatch(msgType))
              {
                  switch(intMsgType)
                  {
                  case 1:
                   emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\tsnd:\t" + msgType + "\tLogin");
                   break;
                  case 2:
                   emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\tsnd:\t" + msgType + "\tLogin Ack");
                   break;
                  case 5:
                   emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\tsnd:\t" + msgType + "\tData Ack");
                   break;
                  case 6:
                   emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\tsnd:\t" + msgType + "\tLife");
                   break;
                  case 7:
                   emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\tsnd:\t" + msgType + "\tLife Ack");
                   break;
                  case 3:

                      if(data.mid(28,2)=="07")
                      {
                          log("snd:\t" + data);
                          emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") +
                                          "\tsnd:\t" +
                                          msgType + "\t" +
                                          data.mid(26,4) + "\t" +
                                          data.mid(102,2) + "\t" +
                                          data.mid(36,20).trimmed() + "\t" +
                                          data.mid(56,20).trimmed() + "\t"
                                          );
                      }
                      else if(data.mid(28,2)=="06")
                      {
                          emit appendText(QDateTime::currentDateTime().time().toString("HH:mm:ss") +
                                          "\tsnd:\t" +
                                          msgType + "\t" +
                                          data.mid(26,4) + "\t" +
                                          data.mid(75,20).trimmed() + "\t" +
                                          data.mid(42,20).trimmed() + "\t"
                                          );
                      }
                   break;
                  }
              }
              log(QDateTime::currentDateTime().time().toString("HH:mm:ss") + "\tsnd:\t" + data.toLatin1());
          }
    }

}



void TcpServer::log(QString  data)
{
  QFile f("test_" + QDateTime::currentDateTime().toString("yyyyMMdd") + ".log" );
  if (f.open(QIODevice::WriteOnly | QIODevice::Append)) {
      QTextStream stream( &f );
      stream<< QDateTime::currentDateTime().time().toString("HH:mm:ss")<< " " << data << endl;
  }
}

void TcpServer::setPort(QString port)
{
  bool ok;
  serverPort = port.toUShort(&ok,10);
}
