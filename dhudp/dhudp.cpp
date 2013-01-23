#include "dhudp.h"
#include <QDebug>

namespace nProtocUDP{

DHudp::DHudp(const QByteArray arg, QObject *parent) :
    DataHandler(parent),i_encoder(0),
    i_tcpCmdSkt(0),i_cmd_counter(0),i_cmdPacketSize(0),
    i_udpDataSkt(0),
    i_clientCmdListingPort(0),i_clientDataListingPort(0),
    i_sendFragsTimer(0)
{
    i_tcpCmdSkt = new QTcpSocket(this);
    i_udpDataSkt = new QUdpSocket(this);
    i_encoder = new DHudpEncoder(this);
    i_sendFragsTimer = new QTimer(this);
    connect(i_sendFragsTimer, SIGNAL(timeout()),
            this, SLOT(sendCurrentCycleFrags()));

    ServerConfig* sc = ServerConfig::get_instance();
    i_encoder->setRawFile(sc->getRawFileName());

    QDataStream in(arg);
    in.setVersion(QDataStream::Qt_4_8);
    in >> i_clientCmdAddrs;
    in >> i_clientCmdListingPort;

    qDebug() << "DHudp::DHudp() new UDP client wating at "
             << i_clientCmdAddrs
             << ":" << i_clientCmdListingPort;

    i_tcpCmdSkt->connectToHost(QHostAddress(i_clientCmdAddrs),
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
    i_cmdPacketSize = 0;

    //get packet size
    QDataStream in(i_tcpCmdSkt);
    in.setVersion(QDataStream::Qt_4_8);
    if (i_cmdPacketSize == 0) {
        if (i_tcpCmdSkt->bytesAvailable() < (int)sizeof(quint16)){
            qDebug() << "\t E: packet size wrong"
                     << i_tcpCmdSkt->bytesAvailable()
                     << "/"
                     << (int)sizeof(quint16);;
            return;
        }
        in >> i_cmdPacketSize;
    }

    //ensure data size available
    if (i_tcpCmdSkt->bytesAvailable() < i_cmdPacketSize){
        qDebug() << "\t E: not enough data bytes"
                 << i_tcpCmdSkt->bytesAvailable()
                 << "/need "
                 << i_cmdPacketSize;
        return;
    }

    //read in data
    QByteArray payloadArrey;
    in >> payloadArrey;

    //analyze payload
    Packet p;
    if( p.fromPayload(payloadArrey)){
        switch(p.getType()){
        case PTYPE_CMD:
            processCMD(p);
            break;
        case PTYPE_DATA:
            qDebug() << "\t Err: Data from CMD link";
            break;
        default:
            qDebug() << "\t DHudp cmd skt: unknown packet type";
        }
    }
}

void DHudp::onCmdSktDisconnected()
{
    i_sendFragsTimer->stop();
}

void DHudp::sendCurrentCycleFrags()
{
    i_sendFragsTimer->stop();
    //TODO send frags
    i_sendFragsTimer->start();
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
        psCmdDbg("CON_START");
        if(p.getCMDarg().size() != 0){
            QDataStream args(p.getCMDarg());
            args.setVersion(QDataStream::Qt_4_8);
            args >> i_clientDataAddrs;
            args >> i_clientDataListingPort;

            i_udpDataSkt->abort();
            i_udpDataSkt->connectToHost(
                        QHostAddress(i_clientDataAddrs),
                        i_clientDataListingPort);

            this->startSending();
        }
        break;
    case CON_NEXT:
        psCmdDbg("CON_NEXT","TODO");
        break;
    case CON_CHG_CYC:
        psCmdDbg("CON_CHG_CYC",
                 QString::number(QVariant(p.getCMDarg()).toUInt()));
        break;
    case ALA_DONE:
        psCmdDbg("ALA_DONE");
        i_sendFragsTimer->stop();
        break;
    case QUE_DECODE_PARAM:
        psCmdDbg("QUE_DECODE_PARAM");
        if(i_encoder){
            this->writeOutCmd(ACK_DECODE_PARAM,
                              i_encoder->getDecoderParameters());
        }
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

void DHudp::startSending()
{
    qDebug() << "TODO: DHudp::startSending() to client"
             << i_clientDataAddrs << ":" << i_clientDataListingPort;

    i_sendFragsTimer->start(SEND_FRAGMENT_INTERVAL);
}

}//namespace nProtocUDP
