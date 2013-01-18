#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QTime>
#include "protocol/packet.h"

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
    void writeOutCMD(eControl_CMD cmd,
                     QByteArray arg = QByteArray());

private:
    QString psCmdDbg(QString cmd, QString arg = QString());
    quint16 packetSize; //for reading packet
    int i_cmd_counter;
};

#endif // CONNECTION_H
