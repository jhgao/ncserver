#ifndef DROPLINEEDIT_H
#define DROPLINEEDIT_H

#include <QLineEdit>
#include <QDragEnterEvent>
#include <QUrl>
#include <QFileInfo>


class DropLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit DropLineEdit(QWidget *parent = 0);
protected:
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
    
signals:
    void gotLocalFileUrlList(QList<QUrl> urlList);
    
public slots:
    
};

#endif // DROPLINEEDIT_H
