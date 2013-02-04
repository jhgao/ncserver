#ifndef WAITINGSERVER_H
#define WAITINGSERVER_H

#include <QTcpServer>
#include <QMap>
#include "execthread.h"
#include "connection.h"

class WaitingServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit WaitingServer(QObject *parent = 0);

signals:
    void sig_connectionLogForGui(QString);
    void sig_logForGui(QString);
    void sig_statusString(QString);

protected:
    void incomingConnection(int socketDescriptor);

private slots:
    void onConnectionFinished(Connection *con);
    
private:
    void emitStatusString();
    QMap<Connection*,ExecThread*> i_ConMapList;    //connection and thread map list
    int icount;
};

#endif // WAITINGSERVER_H
