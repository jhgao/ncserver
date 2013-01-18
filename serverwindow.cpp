#include "serverwindow.h"
#include "ui_serverwindow.h"

ServerWindow::ServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerWindow)
{
    ui->setupUi(this);

    ServerConfig::get_instance()->setRawFileName(RAW_FILE);

    qDebug() << QCoreApplication::applicationDirPath();

    ///

    if (!i_tcpWaitingServer.listen(QHostAddress::Any,TCP_CONTROL_PORT)) {
        qDebug() << "Waiting server listen failed";
        exit(-1);
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    qDebug() << "\n Waiting server is listening at" << ipAddress
             << ":" << i_tcpWaitingServer.serverPort();
    this->showListeningPort(ipAddress,i_tcpWaitingServer.serverPort());
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

void ServerWindow::showListeningPort(QString ip, quint16 port)
{
    QString s("listening at ");
    s.append(ip);
    s.append(" : ");
    s.append(QString::number(port));
    this->ui->label_ip_port->setText(s);
}
