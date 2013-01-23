#ifndef DHUDP_H
#define DHUDP_H

#include <QDataStream>
#include <QUdpSocket>
#include <QTcpServer>
#include "datahandler.h"
#include "protocol/cmd_define.h"
#include "protocol/packet.h"

#include "dhudpprotocol.h"
#include "dhudpencoder.h"

namespace nProtocUDP{

class DHudp : public DataHandler
{
    Q_OBJECT
public:
    explicit DHudp(const QByteArray arg, QObject *parent = 0);
    QByteArray getInitProtocAckArg();
    bool isInitOk();
signals:
    
public slots:
    void onIncomingTcpCmdConnection();

    void onCmdSktReadyRead();
    void onCmdSktDisconnected();
private:
    void writeOutCmd(quint16, const QByteArray& = QByteArray());
    void processCMD(const Packet& p);
    QString psCmdDbg(QString cmd, QString arg = QString());
    void processData(const Packet& p);

    void startSending();
    QTcpServer* i_tcpCmdServer;
    QString i_ipAddress;    //local ip
    QTcpSocket* i_tcpCmdSkt;
    int i_cmd_counter;

    quint16 i_cmdPacketSize;   //used when nonblocking rcv
    quint16 i_dataPacketSize;   //used when nonblocking rcv
    DHudpEncoder *i_encoder;
};

}//namespace nProtocUDP

#endif // DHUDP_H
