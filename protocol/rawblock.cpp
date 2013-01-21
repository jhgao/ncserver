#include "rawblock.h"

RawBlock::RawBlock()
{
}

QByteArray RawBlock::toArray()
{
    QByteArray a;
    QDataStream out(&a, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << fileSize;
    out << offsetFrom;
    out << offsetTo;
    out << data;

    return a;
}
