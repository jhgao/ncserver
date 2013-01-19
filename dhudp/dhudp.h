#ifndef DHUDP_H
#define DHUDP_H

#include "datahandler.h"
#include "protocol/cmd_define.h"

class DHudp : public DataHandler
{
    Q_OBJECT
public:
    explicit DHudp(QByteArray arg, QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // DHUDP_H
