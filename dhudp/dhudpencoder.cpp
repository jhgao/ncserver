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

}//namespace nProtocUDP
