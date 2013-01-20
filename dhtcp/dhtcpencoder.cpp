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

    quint64 offset = RAW_BLOCK_SIZE * i;
    if( offset +1 > i_rawFile.size() ) return QByteArray();
    b.fileSize = i_rawFile.size();

    QFile f(i_rawFile.absoluteFilePath());
    if( f.open(QIODevice::ReadOnly) ) return QByteArray();
    f.seek(offset);
    b.data = f.read((quint64)RAW_BLOCK_SIZE);
    b.offsetFrom = offset;
    b.offsetTo = offset + b.data.size();
    f.close();

    return b.toArray();
}
}
