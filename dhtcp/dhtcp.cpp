#include "dhtcp.h"

DHtcp::DHtcp(QByteArray arg, QObject *parent) :
    DataHandler(parent)
{
    QByteArray ackArg;
    QDataStream out(&ackArg, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16) PROTOC_TCP;
    out << ackArg;

    this->sigWriteOutCmd(DATALINK_DECLARE_ACK,
                      ackArg);
}

