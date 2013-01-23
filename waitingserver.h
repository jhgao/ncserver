#ifndef WAITINGSERVER_H
#define WAITINGSERVER_H

#include <QTcpServer>
#include "connectionthread.h"

class WaitingServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit WaitingServer(QObject *parent = 0);

protected:
    void incomingConnection(int socketDescriptor);

private slots:
    void onConnectionFinished(ConnectionThread *ct);
    
private:
    QList<ConnectionThread*> listOfConThreads;   //for server statics
    int icount;
};

#endif // WAITINGSERVER_H
