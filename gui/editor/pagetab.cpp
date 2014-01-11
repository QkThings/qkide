#include "pagetab.h"
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
