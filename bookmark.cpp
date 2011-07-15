#include "bookmark.h"

//Constructor
Bookmark::Bookmark(QString pTitle, QUrl pUrl, QObject *parent): QAction(pTitle,parent), bTitle(pTitle), bUrl(pUrl) {
    connect(this, SIGNAL(triggered()),SLOT(triggeredBase()));
}

//Data abstraction
QString Bookmark::title() {
    return bTitle;
}

QUrl Bookmark::url() {
    return bUrl;
}

int Bookmark::id() {
    return bID;
}

void Bookmark::setName(QString pTitle) {
    bTitle = pTitle;
}

void Bookmark::setUrl(QUrl pUrl) {
    bUrl = pUrl;
}

void Bookmark::setID(int pID) {
    bID = pID;
}

//When triggered from the old signal we emit the new "enriched" signal
void Bookmark::triggeredBase() {
    emit Bookmark::triggeredWithUrl(bUrl);
}
