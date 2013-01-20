#ifndef DHTCP_H
#define DHTCP_H

#include <QDataStream>
#include <QTcpSocket>
#include <QHostAddress>
#include "datahandler.h"
#include "protocol/cmd_define.h"

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

private:
    bool i_isInitOk;
    QString i_clientAddrs;
    quint16 i_clientDataPort;
    QTcpSocket i_tcpDataSkt;
};

}
#endif // DHTCP_H
