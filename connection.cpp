#include "connection.h"

Connection::Connection(int socketDescriptor, QObject *parent) :
    QTcpSocket(parent),packetSize(0),i_cmd_counter(0),i_dh(0),
    i_protoc(PROTOC_NONE),i_socketDescriptor(socketDescriptor)
{
    this->setSocketDescriptor(socketDescriptor);
    connect(this, SIGNAL(readyRead()),
            this, SLOT(onControlSktReadyRead()));
    connect(this, SIGNAL(disconnected()),   //TODO: when to finish
            this, SIGNAL(sig_ConnectionFinished()));

    qDebug() << ">>> Connection() from "
             << this->peerAddress().toString()
             << " : " << this->peerPort();
}

void Connection::onControlSktReadyRead()
{
    packetSize = 0;

    //get packet size
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_0);
    if (packetSize == 0) {
        if (this->bytesAvailable() < (int)sizeof(quint16)){
            qDebug() << "\t E: packet size wrong"
                     << this->bytesAvailable()
                     << "/"
                     << (int)sizeof(quint16);;
            return;
        }
        in >> packetSize;
    }

    //ensure data size available
    if (this->bytesAvailable() < packetSize){
        qDebug() << "\t E: not enough data bytes"
                 << this->bytesAvailable()
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
//            processData(p);
            qDebug() << "\t Data from CMD link";
            break;
        default:
            qDebug() << "\t unknown packet type";
        }
    }
}

void Connection::processCMD(const Packet &p)
{
    i_cmd_counter++;

    QDataStream args(p.getCMDarg());
    args.setVersion(QDataStream::Qt_4_0);

    switch(p.getCMD()){
    case DATALINK_DECLARE:
        psCmdDbg("DATALINK_DECLARE",p.getCMDarg().toHex());
        args >> i_protoc;
        args >> i_protocArg;
        this->processProtocolDeclare((eProtocTypes)i_protoc,i_protocArg);
        break;
    default:
        qDebug() << "\t unknown cmd" << p.getCMD();
    }
}

bool Connection::initDataHandler(const eProtocTypes type, const QByteArray protocArg)
{
    switch( type){
    case PROTOC_NONE:
        qDebug() << "\t protoc none";
        return false;
        break;
    case PROTOC_TCP:
        i_dh = new DHtcp(protocArg,this);
        break;
    case PROTOC_UDP:
        i_dh = new DHudp(protocArg,this);
        break;
    default:
        qDebug() << "\t unknown protoc type" << type;
        return false;
    }
    if( i_dh ){   //init dh success
        connect(i_dh, SIGNAL(sig_writeOutCmd(eControl_CMD,QByteArray)),
                this, SLOT(writeOutCMD(eControl_CMD,QByteArray)));

        return true;
    }else    //init dh failed
        return false;
}

void Connection::writeOutCMD(const eControl_CMD cmd, const QByteArray arg)
{
    if(!this->isWritable()) return;

    Packet p(cmd,arg);
    this->write(p.genPacket());
}

QString Connection::psCmdDbg(QString cmd, QString arg)
{
    QString dbg;
    dbg = " cmd " + QString::number(i_cmd_counter);
    dbg += " [" + cmd + "] ";
    dbg += arg;
    dbg += "\tfrom " + this->peerAddress().toString()
            + " time " + QTime::currentTime().toString();

    qDebug() << dbg;
    return dbg;
}

void Connection::processProtocolDeclare(const eProtocTypes type, const QByteArray protocArg)
{
    /* init datahandler */
    QByteArray protocAckArg;
    if( initDataHandler(type,protocArg )){
        protocAckArg = i_dh->getInitProtocAckArg();
    }

    /* gen ack ( CMD , (protocol , protocl arugments)) */
    QByteArray ack;
    QDataStream out(&ack,QIODevice::WriteOnly);
    out << (quint16) type;
    out << protocAckArg;
    writeOutCMD(DATALINK_DECLARE_ACK,ack);
}
