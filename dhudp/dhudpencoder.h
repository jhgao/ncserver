#ifndef DHUDPENCODER_H
#define DHUDPENCODER_H

extern "C" {
    #include "clib/nc.h"
    #include "clib/randomlib.h"
}

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <iostream>
using std::ifstream;
using std::ofstream;
using std::ios;
using std::cin;

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
    ~DHudpEncoder();

    bool initClib(QString rawFn);

    QByteArray getEncodedBlock(quint64 i);
    bool setRawFile(QString absPath);
    bool setEncodedFile(QString);
    QByteArray getDecoderParameters() const;
    quint64 getRawFileBlockNum() const;
    quint32 getTotalCycleNum() const;
    quint8 blockNumInCycle(quint32) const;
signals:
    
public slots:
private:
    bool touch(QString aFilePath);

    // NC Variables
    FIELD* result_temp ;
    FILE* encoded_file;
    FILE* decoded_file;
    NCCB* i_cb ;
    quint8 i_d;

    QFileInfo i_rawFile;
    quint64 i_blockSize;
    quint64 i_blockNum;

    QFileInfo i_encodedFile;

    bool i_isInitOk; //wrap c code
};
}//namespace nProtocUDP

#endif // DHUDPENCODER_H
