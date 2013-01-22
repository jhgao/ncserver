#include "dhtcpencoder.h"
#include <QDebug>

namespace DHtcp{
DHtcpEncoder::DHtcpEncoder(QObject *parent) :
    QObject(parent),i_isReady(false)
{
}

bool DHtcpEncoder::setRawFile(QString absPath)
{
    if(!QFile::exists(absPath)) return false;

    i_rawFile.setFile(absPath);
    return true;
}

QString DHtcpEncoder::getRawFileName() const
{
    return i_rawFile.absoluteFilePath();
}

bool DHtcpEncoder::isReady()
{
    if(i_rawFile.exists()) return true;
    else{
        qDebug() << "\t Not exists: " << i_rawFile.absoluteFilePath();
        return false;
    }
}

quint64 DHtcpEncoder::getBlockNum()
{
    return (i_rawFile.size() + RAW_BLOCK_SIZE -1) / RAW_BLOCK_SIZE;
}

QByteArray DHtcpEncoder::getBlock(quint32 i)
{
    RawBlock b;
    b.fileSize = i_rawFile.size();

    qint64 offset = RAW_BLOCK_SIZE * i;
    if( offset +1 > i_rawFile.size() ){
        qDebug() << "\t DHtcpEncoder::getBlock() beyond last block";
        b.offsetFrom = 0;
        b.offsetTo = 0;
        return b.toArray();
    }

    QFile f(i_rawFile.absoluteFilePath());
    if( !f.open(QIODevice::ReadOnly) ){
        qDebug() << "\t DHtcpEncoder::getBlock() open file failed";
        return QByteArray();
    }

    f.seek(offset);
    b.data = f.read((quint64)RAW_BLOCK_SIZE);
    f.close();

    b.offsetFrom = offset;
    b.offsetTo = offset + b.data.size();

    return b.toArray();
}
}
