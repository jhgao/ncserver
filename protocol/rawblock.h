#ifndef RAWBLOCK_H
#define RAWBLOCK_H

#include <QByteArray>
#include <QDataStream>

class RawBlock
{
public:
    RawBlock();
    QByteArray toArray();

    quint64 fileSize;
    quint64 offsetFrom;
    quint64 offsetTo;
    QByteArray data;
};

#endif // RAWBLOCK_H
