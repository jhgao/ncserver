#include "dhudp.h"
#include <QDebug>

namespace nProtocUDP{

DHudp::DHudp(const QByteArray arg, QObject *parent) :
    DataHandler(parent),i_tcpCmdSkt(0),
    i_cmd_counter(0),i_cmdPacketSize(0),i_dataPacketSize(0),
    i_encoder(0)
{
    i_tcpCmdSkt = new QTcpSocket(this);
    i_encoder = new DHudpEncoder(this);

    ServerConfig* sc = ServerConfig::get_instance();
    i_encoder->setRawFile(sc->getRawFileName());

    QDataStream in(arg);
    in.setVersion(QDataStream::Qt_4_8);
    in >> i_clientAddrs;
    in >> i_clientCmdListingPort;

    qDebug() << "DHudp::DHudp() new UDP client wating at "
             << i_clientAddrs
             << ":" << i_clientCmdListingPort;

    i_tcpCmdSkt->connectToHost(QHostAddress(i_clientAddrs),
                               i_clientCmdListingPort);

    if( i_tcpCmdSkt->waitForConnected(WAIT_FOR_CONNECTED_TIMEOUT) ){
        connect(i_tcpCmdSkt,SIGNAL(readyRead()),
                this, SLOT(onCmdSktReadyRead()));
        connect(i_tcpCmdSkt,SIGNAL(disconnected()),
                this, SLOT(onCmdSktDisconnected()));
    }else{
        qDebug() << "\t Err: can not connect to client";
    }
}

QByteArray DHudp::getInitProtocAckArg()
{
    return QByteArray();
}

bool DHudp::isInitOk()
{
    return true;
}

void DHudp::onCmdSktReadyRead()
{
}

void DHudp::onCmdSktDisconnected()
{
}

void DHudp::writeOutCmd(quint16 cmd, const QByteArray &arg)
{
    if(!i_tcpCmdSkt) return;

    Packet p(cmd,arg);
    i_tcpCmdSkt->write(p.genPacket());
}

void DHudp::processCMD(const Packet &p)
{
    i_cmd_counter++;

    switch(p.getCMD()){
    case CON_START:
        psCmdDbg("CON_START","TODO");
        break;
    case CON_NEXT:
        psCmdDbg("CON_NEXT","TODO");
        break;
    case CON_CHG_CYC:
        psCmdDbg("CON_CHG_CYC",
                 QString::number(QVariant(p.getCMDarg()).toUInt()));
        break;
    case ALA_DONE:
        psCmdDbg("ALA_DONE","TODO");
        break;
    case QUE_DECODE_PARAM:
        psCmdDbg("QUE_DECODE_PARAM","TODO");
        break;
    case ACK_DATA_PORT:
        psCmdDbg("ACK_DATA_PORT","TODO");
        break;
    default:
        psCmdDbg(QString::number(p.getCMD()) + "?UNKNOWN" );
    }
}

QString DHudp::psCmdDbg(QString cmd, QString arg)
{
    QString dbg;
    dbg = " [DHtcp] cmd " + QString::number(i_cmd_counter);
    dbg += " [" + cmd + "] ";
    dbg += arg;
    dbg += "\tfrom " + i_tcpCmdSkt->peerAddress().toString()
            + " time " + QTime::currentTime().toString();

    qDebug() << dbg;
    return dbg;
}

void DHudp::processData(const Packet &p)
{
    qDebug() << "TODO:  DHudp::processData() ";
}

}//namespace nProtocUDP
