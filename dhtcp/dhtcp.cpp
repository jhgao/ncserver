#include "dhtcp.h"

namespace DHtcp{

DHtcp::DHtcp(const QByteArray arg, QObject *parent) :
    DataHandler(parent),i_cmd_counter(0),
    i_tcpDataSkt(0),i_encoder(0),i_blockNo(0)
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
    i_tcpDataSkt->waitForConnected(WAIT_FOR_CONNECTED_TIMEOUT);

    if( i_tcpDataSkt->isOpen() ){
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
    quint16 packetSize = 0;

    //get packet size
    QDataStream in(i_tcpDataSkt);
    in.setVersion(QDataStream::Qt_4_8);
    if (packetSize == 0) {
        if (i_tcpDataSkt->bytesAvailable() < (int)sizeof(quint16)){
            qDebug() << "\t E: packet size wrong"
                     << i_tcpDataSkt->bytesAvailable()
                     << "/"
                     << (int)sizeof(quint16);;
            return;
        }
        in >> packetSize;
    }

    //ensure data size available
    if (i_tcpDataSkt->bytesAvailable() < packetSize){
        qDebug() << "\t E: not enough data bytes"
                 << i_tcpDataSkt->bytesAvailable()
                 << "/need "
                 << packetSize;
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
}

void DHtcp::processCMD(const Packet &p)
{
    ++i_cmd_counter;

    switch(p.getCMD()){
    case CMD_START:
        psCmdDbg("CMD_START");
        this->waitSendFile();
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

bool DHtcp::waitSendFile()
{
    i_blockNo = 0;
    quint64 blockNum = i_encoder->getBlockNum();
    quint64 wroteBytes = 0;
    while( i_blockNo < blockNum ){
        Packet p(i_encoder->getBlock(i_blockNo));

        wroteBytes = i_tcpDataSkt->write(p.genPacket());
        if( wroteBytes < 0 ){
            qDebug() << "\t Err: send block" << i_blockNo
                     <<  p.dbgString();
        }

        if(! i_tcpDataSkt->waitForBytesWritten(WAIT_FOR_BYTES_WRITTEN_TIMEOUT)){ //to hardware
            qDebug() << "\t DHtcp failed send block" << i_blockNo;
            return false;
        }
        ++i_blockNo;
    }
    Packet p(FILE_SENT);
    i_tcpDataSkt->write(p.genPacket());
    qDebug() << "DHtcp::waitSendFile() done, send block" << i_blockNo;
    return true;
}

}
