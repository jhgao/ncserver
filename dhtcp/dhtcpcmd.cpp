#include "dhtcpcmd.h"
namespace DHtcp{

DHtcpCmd::DHtcpCmd(eCMD aCmd, const QByteArray &aArg) :
    cmd((quint16)aCmd),arg(aArg)
{
}

QByteArray DHtcpCmd::genPacket()
{
    QByteArray p;
    QDataStream out(&p, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)cmd;
    out << arg;
    out.device()->seek(0);
    out << (quint16)(p.size() - sizeof(quint16));
    return p;
}

}
