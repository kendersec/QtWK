#include <QtGui>
#include "dbmanager.h"

class QWebView;
QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QUrl& url);

protected slots:

    void adjustLocation();
    void changeLocation();
    void adjustTitle();
    void setProgress(int p);
    void finishLoading(bool);
    void loadBookmark(QUrl pUrl);

    void viewSource();
    void slotSourceDownloaded();

    void highlightAllLinks();
    void rotateImages(bool invert);

    void addBookmark();
    void editBookmarks(bool);

    void dbError(QString err);
private:
    DBManager *dbm;

    QString jQuery;
    QWebView *view;
    QLineEdit *locationEdit;
    QAction *rotateAction;
    int progress;
    QMenu *bookmarkMenu;
    QAction *editBookmarksAction;
    QList<Bookmark*> bookmarks;

    void populateBookmarks();
};
