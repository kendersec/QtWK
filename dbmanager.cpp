#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include "dbmanager.h"

DBManager::DBManager(QObject *parent) :
    QObject(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":bookmarks.dat");
    if (!db.open()) {
        emit error(db.lastError().text());
    }

    if(!QFile::exists(":bookmarks.dat"))
    {
        QSqlQuery query;
        query.exec("CREATE TABLE Bookmarks (id INTEGER PRIMARY KEY, title VARCHAR(40) NOT NULL, url VARCHAR(100) NOT NULL)");
    }
}

QList<Bookmark*> DBManager::list() {
    QList<Bookmark*> list;
    QSqlQuery query;
    query.exec("SELECT * FROM Bookmarks");
    while(query.next()) {
        QUrl url = QUrl(query.value(2).toString());
        Bookmark *b = new Bookmark(query.value(1).toString(), url,0);
        b->setID(query.value(0).toInt());

        list.append(b);
    }

    return list;
}

int DBManager::addBookmark(Bookmark *b) {
    QSqlQuery query;
    query.prepare("INSERT INTO Bookmarks (title, url) VALUES (?,?)");
    query.addBindValue(b->title());
    query.addBindValue(b->url());
    query.exec();

    QSqlQuery query2;
    query2.exec("SELECT MAX(id) FROM Bookmarks");
    query2.next();
    return query2.value(0).toInt();
}

void DBManager::delBookmark(QObject *b) {
    QSqlQuery query;
    query.prepare("DELETE FROM Bookmarks WHERE id=?");

    // Watch out! Casting here to get back the Bookmark object
    query.addBindValue(((Bookmark *)b)->id());
    query.exec();
}
