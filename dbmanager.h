#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include "bookmark.h"

class DBManager : public QObject
{
    Q_OBJECT
public:
    explicit DBManager(QObject *parent = 0);
    QList<Bookmark*> list();
    int addBookmark(Bookmark *b);

signals:
    void error(QString err);

public slots:
    void delBookmark(QObject *b);

private:

};

#endif // DBMANAGER_H
