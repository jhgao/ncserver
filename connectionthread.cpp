#include "connectionthread.h"

ConnectionThread::ConnectionThread(int socketDescriptor, QObject *parent) :
    QThread(parent),i_socketDescriptor(socketDescriptor)
{
    connect(this, SIGNAL(finished()), this, SLOT(onFinished()));
    connect(this, SIGNAL(error(QTcpSocket::SocketError)),
            this, SLOT(onError(QTcpSocket::SocketError)));
}

void ConnectionThread::run()
{
    Connection con(i_socketDescriptor);
    connect(&con,SIGNAL(sig_ConnectionFinished()),
            this,SLOT(quit()));
    exec();
}


void ConnectionThread::onFinished()
{
    emit sig_ConnectionFinished(this);
    this->deleteLater();
}


void ConnectionThread::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << this << socketError;
}
