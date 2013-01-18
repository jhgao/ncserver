#include "waitingserver.h"

WaitingServer::WaitingServer(QObject *parent) :
    QTcpServer(parent),icount(0)
{
}

void WaitingServer::incomingConnection(int socketDescriptor)
{
    qDebug() << "WaitingServer::incomingConnection()";
    icount++;

    Connection* pCon = new Connection(socketDescriptor,this);
    listOfConnections.append(pCon);
    connect(pCon,SIGNAL(sig_ConnectionFinished(Connection*)),
            this,SLOT(onConnectionFinished(Connection*)));

    qDebug() << "\t working link:" << listOfConnections.size()
             << " / in count" << icount
             << "\t" << pCon->peerAddress().toString()
             << ":" << pCon->peerPort();
}

void WaitingServer::onConnectionFinished(Connection *con)
{
    listOfConnections.removeOne(con);
}
