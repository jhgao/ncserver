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
    bool setRawFile(QString absPath);
    QByteArray getDecoderParameters() const;
    quint64 getRawFileBlockNum() const;
    
signals:
    
public slots:
private:
    QFileInfo i_rawFile;
};
}//namespace nProtocUDP

#endif // DHUDPENCODER_H
