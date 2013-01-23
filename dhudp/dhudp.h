#ifndef DHUDP_H
#define DHUDP_H

#include <QDataStream>
#include <QUdpSocket>
#include <QTcpServer>
#include "datahandler.h"
#include "protocol/cmd_define.h"
#include "protocol/packet.h"
#include "serverconfig.h"

#include "dhudpprotocol.h"
#include "dhudpencoder.h"

namespace nProtocUDP{

static const int WAIT_FOR_CONNECTED_TIMEOUT  = 5000;
static const int WAIT_FOR_BYTES_WRITTEN_TIMEOUT = 5000;

class DHudp : public DataHandler
{
    Q_OBJECT
public:
    explicit DHudp(const QByteArray arg, QObject *parent = 0);
    QByteArray getInitProtocAckArg();
    bool isInitOk();

signals:
    
public slots:
private slots:
    void onCmdSktReadyRead();
    void onCmdSktDisconnected();
private:
    void writeOutCmd(quint16, const QByteArray& = QByteArray());
    void processCMD(const Packet& p);
    QString psCmdDbg(QString cmd, QString arg = QString());
    void processData(const Packet& p);

    void startSending();
    QTcpSocket* i_tcpCmdSkt;
    int i_cmd_counter;

    QString i_clientAddrs;
    quint16 i_clientCmdListingPort;

    quint16 i_cmdPacketSize;   //used when nonblocking rcv
    quint16 i_dataPacketSize;   //used when nonblocking rcv
    DHudpEncoder *i_encoder;
};

}//namespace nProtocUDP

#endif // DHUDP_H
