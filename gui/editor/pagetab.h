#ifndef PAGETABWIDGET_H
#define PAGETABWIDGET_H

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
