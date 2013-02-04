#include "waitingserver.h"

WaitingServer::WaitingServer(QObject *parent) :
    QTcpServer(parent),icount(0)
{
}

void WaitingServer::incomingConnection(int socketDescriptor)
{
    qDebug() << "WaitingServer::incomingConnection()";
    icount++;

    Connection *con = new Connection(socketDescriptor);
    ExecThread *exet = new ExecThread(this);

    connect(con, SIGNAL(sig_ConnectionFinished(Connection*)),
            this, SLOT(onConnectionFinished(Connection*)));

    con->moveToThread(exet);
    exet->start(); //TODO: priority

    i_ConMapList.insert(con,exet);

    qDebug() << "\t working link / incoming count: "
             << i_ConMapList.size()
             << "/" << icount;
}

void WaitingServer::onConnectionFinished(Connection *con)
{
    qDebug() << "WaitingServer::onConnectionFinished()" << con;
    ExecThread *exet = i_ConMapList.value(con);
    if(exet){
        exet->quit();
//        exet->deleteLater();  //TODO: clean up
    }
    i_ConMapList.remove(con);
}
