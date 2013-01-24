#include "dhudpencoder.h"
#include <QDebug>

namespace nProtocUDP{

DHudpEncoder::DHudpEncoder(QObject *parent) :
    QObject(parent),i_isInitOk(false),
    i_blockSize(ENC_BLOCK_SIZE),i_blockNum(0)
{
}

DHudpEncoder::~DHudpEncoder()
{
    clearNC(i_cb);
    if(mtab != NULL)
        free(mtab);
    if(dtab != NULL)
        free(dtab);
}

bool DHudpEncoder::initClib(QString rawFn)
{
    qDebug() << "\t load: \"muldiv.tab\"...";
     if ( preNC() == cTRUE ){      //initMulDivTab() @ nc.c
         qDebug() << "\t succeeded.";
     }else{
         qDebug() << "\t failed.";
         _fcloseall( );
     }

     //NC params
     i_cb = (NCCB *)malloc(sizeof(NCCB));
     if( i_cb == NULL){
         qDebug() << "\t malloc NCCB failed";
     }else {
         this->setRawFile(rawFn);
         this->setEncodedFile("Out.wmv");

         //init NC
         i_blockNum = this->getRawFileBlockNum();

         if( cFALSE == initNC(i_cb,
                            i_rawFile.fileName().toAscii().data(),
                            i_blockSize,
                            i_blockNum,
                            i_d)){
             qDebug() << "\t initNC() failed";
             _fcloseall( );
         }
     }
     i_isInitOk = true;
     return true;
}

QByteArray DHudpEncoder::getEncodedBlock(quint64 i)
{
    if( !i_isInitOk ) return QByteArray();

    //data
    QByteArray data = QByteArray::fromRawData(
                (const char *)encodeIdentity(i_cb,i),
                sizeof(FIELD)*i_cb->blockSize
                );

    //coeff
    QByteArray coeff;
    char c1 = (char)0x01;
    coeff.resize(i_blockNum);
    coeff.fill((char)0x00);
    coeff.replace(i,1,&c1,1);

    return coeff.append(data);
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

quint8 DHudpEncoder::blockNumInCycle(quint32 cyc) const
{
    quint8 n;
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
