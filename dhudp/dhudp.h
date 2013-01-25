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
#include "fragment.h"

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

    void sendCurrentCycleFrags();
private:
    void writeOutCmd(quint16, const QByteArray& = QByteArray());
    void writeOutData(const QByteArray a);
    void processCMD(const Packet& p);
    QString psCmdDbg(QString cmd, QString arg = QString());

    void startSending();
    void stopSending();
    bool toNextCycle();

    bool toCycle(quint32);
    void genCycleBlocks();  //encode
    void genCycleFragments();

    bool touch(QString);

    QTcpSocket* i_tcpCmdSkt;
    int i_cmd_counter;
    quint16 i_cmdPacketSize;   //used when nonblocking rcv
    QString i_clientCmdAddrs;
    quint16 i_clientCmdListingPort;

    QUdpSocket* i_udpDataSkt;
    QString i_clientDataAddrs;
    quint16 i_clientDataListingPort;

    DHudpEncoder *i_encoder;
    QTimer *i_sendFragsTimer;

    //
    bool i_isSending;
    quint32 i_cyc;      //current cycle number
    QList<QByteArray> i_cycleFragments;
    QList<QByteArray> i_cycleBlocks;
};

}//namespace nProtocUDP

#endif // DHUDP_H
