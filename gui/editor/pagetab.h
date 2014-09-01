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

#include <QTabWidget>

class Page;

class PageTab : public QTabWidget
{
    Q_OBJECT
public:
    explicit PageTab(QWidget *parent = 0);

    Page* page(int index);
    QList<Page *> pages();
    int hasPage(const QString &pageName);

    void removeTab(int index);

protected:
    void focusInEvent(QFocusEvent *);

public slots:

};

#endif // PAGETABWIDGET_H
