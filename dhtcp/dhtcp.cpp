#include "dhtcp.h"

namespace DHtcp{

DHtcp::DHtcp(const QByteArray arg, QObject *parent) :
    DataHandler(parent),i_isInitOk(false)
{
    QDataStream in(arg);
    in.setVersion(QDataStream::Qt_4_0);
    in >> i_clientAddrs;
    in >> i_clientDataPort;

    qDebug() << "DHtcp::DHtcp() serve client @ "
             << i_clientAddrs
             << ":" << i_clientDataPort;

    i_tcpDataSkt.connectToHost(QHostAddress(i_clientAddrs),
                               i_clientDataPort);
    i_isInitOk = i_tcpDataSkt.waitForConnected(WAIT_CONNECT_TIMEOUT);
}

QByteArray DHtcp::getInitProtocAckArg() //local ip, data port
{
    QByteArray arg;
    QDataStream out(&arg, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << i_tcpDataSkt.localAddress().toString();
    out << (quint16)i_tcpDataSkt.localPort();
    return arg;
}

bool DHtcp::isInitOk()
{
    return i_isInitOk;
}

}
