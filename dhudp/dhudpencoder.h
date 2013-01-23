#ifndef DHUDPENCODER_H
#define DHUDPENCODER_H

#include <QObject>

namespace nProtocUDP{
class DHudpEncoder : public QObject
{
    Q_OBJECT
public:
    explicit DHudpEncoder(QObject *parent = 0);
    
signals:
    
public slots:
    
};
}//namespace nProtocUDP

#endif // DHUDPENCODER_H
