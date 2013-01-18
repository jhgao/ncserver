#include "connection.h"

Connection::Connection(int socketDescriptor, QObject *parent) :
    QTcpSocket(parent),packetSize(0),i_cmd_counter(0),i_dh(0),
    i_protoc(PROTOC_NONE),i_protocPort(0),
    i_socketDescriptor(socketDescriptor)
{
    this->setSocketDescriptor(socketDescriptor);
    connect(this, SIGNAL(readyRead()),
            this, SLOT(onControlSktReadyRead()));
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
        case DATALINK_DECLARE:

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
//    qDebug() << "Connection::processCMD()" << p.getCMD();
    i_cmd_counter++;

    QDataStream args(p.getCMDarg());
    args.setVersion(QDataStream::Qt_4_0);

    switch(p.getCMD()){
    case DATALINK_DECLARE:
        psCmdDbg("DATALINK_DECLARE");
        args >> i_protoc;
        args >> i_protocPort;
        writeOutCMD(DATALINK_DECLARE_ACK,
                    initDataHandler((eProtocTypes)i_protoc,i_protocPort));
        break;
    default:
        qDebug() << "\t unknown cmd" << p.getCMD();
    }
}

QByteArray Connection::initDataHandler(eProtocTypes type, quint16 port)
{
    if( PROTOC_NONE == type ){
        qDebug() << "\t protoc none";
        return QByteArray();
    }

    switch( type){
    case PROTOC_TCP:
        i_dh = new DHtcp(port,this);
        break;
    case PROTOC_UDP:
        break;
    default:
        qDebug() << "\t unknown protoc type" << type;
    }
    if( i_dh ){
        connect(i_dh, SIGNAL(sig_writeOutCmd(eControl_CMD,QByteArray)),
                this, SLOT(writeOutCMD(eControl_CMD,QByteArray)));
    }
    //prepare return ack ( protocol , port)
    QByteArray arg;
    QDataStream out(&arg,QIODevice::WriteOnly);
    out << (quint16) type;  //protocol type
    out << (quint16) port;  //protocol port

    return arg;
}

void Connection::writeOutCMD(eControl_CMD cmd, QByteArray arg)
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
