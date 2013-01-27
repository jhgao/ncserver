#ifndef DHUDPENCODER_H
#define DHUDPENCODER_H

#include <QObject>
#include <QFileInfo>
#include "decparams.h"
#include "dhudpprotocol.h"

namespace nProtocUDP{

class DHudpEncoder : public QObject
{
    Q_OBJECT
public:
    explicit DHudpEncoder(QObject *parent = 0);

    QByteArray getEncodedBlockCpp(quint64 i);
    bool setRawFile(QString absPath);
    bool setEncodedFile(QString);
    QByteArray getDecoderParameters() const;
    quint64 getRawFileBlockNum() const;
    quint32 getTotalCycleNum() const;
    quint32 blockNumInCycle(quint32) const;
signals:
    
public slots:
private:
    bool touch(QString aFilePath);

    QFileInfo i_rawFile;
    quint64 i_blockSize;
    quint64 i_blockNum;

    QFileInfo i_encodedFile;
};
}//namespace nProtocUDP

#endif // DHUDPENCODER_H
