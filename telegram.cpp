#include "telegram.h"

telegram::telegram(QObject *parent) : QObject(parent)
{

     hdrMsgType.fill('0',3);
     hdrVers = "01";
     hdrE03SeqNo=0;
     hdrE06SeqNo=0;
     hdrSrcID = "2501";
     hdrTargID = "2502";
     hdrDataLength.fill('0',4);

     dataHdrTopoL.fill('\0',4);


     dataState.fill('0',1);
     dataSopt.fill(' ',20);
     dataDopt.fill(' ',20);
     dataTnumber.fill(' ',8);
     dataDate.fill(' ',8);
     dataSchTime.fill('0',14);
     dataActTime.fill('0',14);
     dataMsgType.fill('0',2);
     dataSSignal.fill(' ',20);
     dataDSignal.fill(' ',20);

     dataInfra = "2502";
     dataDelay.fill('0',5);

     dataRefNum = "0001";
     dataKindofReq = "2";
}

QString telegram::CreateTelegramm(QString dataHdrID, QString msgType, int msgState)
{
  QString message;
  QString header;
  dataState = QString::number(msgState);
  QBitArray dataHdrTopoLBits(32);
  dataHdrTopoL = QString::fromLatin1(bitsToBytes(dataHdrTopoLBits),4);

  if(msgType == "E03")
    {
      if(dataHdrID == "2807")
        {
          dataHdrTopoLBits.fill(true,24,26);
          dataHdrTopoL = QString::fromLatin1(bitsToBytes(dataHdrTopoLBits),4);
          message = dataHdrID + dataHdrVers + dataHdrTopoL +
                    dataState +
                    dataSopt +
                    dataDopt +
                    dataTnumber +
                    dataSchTime +
                    dataActTime +
                    dataMsgType +
                    dataSSignal +
                    dataDSignal;
        }
      else
        {
          message = "";
        }
      header = hdrLength + msgType + hdrVers +
               QString::number(++hdrE03SeqNo).rightJustified(5,'0') +
               hdrSrcID + hdrTargID +
               QString::number(message.length()).rightJustified(4,'0');
      return header + message;
    }
  else if(msgType == "E06")
    {
      header = hdrLength + msgType + hdrVers +
               QString::number(++hdrE06SeqNo).rightJustified(5,'0') +
               hdrSrcID + hdrTargID +
               QString::number(message.length()).rightJustified(4,'0');
      return header + message;
    }
  return "";

}

QByteArray telegram::bitsToBytes(QBitArray bits)
{
    QByteArray bytes;
    bytes.resize(bits.count()/8);
    bytes.fill(0);
    // Convert from QBitArray to QByteArray
    for(int b=0; b<bits.count(); ++b)
        bytes[b/8] = ( bytes.at(b/8) | ((bits[b]?1:0)<<(b%8)));
    return bytes;
}
