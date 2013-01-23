#ifndef DHUDPENCODER_H
#define DHUDPENCODER_H

#include <QObject>
#include <QFileInfo>

namespace nProtocUDP{
class DHudpEncoder : public QObject
{
    Q_OBJECT
public:
    explicit DHudpEncoder(QObject *parent = 0);
    bool setRawFile(QString absPath);
    
signals:
    
public slots:
private:
    QFileInfo i_rawFile;
};
}//namespace nProtocUDP

#endif // DHUDPENCODER_H
