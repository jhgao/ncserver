#include "dhudp.h"

namespace DHudp{

DHudp::DHudp(const QByteArray arg, QObject *parent) :
    DataHandler(parent)
{
    qDebug() << "DHudp::DHudp()";
}

QByteArray DHudp::getInitProtocAckArg()
{
    return QByteArray();
}

bool DHudp::isInitOk()
{
    return true;
}

}
