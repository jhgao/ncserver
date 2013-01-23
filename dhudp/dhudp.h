#ifndef DHUDP_H
#define DHUDP_H

#include <QDataStream>
#include <QUdpSocket>
#include "datahandler.h"
#include "protocol/cmd_define.h"

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
    
};

}

#endif // DHUDP_H
