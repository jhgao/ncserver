#include "waitingserver.h"

WaitingServer::WaitingServer(QObject *parent) :
    QTcpServer(parent),icount(0)
{
}

void WaitingServer::incomingConnection(int socketDescriptor)
{
    qDebug() << "WaitingServer::incomingConnection()";
    icount++;

    ConnectionThread* ct = new ConnectionThread(socketDescriptor,this);
    listOfConThreads.append(ct);
    connect(ct, SIGNAL(sig_ConnectionFinished(ConnectionThread*)),
            this, SLOT(onConnectionFinished(ConnectionThread*)));
    ct->start();    //TODO: priority

    qDebug() << "\t working link:" << listOfConThreads.size()
             << " / in count" << icount;
}

void WaitingServer::onConnectionFinished(ConnectionThread *ct)
{
    listOfConThreads.removeOne(ct);
}
