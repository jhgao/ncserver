#include "dhudpencoder.h"
#include <QDebug>

namespace nProtocUDP{

DHudpEncoder::DHudpEncoder(QObject *parent) :
    QObject(parent),
    i_blockSize(ENC_BLOCK_SIZE),i_blockNum(0)
{
}
/* encoding without clib */
QByteArray DHudpEncoder::getEncodedBlockCpp(quint64 i)
{
    //coeff
    QByteArray coeff;
    char c1 = (char)0x01;
    coeff.resize(i_blockNum);
    coeff.fill((char)0x00);
    coeff.replace(i,1,&c1,1);

    //read raw Block
    quint64 offset = i_blockSize*i;
    if( offset >= i_rawFile.size()){
        qDebug() << "\t Err: encoding block out of file";
        return QByteArray();
    }

    QByteArray rawBlock;
    QFile rawFile(i_rawFile.absoluteFilePath());
    rawFile.open(QIODevice::ReadOnly);
    rawFile.seek(offset);
    rawBlock = rawFile.read(i_blockSize);
    rawFile.close();
    if(rawBlock.size() < i_blockSize){
        qDebug() << "\t Err: read raw block not enough size";
        rawBlock.resize(i_blockSize);
        rawBlock.replace(rawBlock.size(),
                         i_blockSize - rawBlock.size(),
                         0x00);
    }

    //gen encoded block
    QByteArray encBlock;
    //TODO encode
    encBlock = coeff.append(rawBlock);

    return encBlock;
}

bool DHudpEncoder::setRawFile(QString absPath)
{
    if(!QFile::exists(absPath)){
        qDebug() << "\t" <<  absPath << "not exist";
        return false;
    }

    i_rawFile.setFile(absPath);
    i_blockNum = this->getRawFileBlockNum();
    return true;
}

bool DHudpEncoder::setEncodedFile(QString fp)
{
    if(QFile::exists(fp)){
        i_encodedFile.setFile(fp);
        return true;
    }
    else {
        if(! this->touch(fp) ) return false;
        else i_encodedFile.setFile(fp);
        return true;
    }
}

QByteArray DHudpEncoder::getDecoderParameters() const
{
    DecParams p;
    p.rawFileLength = i_rawFile.size();
    p.inBlockCoeffLen = this->getRawFileBlockNum();
    p.inBlockDataSize = ENC_BLOCK_SIZE;
    p.totalEncBlocks = this->getRawFileBlockNum();
    p.oneCycleBlockNum = ONE_CYCLE_BLOCKS;
    p.totalCycleNum = this->getTotalCycleNum();
    return p.toArray();
}

quint64 DHudpEncoder::getRawFileBlockNum() const
{
    return (i_rawFile.size() + ENC_BLOCK_SIZE -1 ) / ENC_BLOCK_SIZE;
}

quint32 DHudpEncoder::getTotalCycleNum() const
{
    return (getRawFileBlockNum() + ONE_CYCLE_BLOCKS - 1) / ONE_CYCLE_BLOCKS;
}

quint32 DHudpEncoder::blockNumInCycle(quint32 cyc) const
{
    quint32 n;
    if( cyc + 1 < getTotalCycleNum()
            || 0 == getRawFileBlockNum() % ONE_CYCLE_BLOCKS){
         n = ONE_CYCLE_BLOCKS;
    }else if( cyc +1 == getTotalCycleNum()){
        n = getRawFileBlockNum() % ONE_CYCLE_BLOCKS;
    }else {
        n = 0;
    }
    return n;
}

bool DHudpEncoder::touch(QString aFilePath)
{
    if( QFile::exists(aFilePath)) return true;

    QFile f(aFilePath);
    bool rst = f.open(QIODevice::ReadWrite);
    f.close();
    return rst;
}

}//namespace nProtocUDP
