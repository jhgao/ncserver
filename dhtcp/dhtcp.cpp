#include "dhtcp.h"

DHtcp::DHtcp(quint16 port, QObject *parent) :
    DataHandler(port, parent)
{
    QByteArray arg;
    QDataStream out(&arg, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16) PROTOC_TCP;
    out << (quint16) port;

    this->sigWriteOutCmd(DATALINK_DECLARE_ACK,
                      arg);
}

