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

class Connection : public QTcpSocket
{
    Q_OBJECT
public:
    explicit Connection(int socketDescriptor,
                             QObject *parent = 0);
    
signals:
    void sig_ConnectionFinished(Connection*);
    
public slots:
private slots:
    void onControlSktReadyRead();
    void processCMD(const Packet& p);
    QByteArray initDataHandler(eProtocTypes type, QByteArray protocArg);

    void writeOutCMD(eControl_CMD cmd,
                     QByteArray arg = QByteArray());

private:
    QString psCmdDbg(QString cmd, QString arg = QString());
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
