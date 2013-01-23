#include "dhudpencoder.h"

namespace nProtocUDP{
DHudpEncoder::DHudpEncoder(QObject *parent) :
    QObject(parent)
{
}

bool DHudpEncoder::setRawFile(QString absPath)
{
    if(!QFile::exists(absPath)) return false;

    i_rawFile.setFile(absPath);
    return true;
}

QByteArray DHudpEncoder::getDecoderParameters() const
{
    DecParams p;
    p.rawFileLength = i_rawFile.size();
    p.inBlockCoeffLen = this->getRawFileBlockNum();
    p.inBlockDataSize = ENC_BLOCK_SIZE;
    p.totalEncBlocks = this->getRawFileBlockNum();
    return p.toArray();
}

quint64 DHudpEncoder::getRawFileBlockNum() const
{
    return (i_rawFile.size() + ENC_BLOCK_SIZE -1 ) / ENC_BLOCK_SIZE;
}

}//namespace nProtocUDP
