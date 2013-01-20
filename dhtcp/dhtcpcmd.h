#ifndef DHTCPCMD_H
#define DHTCPCMD_H

#include <QByteArray>
#include <QDataStream>
#include "dhtcpprotocol.h"

namespace DHtcp{
class DHtcpCmd
{
public:
    DHtcpCmd(eCMD,const QByteArray &);
    QByteArray genPacket();

    quint16 cmd;
    QByteArray arg;
};
}

#endif // DHTCPCMD_H
