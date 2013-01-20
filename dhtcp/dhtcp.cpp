#include "dhtcp.h"

DHtcp::DHtcp(const QByteArray arg, QObject *parent) :
    DataHandler(parent)
{
    qDebug() << "DHtcp::DHtcp()";
}

QByteArray DHtcp::getInitProtocAckArg()
{
    QByteArray arg;
    QDataStream out(&arg, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)15;
    return arg;
}
