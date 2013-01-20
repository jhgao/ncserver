#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QTime>
#include "protocol/packet.h"
#include "protocol/protocoltypes.h"
#include "datahandler.h"

//datahandler & encoders
#include "dhtcp/dhtcp.h"
#include "dhudp/dhudp.h"

class Connection : public QTcpSocket
{
    Q_OBJECT
public:
    explicit Connection(int socketDescriptor,
                             QObject *parent = 0);
    
signals:
    void sig_ConnectionFinished();
    
public slots:
private slots:
    void onControlSktReadyRead();
    void processCMD(const Packet& p);
    bool initDataHandler(const eProtocTypes type, const QByteArray protocArg);

    void writeOutCMD(const eControl_CMD cmd,
                     const QByteArray arg = QByteArray());

private:
    QString psCmdDbg(QString cmd, QString arg = QString());
    void processProtocolDeclare(const eProtocTypes type, const QByteArray protocArg);
    int i_socketDescriptor;
    quint16 packetSize; //for reading packet
    int i_cmd_counter;

    //protocol
    quint16     i_protoc;
    QByteArray     i_protocArg;

    //dh
    DataHandler *i_dh;
};

#endif // CONNECTION_H
