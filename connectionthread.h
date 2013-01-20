#ifndef CONNECTIONTHREAD_H
#define CONNECTIONTHREAD_H

#include <QThread>
#include "connection.h"

class ConnectionThread : public QThread
{
    Q_OBJECT
public:
    explicit ConnectionThread(int socketDescriptor, QObject *parent = 0);
    void run();

signals:
    void error(QTcpSocket::SocketError socketError);
    void sig_ConnectionFinished(ConnectionThread*);

private slots:
    void onError(QTcpSocket::SocketError socketError);
    void onFinished();

private:
    int i_socketDescriptor;
};

#endif // CONNECTIONTHREAD_H
