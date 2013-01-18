#include "serverconfig.h"
ServerConfig* ServerConfig::instance_p = 0;
ServerConfig *ServerConfig::get_instance(void)
{
    if(instance_p == 0)
   {
        instance_p = new ServerConfig();
    }
    return instance_p;
}

ServerConfig::ServerConfig(QObject *parent) :
    QObject(parent)
{
}

void ServerConfig::setRawFileName(QString fn)
{
    if(QFile::exists(fn))
        m_rawFn = fn;
}

const QString ServerConfig::getRawFileName() const
{
    return m_rawFn;
}
