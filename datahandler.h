#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include "protocol/cmd_define.h"
#include "protocol/protocoltypes.h"

class DataHandler : public QObject
{
    Q_OBJECT
public:
    explicit DataHandler(QObject *parent = 0);
    /*
     * after a specific protocol data handler is initialized,
     * conotrol link will call this func and send ack back
     * to client.
     */
    virtual QByteArray getInitProtocAckArg() = 0;
signals:
    void sig_writeOutCmd(eControl_CMD,QByteArray);
    
public slots:
protected:
    /* signal parent object to write out CMD */
    void sigWriteOutCmd(const eControl_CMD, const QByteArray);
};

#endif // DATAHANDLER_H
