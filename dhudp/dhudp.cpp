#include "dhudp.h"

DHudp::DHudp(const QByteArray arg, QObject *parent) :
    DataHandler(parent)
{
    qDebug() << "DHudp::DHudp()";
}

QByteArray DHudp::getInitAckArg()
{
    return QByteArray();
}
