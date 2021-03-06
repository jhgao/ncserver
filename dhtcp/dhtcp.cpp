#include "dhtcp.h"

namespace nProtocTCP{

DHtcp::DHtcp(const QByteArray arg, QObject *parent) :
    DataHandler(parent),i_cmd_counter(0),
    i_tcpDataSkt(0),i_packetSize(0),i_encoder(0),i_blockNo(0)
{
    i_tcpDataSkt = new QTcpSocket(this);
    i_encoder = new DHtcpEncoder(this);

    ServerConfig* sc = ServerConfig::get_instance();
    i_encoder->setRawFile(sc->getRawFileName());

    QDataStream in(arg);
    in.setVersion(QDataStream::Qt_4_8);
    in >> i_clientAddrs;
    in >> i_clientDataPort;

    qDebug() << "DHtcp::DHtcp() serve client @ "
             << i_clientAddrs
             << ":" << i_clientDataPort;

    i_tcpDataSkt->connectToHost(QHostAddress(i_clientAddrs),
                               i_clientDataPort);

    if( i_tcpDataSkt->waitForConnected(WAIT_FOR_CONNECTED_TIMEOUT) ){
        connect(i_tcpDataSkt,SIGNAL(readyRead()),
                this, SLOT(onDataSktReadyRead()));
    }
}

QByteArray DHtcp::getInitProtocAckArg() //local ip, data port
{
    QByteArray arg;
    QDataStream out(&arg, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << i_tcpDataSkt->localAddress().toString();
    out << (quint16)i_tcpDataSkt->localPort();
    return arg;
}

bool DHtcp::isInitOk()
{
    if( !i_tcpDataSkt ) return false;
    if( !i_encoder->isReady()) return false;
    return true;
}

void DHtcp::onDataSktReadyRead()
{
    //get packet size
    QDataStream in(i_tcpDataSkt);
    in.setVersion(QDataStream::Qt_4_8);
    if (i_packetSize == 0) {
        if (i_tcpDataSkt->bytesAvailable() < (int)sizeof(quint16)){
            qDebug() << "\t E: packet size wrong"
                     << i_tcpDataSkt->bytesAvailable()
                     << "/"
                     << (int)sizeof(quint16);;
            return;
        }
        in >> i_packetSize;
    }

    //ensure data size available
    if (i_tcpDataSkt->bytesAvailable() < i_packetSize){
        qDebug() << "\t E: not enough data bytes"
                 << i_tcpDataSkt->bytesAvailable()
                 << "/need "
                 << i_packetSize;
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
            processData(p);
            break;
        default:
            qDebug() << "\t unknown packet type";
        }
    }

    i_packetSize = 0;
}

void DHtcp::processCMD(const Packet &p)
{
    ++i_cmd_counter;

    switch(p.getCMD()){
    case CMD_START:
        psCmdDbg("CMD_START");
        this->sendStartRequest();
        break;
    case FILE_SENT_BLOCKING_GOAHEAD:
        psCmdDbg("FILE_SENT_BLOCKING_GOAHEAD");
        this->sendFileBlocking();
        break;
    case CMD_STOP:
        psCmdDbg("CMD_STOP");
        break;
    default:
        psCmdDbg(QString::number(p.getCMD()) + "?UNKNOWN" );
    }
}

QString DHtcp::psCmdDbg(QString cmd, QString arg)
{
    QString dbg;
    dbg = " [DHtcp] cmd " + QString::number(i_cmd_counter);
    dbg += " [" + cmd + "] ";
    dbg += arg;
    dbg += "\tfrom " + i_tcpDataSkt->peerAddress().toString()
            + " time " + QTime::currentTime().toString();

    qDebug() << dbg;
    return dbg;
}

void DHtcp::processData(const Packet &p)
{
    qDebug() << "DHtcp::processData()"
             << p.getData().toHex();

}

bool DHtcp::waitSendCurrentBlock()
{
    Packet p(i_encoder->getBlock(i_blockNo));
    i_tcpDataSkt->write(p.genPacket());
    return i_tcpDataSkt->waitForBytesWritten(WAIT_FOR_BYTES_WRITTEN_TIMEOUT);
}

bool DHtcp::sendFileBlocking()
{
    i_blockNo = 0;

    QFile rawFile(i_encoder->getRawFileName());

    rawFile.open(QIODevice::ReadOnly);
    while( !rawFile.atEnd() ){
        QByteArray a = rawFile.read(RAW_BLOCK_SIZE);
        i_tcpDataSkt->write(a);

        if(! i_tcpDataSkt->waitForBytesWritten(WAIT_FOR_BYTES_WRITTEN_TIMEOUT)){ //to hardware
            qDebug() << "\t DHtcp failed send at file pos" << rawFile.pos();
            return false;
        }
    }

    i_tcpDataSkt->disconnectFromHost();
    qDebug() << "DHtcp::waitSendFile() done";
    return true;
}

void DHtcp::writeOutCmd(quint16 cmd, const QByteArray &arg)
{
    if(!i_tcpDataSkt) return;

    Packet p(cmd,arg);
    i_tcpDataSkt->write(p.genPacket());
}

void DHtcp::sendStartRequest()
{
    qDebug() << "DHtcp::sendStartRequest()";
    QByteArray arg;
    QDataStream out(&arg,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (quint64) i_encoder->getRawFileSize();

    this->writeOutCmd(FILE_SENT_BLOCKING,arg);
}

}
