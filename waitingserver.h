#ifndef WAITINGSERVER_H
#define WAITINGSERVER_H

#include <QTcpServer>
#include "connection.h"

class WaitingServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit WaitingServer(QObject *parent = 0);

protected:
    void incomingConnection(int socketDescriptor);

private slots:
    void onConnectionFinished(Connection *con);
    
private:
    QList<Connection*> listOfConnections;   //for server statics
    int icount;
};

#endif // WAITINGSERVER_H
