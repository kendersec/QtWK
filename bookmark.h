#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QObject>
#include <QAction>
#include <QUrl>

//Inheriting from QAction to use the element in the menu
class Bookmark : public QAction
{
    Q_OBJECT
public:
    Bookmark(QString pTitle, QUrl pUrl, QObject *parent);

    //Data abstraction
    QString title();
    QUrl url();
    int id();

    void setName(QString pTitle);
    void setUrl(QUrl pUrl);
    void setID(int pID);

signals:
    void triggeredWithUrl(QUrl pUrl);

private slots:
    void triggeredBase();

private:
    QString bTitle;
    QUrl bUrl;
    int bID;
};

#endif // BOOKMARK_H
