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

    i_ConMapList.insert(con,exet);
    connect(con, SIGNAL(sig_ConnectionFinished(Connection*)),
            this, SLOT(onConnectionFinished(Connection*)));
    connect(con, SIGNAL(sig_logForGui(QString)),
            this, SIGNAL(sig_connectionLogForGui(QString)));


    QString s("New connection from");
    s += con->peerAddress().toString()
            + ":" + QString::number(con->peerPort());

    QString counts;
    counts += QString::number(i_ConMapList.size())
            + "/" + QString::number(icount);

    emit sig_logForGui(s + " [" + counts + "]" + "<" + QString::number(reinterpret_cast<unsigned long int>(con)) + ">");

    this->emitStatusString();

    con->moveToThread(exet);
    exet->start(); //TODO: priority
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
    this->emitStatusString();
}

void WaitingServer::emitStatusString()
{
    QString stats;
     stats += "working link / incoming count: "
             + QString::number(i_ConMapList.size())
             + "/" + QString::number(icount);

      emit sig_statusString(stats);
     qDebug() << "\t" << stats;
}
