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
#include "page.h"

#include <QMouseEvent>

PageTab::PageTab(QWidget *parent) :
    QTabWidget(parent)
{
    setAutoFillBackground(true);
}

Page* PageTab::page(int index)
{
    return qobject_cast<Page *>(widget(index));
}

void PageTab::focusInEvent(QFocusEvent *)
{

}

void PageTab::removeTab(int index)
{
    page(index)->close();
    QTabWidget::removeTab(index);
}

QList<Page *> PageTab::pages()
{
    QList<Page *> list;
    for(int i = 0; i < count(); i++)
    {
        list.append(page(i));
    }

    return list;
}

int PageTab::hasPage(const QString &pageName)
{
    int index = -1;
    for(index = 0; index < count(); index++)
    {
        Page *page = qobject_cast<Page *>(widget(index));
        if(page->name().compare(pageName) == 0)
            return index;
    }

    return index;
}
