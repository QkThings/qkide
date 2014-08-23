#include "browser.h"

#include <QDebug>
//#include <QtWebKit/QWebPage>
#include <QFile>
#include <QWebPage>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>

Browser::Browser(QWidget *parent) :
    QWebView(parent)
{
    page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
    connect(this, SIGNAL(linkClicked(const QUrl &)), this, SLOT(slotHandleLink(const QUrl &)));

    show();
}

void Browser::slotHandleLink(const QUrl &url)
{
    qDebug() << "url.host()" << url.host();
    qDebug() << "url.path()" << url.toString();

    QString urlStr = url.toString();
    QStringList fields = urlStr.split(":");

    int i;


    if(fields.at(0) == "prj")
    {
        if(fields.at(1) == "create")
            emit createProject();
        else if(fields.at(1) == "open")
            emit openProject();
        else if(fields.at(1) == "recent")
        {
            i = fields.at(2).toInt();
            emit openRecentProject(i);
        }
    }
    else if(fields.at(0) == "link")
    {
        QDesktopServices::openUrl(QUrl("http://" + fields.at(1)));
    }
}

void Browser::contextMenuEvent(QContextMenuEvent *e)
{
    e->accept();
}
