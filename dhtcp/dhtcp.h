#ifndef DHTCP_H
#define DHTCP_H

#include <QTcpSocket>
#include "datahandler.h"
#include "protocol/cmd_define.h"

class DHtcp : public DataHandler
{
    Q_OBJECT
public:
    explicit DHtcp(quint16,QObject *parent = 0);
    
signals:
    
public slots:
private:
    QTcpSocket i_cmdSkt;

    void (*i_fpWriteOutCmd)(eControl_CMD,QByteArray);
};

#endif // DHTCP_H
