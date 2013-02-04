#include "serverwindow.h"
#include "ui_serverwindow.h"

ServerWindow::ServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerWindow)
{
    ui->setupUi(this);

    connect(ui->lineEdit_rawFile, SIGNAL(gotLocalFileUrlList(QList<QUrl>)),
            this, SLOT(setEncoderRawFiles(QList<QUrl>)));

    ui->lineEdit_rawFile->setText(RAW_FILE);
    ServerConfig::get_instance()->setRawFileName(RAW_FILE);

    connect(&i_tcpWaitingServer, SIGNAL(sig_connectionLogForGui(QString)),
            ui->textEdit_log, SLOT(append(QString)));
    connect(&i_tcpWaitingServer, SIGNAL(sig_logForGui(QString)),
            ui->textEdit_log, SLOT(append(QString)));
    connect(&i_tcpWaitingServer, SIGNAL(sig_statusString(QString)),
            ui->statusBar, SLOT(showMessage(QString)));

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

void ServerWindow::setEncoderRawFiles(QList<QUrl> urlList)
{
    if( urlList.size() > 0){
       ServerConfig::get_instance()->setRawFileName(urlList[0].toLocalFile());
    }
}

void ServerWindow::showListeningPort(QString ip, quint16 port)
{
    QString s("listening at ");
    s.append(ip);
    s.append(" : ");
    s.append(QString::number(port));
    this->ui->label_ip_port->setText(s);
}
