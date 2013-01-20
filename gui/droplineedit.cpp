#include "droplineedit.h"
#include <QDebug>

DropLineEdit::DropLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    this->setAcceptDrops(true);
}

void DropLineEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void DropLineEdit::dropEvent(QDropEvent *event)
{
    QList<QUrl> urlList = event->mimeData()->urls();

    if( urlList.size() == 0 ) return;

    QList<QUrl> localUrlList;
    for(int i = 0; i< urlList.size(); ++i){
        if( !urlList[i].isLocalFile() ){
            qDebug() << "\t not a local file:" << urlList[i].toString();
            break;
        }else {
            localUrlList.append(urlList[i]);
        }
    }

    emit gotLocalFileUrlList(localUrlList);

    if( localUrlList.size() > 0){
        QFileInfo finfo(localUrlList[0].toLocalFile());
        this->setText(finfo.filePath());
    }
}
