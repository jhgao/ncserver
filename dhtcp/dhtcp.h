#ifndef DHTCP_H
#define DHTCP_H

#include "datahandler.h"

class DHtcp : public DataHandler
{
    Q_OBJECT
public:
    explicit DHtcp(quint16,QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // DHTCP_H
