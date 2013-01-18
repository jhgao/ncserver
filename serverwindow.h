#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include "waitingserver.h"
#include "serverconfig.h"
#include "protocol/ports_define.h"

namespace Ui {
class ServerWindow;
}

class ServerWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ServerWindow(QWidget *parent = 0);
    ~ServerWindow();
    
private:
    void showListeningPort(QString ip, quint16 port);

    Ui::ServerWindow *ui;
    WaitingServer i_tcpWaitingServer;
};

#endif // SERVERWINDOW_H
