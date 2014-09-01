/*
 * QkThings LICENSE
 * The open source framework and modular platform for smart devices.
 * Copyright (C) 2014 <http://qkthings.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
