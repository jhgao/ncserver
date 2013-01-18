#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include <QObject>
#include <QFile>

#define RAW_FILE "Test.wmv" // test input file

class ServerConfig : public QObject
{
    Q_OBJECT
public:
    static ServerConfig* get_instance(void);
    void setRawFileName(QString);
    const QString getRawFileName()const;
    
signals:
    
public slots:
private:
    static ServerConfig* instance_p;
    explicit ServerConfig(QObject *parent = 0);
    QString m_rawFn;
    
};

#endif // SERVERCONFIG_H
