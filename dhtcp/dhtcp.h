#ifndef DHTCP_H
#define DHTCP_H

#include <QDataStream>
#include <QTcpSocket>
#include <QHostAddress>
#include "datahandler.h"
#include "protocol/cmd_define.h"
#include "protocol/packet.h"

#include "dhtcpcmd.h"

namespace DHtcp{

#define WAIT_CONNECT_TIMEOUT 5000

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
    bool i_isInitOk;
    QString i_clientAddrs;
    quint16 i_clientDataPort;
    QTcpSocket* i_tcpDataSkt;
    int i_cmd_counter;
};

}
#endif // DHTCP_H
