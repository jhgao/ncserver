#ifndef DHTCP_H
#define DHTCP_H

#include <QTcpSocket>
#include "datahandler.h"
#include "protocol/cmd_define.h"

class DHtcp : public DataHandler
{
    Q_OBJECT
public:
    explicit DHtcp(QByteArray arg,QObject *parent = 0);
    
signals:
    
public slots:
private:
};

#endif // DHTCP_H
