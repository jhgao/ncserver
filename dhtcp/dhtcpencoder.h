#ifndef DHTCPENCODER_H
#define DHTCPENCODER_H

#include <QObject>
#include <QFileInfo>
#include <QFile>

#include "protocol/rawblock.h"

#define RAW_BLOCK_SIZE (1000) //1000bytes

namespace DHtcp{
class DHtcpEncoder : public QObject
{
    Q_OBJECT
public:
    explicit DHtcpEncoder(QObject *parent = 0);
    bool setRawFile(QString absPath);
    bool isReady();
    quint64 getBlockNum();
    QByteArray getBlock(quint32);
signals:
    
public slots:
private:
    QFileInfo i_rawFile;
    bool i_isReady;
};
}

#endif // DHTCPENCODER_H
