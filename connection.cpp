#include "connection.h"

Connection::Connection(int socketDescriptor, QObject *parent) :
    QTcpSocket(parent),packetSize(0),i_cmd_counter(0)
{
    this->setSocketDescriptor(socketDescriptor);
    connect(this, SIGNAL(readyRead()),
            this, SLOT(onControlSktReadyRead()));
}

void Connection::onControlSktReadyRead()
{
    qDebug() << "Connection::onControlSktReadyRead()";
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
    qDebug() << "Connection::processCMD()" << p.getCMD();
    i_cmd_counter++;

    switch(p.getCMD()){
    case CON_CONNECTING:
        psCmdDbg("CON_CONNECTING");
        writeOutCMD(CON_CONNECTED);
        break;
    default:
        qDebug() << "\t unknown cmd" << p.getCMD();
    }
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
