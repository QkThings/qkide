#ifndef BROWSER_H
#define BROWSER_H

//#include <QtWebKit/QWebView>
#include <QtWebKitWidgets>

class Browser : public QWebView
{
    Q_OBJECT
public:
    explicit Browser(QWidget *parent = 0);
    
protected:
    void contextMenuEvent(QContextMenuEvent *e);

signals:
    void openRecentProject(int index);
    void openProject();
    void createProject();
    
public slots:


private slots:
    void slotHandleLink(const QUrl &url);
    
};

#endif // BROWSER_H
