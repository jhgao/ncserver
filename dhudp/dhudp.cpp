#include "dhudp.h"
#include <QDebug>

namespace nProtocUDP{

DHudp::DHudp(const QByteArray arg, QObject *parent) :
    DataHandler(parent),i_encoder(0),
    i_tcpCmdSkt(0),i_cmd_counter(0),i_cmdPacketSize(0),
    i_clientCmdListingPort(0),i_clientDataListingPort(0),
    i_udpDataSkt(0),i_cyc(0),i_sendFragsTimer(0),
    i_isSending(false)
{
    i_tcpCmdSkt = new QTcpSocket(this);
    i_udpDataSkt = new QUdpSocket(this);

    i_encoder = new DHudpEncoder(this);
    ServerConfig* sc = ServerConfig::get_instance();
    i_encoder->setRawFile(sc->getRawFileName());
//    i_encoder->initClib(sc->getRawFileName());

    i_sendFragsTimer = new QTimer(this);
    connect(i_sendFragsTimer, SIGNAL(timeout()),
            this, SLOT(sendCurrentCycleFrags()));


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
    this->stopSending();
}

void DHudp::sendCurrentCycleFrags()
{
    i_sendFragsTimer->stop();   //pause

    if( i_cycleFragments.isEmpty() ){
        this->genCycleBlocks();
        this->genCycleFragments();
    }

    for(int i = 0; i< i_cycleFragments.size(); ++i){
        this->writeOutData(i_cycleFragments.at(i));
    }

    i_sendFragsTimer->start();  //resume
}

void DHudp::writeOutCmd(quint16 cmd, const QByteArray &arg)
{
    if(!i_tcpCmdSkt) return;

    Packet p(cmd,arg);
    i_tcpCmdSkt->write(p.genPacket());
}

void DHudp::writeOutData(const QByteArray a)
{
    if(i_udpDataSkt){
        Packet p(a);
        QByteArray dg = p.genPacket();  //dg:datagram
        quint64 wroteSize = i_udpDataSkt->write(dg,dg.size());

        if( wroteSize != dg.size() )
        qDebug() << "Connection::writeOutData() _wrote out wrong size"
                 << wroteSize << "/" << dg.size();
    }
    else{
        qDebug() << "Connection::writeOutData() _no socket available";
    }
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
        psCmdDbg("CON_NEXT",QString::number(i_cyc)+"++");
        if( (i_cyc+1) == i_encoder->getTotalCycleNum() ){
            qDebug() << "\t DHudp::toNextCycle() already last cyc";
            this->writeOutCmd(ALA_LAST_CYCLE);
        }else {
            this->toNextCycle();
        }
        break;
    case CON_CHG_CYC:
        psCmdDbg("CON_CHG_CYC",QString::number(i_cyc) + " -> " +
                 QString::number(
                     QVariant(p.getCMDarg()).toULongLong()));
        if(p.getCMDarg().size() != 0){
            qlonglong tgtCyc = QVariant(p.getCMDarg()).toULongLong();
            if( tgtCyc != i_cyc){
                if( !this->toCycle(tgtCyc))
                    this->writeOutCmd(ALA_OUTRANGE_CYC);
            }
        }
        break;
    case ALA_DONE:
        psCmdDbg("ALA_DONE");
        this->stopSending();
        i_tcpCmdSkt->disconnectFromHost();
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
    dbg = " [DHudp] cmd " + QString::number(i_cmd_counter);
    dbg += " [" + cmd + "] ";
    dbg += arg;
    dbg += "\tfrom " + i_tcpCmdSkt->peerAddress().toString()
            + " time " + QTime::currentTime().toString();

    qDebug() << dbg;
    return dbg;
}

void DHudp::startSending()
{
    qDebug() << "DHudp::startSending() to"
             << i_clientDataAddrs << ":" << i_clientDataListingPort;
    i_isSending = true;
    i_sendFragsTimer->start(SEND_FRAGMENT_INTERVAL);
}

void DHudp::stopSending()
{
    qDebug() << "DHudp::stopSending()";
    i_sendFragsTimer->stop();
    i_isSending = false;
}

bool DHudp::toNextCycle()
{
    return this->toCycle(i_cyc + 1);
}

bool DHudp::toCycle(quint32 tgtCyc)
{
    quint32 lastCyc = i_encoder->getTotalCycleNum() -1;

    if( tgtCyc > lastCyc ){
        qDebug() << "DHudp::toCycle() out of range";
        return false;
    }else{
        i_sendFragsTimer->stop();
        i_cyc = tgtCyc;
        this->genCycleBlocks();
        this->genCycleFragments();
        if(i_isSending) this->startSending();
        return true;
    }
}

void DHudp::genCycleBlocks()
{
    quint32 baseNumber = i_cyc * ONE_CYCLE_BLOCKS;
    QByteArray b;   //temp block
    i_cycleBlocks.clear();

    //TODO last cycle may have lesser blocks
    for( int i = 0; i < i_encoder->blockNumInCycle(i_cyc); ++i){
        b.clear();
        b = i_encoder->getEncodedBlockCpp(baseNumber + i);
        i_cycleBlocks.append(b);
    }
    qDebug() << "DHudp::gen Cycle:"
             << i_cyc << "blocks:"
             << i_cycleBlocks.size();
}

void DHudp::genCycleFragments()
{
    QByteArray b;
    i_cycleFragments.clear();

    for( int blockNo = 0; blockNo<i_cycleBlocks.size(); ++blockNo){
        b.clear();
        b = i_cycleBlocks.at(blockNo);

        //each block
        int fragLimit = ( b.size() + FRAGMENT_SIZE -1) / FRAGMENT_SIZE;
        for ( int fragNo = 0; fragNo< fragLimit; ++fragNo){
            //constructs many fragments into a list
            Fragment frag(i_cyc,
                          blockNo,
                          b.size(),
                          fragNo,
                          fragNo*FRAGMENT_SIZE,
                          b.mid( (fragNo*FRAGMENT_SIZE), FRAGMENT_SIZE));
            // the last frag size is correct, because mid returns bytes till the end

            i_cycleFragments.append(frag.toArray());
        }
    }

    qDebug() << "DHudp::gen Cycle:"
             << i_cyc << "Fragments:"
             << i_cycleFragments.size();
}

bool DHudp::touch(QString aFilePath)
{
    if( QFile::exists(aFilePath)) return true;

    QFile f(aFilePath);
    bool rst = f.open(QIODevice::ReadWrite);
    f.close();
    return rst;
}

}//namespace nProtocUDP
