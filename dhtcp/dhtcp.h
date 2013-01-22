#ifndef DHTCP_H
#define DHTCP_H

#include <QDataStream>
#include <QTcpSocket>
#include <QHostAddress>
#include "datahandler.h"
#include "protocol/cmd_define.h"
#include "protocol/packet.h"
#include "serverconfig.h"

#include "dhtcpprotocol.h"
#include "dhtcpencoder.h"

namespace DHtcp{

#define WAIT_FOR_CONNECTED_TIMEOUT 5000
#define WAIT_FOR_BYTES_WRITTEN_TIMEOUT (5000)  //5s

class DHtcp : public DataHandler
{
    Q_OBJECT
public:
    explicit DHtcp(const QByteArray arg, QObject *parent = 0);
    QByteArray getInitProtocAckArg();
    bool isInitOk();
signals:
    
public slots:
private slots:
    void onDataSktReadyRead();

private:
    void processCMD(const Packet& p);
    QString psCmdDbg(QString cmd, QString arg = QString());
    void processData(const Packet& p);
    bool waitSendCurrentBlock();
    bool sendFileBlocking();
    void writeOutCmd(eCMD, const QByteArray& = QByteArray());
    void sendStartRequest();
    QString i_clientAddrs;
    quint16 i_clientDataPort;
    QTcpSocket* i_tcpDataSkt;
    quint16 i_packetSize;   //used when nonblocking rcv
    int i_cmd_counter;

    DHtcpEncoder* i_encoder;
    quint32 i_blockNo;
};

}
#endif // DHTCP_H
