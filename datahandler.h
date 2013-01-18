#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>

class DataHandler : public QObject
{
    Q_OBJECT
public:
    explicit DataHandler(quint16 port,
                         QObject *parent = 0);
    
signals:
    
public slots:
};

#endif // DATAHANDLER_H
