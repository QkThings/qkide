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
