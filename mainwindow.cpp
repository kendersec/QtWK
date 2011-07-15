#include <QtGui>
#include <QtWebKit>
#include "mainwindow.h"
#include "bookmark.h"

MainWindow::MainWindow(const QUrl& url)
{
    progress = 0;

    //DB for bookmarks
    dbm = new DBManager();
    connect(dbm, SIGNAL(error(QString)),SLOT(dbError(QString)));

    // JQuery loading for easing some operations
    // Multiplatform unified API file handling example
    QFile file;
    file.setFileName(":/jquery.min.js");
    file.open(QIODevice::ReadOnly);
    jQuery = file.readAll();
    file.close();

    view = new QWebView(this);
    view->load(url);
    connect(view, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));
    connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

    locationEdit = new QLineEdit(this);
    // Rezing policy of the widget, take all the room horizontally but remain the same vertically
    locationEdit->setSizePolicy(QSizePolicy::Expanding, locationEdit->sizePolicy().verticalPolicy());
    connect(locationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));

    QToolBar *toolBar = addToolBar(tr("Navigation"));
    toolBar->addAction(view->pageAction(QWebPage::Back));
    toolBar->addAction(view->pageAction(QWebPage::Forward));
    toolBar->addAction(view->pageAction(QWebPage::Reload));
    toolBar->addAction(view->pageAction(QWebPage::Stop));
    toolBar->addWidget(locationEdit);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    QAction *viewSourceAction = new QAction("Page Source", this);
    connect(viewSourceAction, SIGNAL(triggered()), SLOT(viewSource()));
    viewMenu->addAction(viewSourceAction);

    bookmarkMenu = menuBar()->addMenu(tr("&Bookmarks"));
    QAction *addBookmarkAction = new QAction("Add Bookmark", this);
    editBookmarksAction = new QAction("Edit Bookmarks", this);
    editBookmarksAction->setCheckable(true);
    connect(addBookmarkAction, SIGNAL(triggered()), SLOT(addBookmark()));
    connect(editBookmarksAction, SIGNAL(triggered(bool)), SLOT(editBookmarks(bool)));
    bookmarkMenu->addAction(addBookmarkAction);
    bookmarkMenu->addAction(editBookmarksAction);
    bookmarkMenu->addSeparator();
    MainWindow::populateBookmarks();

    QMenu *effectMenu = menuBar()->addMenu(tr("&Effect"));
    effectMenu->addAction("Highlight all links", this, SLOT(highlightAllLinks()));

    rotateAction = new QAction(this);
    rotateAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    rotateAction->setCheckable(true);
    rotateAction->setText(tr("Turn images upside down"));
    connect(rotateAction, SIGNAL(toggled(bool)), this, SLOT(rotateImages(bool)));
    effectMenu->addAction(rotateAction);

    setCentralWidget(view);
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::viewSource()
{
    QNetworkAccessManager* accessManager = view->page()->networkAccessManager();
    QNetworkRequest request(view->url());
    QNetworkReply* reply = accessManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(slotSourceDownloaded()));
}

void MainWindow::slotSourceDownloaded()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(const_cast<QObject*>(sender()));
    QTextEdit* textEdit = new QTextEdit(NULL);
    textEdit->setAttribute(Qt::WA_DeleteOnClose);
    textEdit->show();
    textEdit->setPlainText(reply->readAll());
    reply->deleteLater();
}

void MainWindow::adjustLocation()
{
    locationEdit->setText(view->url().toString());
}

void MainWindow::changeLocation()
{
    QUrl url = QUrl(locationEdit->text());
    view->load(url);
    view->setFocus();
}

void MainWindow::loadBookmark(QUrl pUrl)
{
    view->load(pUrl);
    view->setFocus();
}

void MainWindow::adjustTitle()
{
    if (progress <= 0 || progress >= 100)
        setWindowTitle(view->title());
    else
        setWindowTitle(QString("%1 (%2%)").arg(view->title()).arg(progress));
}

void MainWindow::setProgress(int p)
{
    progress = p;
    adjustTitle();
}

void MainWindow::finishLoading(bool)
{
    progress = 100;
    adjustTitle();
    view->page()->mainFrame()->evaluateJavaScript(jQuery);

    rotateImages(rotateAction->isChecked());
}

void MainWindow::highlightAllLinks()
{
    QString code = "$('a').each( function () { $(this).css('background-color', 'yellow') } )";
    view->page()->mainFrame()->evaluateJavaScript(code);
}

void MainWindow::rotateImages(bool invert)
{
    QString code;
    if (invert)
        code = "$('img').each( function () { $(this).css('-webkit-transition', '-webkit-transform 2s'); $(this).css('-webkit-transform', 'rotate(180deg)') } )";
    else
        code = "$('img').each( function () { $(this).css('-webkit-transition', '-webkit-transform 2s'); $(this).css('-webkit-transform', 'rotate(0deg)') } )";
    view->page()->mainFrame()->evaluateJavaScript(code);
}

void MainWindow::addBookmark()
{
    Bookmark *bookmark = new Bookmark(view->title(),view->url(),bookmarkMenu);
    bookmarkMenu->addAction(bookmark);
    bookmarks.append(bookmark);
    connect(bookmark, SIGNAL(triggeredWithUrl(QUrl)),SLOT(loadBookmark(QUrl)));
    bookmark->setID(dbm->addBookmark(bookmark));
}

void MainWindow::populateBookmarks() {
    bookmarks = dbm->list();
    for(int i=0; i<bookmarks.count(); i++) {
        bookmarks[i]->setParent(bookmarkMenu);
        bookmarkMenu->addAction(bookmarks[i]);
        connect(bookmarks[i],SIGNAL(triggeredWithUrl(QUrl)),SLOT(loadBookmark(QUrl)));
    }
}

void MainWindow::editBookmarks(bool edit) {
    if(edit) {
        editBookmarksAction->setText("Exit edit");
        for(int i=0; i<bookmarks.count(); i++)
        {
            QMenu *menu = new QMenu(bookmarkMenu);
            QAction *del = new QAction("Delete",this);
            connect(del,SIGNAL(triggered()),bookmarks[i],SLOT(deleteLater()));
            connect(bookmarks[i],SIGNAL(destroyed(QObject*)),dbm, SLOT(delBookmark(QObject*)));
            menu->addAction(del);
            bookmarks[i]->setMenu(menu);
        }
    }
    else {
        editBookmarksAction->setText("Edit Bookmarks");
        for(int i=0; i<bookmarks.count(); i++)
        {
            bookmarks[i]->menu()->deleteLater();
            bookmarks[i]->setMenu(NULL);
        }
    }
}

void MainWindow::dbError(QString err)
{
    QMessageBox::warning(this, tr("Database Error"), err);
}
