#include <QTabWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QFile>
#include "editor.h"
#include "pagetab.h"
#include "page.h"
#include "findreplacedialog.h"

#include <QDebug>


Editor::Editor(QWidget *parent) :
    QWidget(parent)
{
    m_activePage = 0;

    mainTabs = new PageTab();
    mainTabs->setMovable(true);
    mainTabs->setTabsClosable(false);
    mainTabs->setDocumentMode(true);

    splittedTabs = new PageTab();
    splittedTabs->setMovable(true);
    splittedTabs->setTabsClosable(false);
    splittedTabs->setDocumentMode(true);
    splittedTabs->hide();

    layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);

    layout->addWidget(mainTabs);
    m_splitted = false;

    horizontalSplitter = new QSplitter(Qt::Horizontal);
    verticalSplitter = new QSplitter(Qt::Vertical);

    horizontalSplitter->setChildrenCollapsible(false);
    verticalSplitter->setChildrenCollapsible(false);

    horizontalSplitter->hide();
    verticalSplitter->hide();

    connect(mainTabs, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequestHandler(int)));
    connect(splittedTabs, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequestHandler(int)));

    m_findReplaceDialog = new FindReplaceDialog(this);
}

Editor::~Editor()
{
    delete m_findReplaceDialog;
    delete verticalSplitter;
    delete horizontalSplitter;
    delete splittedTabs;
    delete mainTabs;
}


void Editor::updateActivePage()
{
    Page *senderPage = qobject_cast<Page *>(sender());
    qDebug() << "updateActivePage() sender =" << senderPage->name();
    m_activePage = senderPage;
    m_findReplaceDialog->setPage(m_activePage);
}

void Editor::addTab(const QString &tabName, QWidget *widget)
{
    mainTabs->addTab(widget, tabName);
}

Page* Editor::addPage(const QString &tabName, const QString &pageName)
{
    Page *page;

    page = new Page(pageName, mainTabs);
    /*if(pageName.contains(".c"))
        mainTabs->addTab(page, QIcon(":/img/cfile.png"), tabName);
    else
        mainTabs->addTab(page, QIcon(":/img/header.png"), tabName);*/
    mainTabs->addTab(page, tabName);
    mainTabs->setCurrentIndex(mainTabs->count() - 1);

    connect(page, SIGNAL(textChanged()), this, SLOT(updatePageText()));
    connect(page, SIGNAL(focused()), this, SLOT(updateActivePage()));

    if(m_splitted)
    {
        page = new Page(pageName, splittedTabs);
        splittedTabs->addTab(page, tabName);
        splittedTabs->setCurrentIndex(splittedTabs->count() - 1);

        connect(page, SIGNAL(textChanged()), this, SLOT(updatePageText()));
        //connect(page, SIGNAL(focused()), this, SLOT(updateActivePage()));
    }

    return page;
}

Page* Editor::addPage(const QString &pageName)
{
    return addPage(pageName, pageName);
}

void Editor::removePage(const QString &pageName)
{
    int tabIndex;

    if(mainTabs->count() == 0) return;

    tabIndex = mainTabs->hasPage(pageName);
    if(tabIndex != -1)
        mainTabs->removeTab(tabIndex);

    if(m_splitted)
    {
        tabIndex = splittedTabs->hasPage(pageName);
        if(tabIndex != -1)
            splittedTabs->removeTab(tabIndex);
    }
}

void Editor::setCurrentPage(int index)
{
    mainTabs->setCurrentIndex(index);
}

void Editor::closeAllPages()
{
    Page *page;

    for(int i=0; i < mainTabs->count(); i++)
    {
        page = qobject_cast<Page *>(mainTabs->widget(i));
        page->close();
    }

    mainTabs->clear();
}

int Editor::hasPage(const QString &pageName)
{
    for(int i=0; i < mainTabs->count(); i++)
    {
        if(page(i)->name().compare(pageName) == 0)
            return i;
    }

    return -1;
}

bool Editor::hasModifiedPages()
{
    QList<Page*> list = pages();
    bool modified = false;
    if(list.count() > 0) {
        for(int i=0; i < list.count(); i++) {
            if(list.at(i)->document()->isModified()) {
                modified = true;
                break;
            }
        }
    }
    return modified;
}

int Editor::countPages()
{
    return mainTabs->count();
}

QList<Page *> Editor::pages()
{
    QList<Page *> list;
    int i, count = countPages();
    for(i=0; i<count; i++) {
        list.append(page(i));
    }
    return list;
}

Page* Editor::page(int index)
{
    return qobject_cast<Page *>(mainTabs->widget(index));
}

Page* Editor::currentPage()
{
    return qobject_cast<Page *>(mainTabs->currentWidget());
}


void Editor::savePage(int index, const QString &filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Can't open" << filePath;
        return;
    }
    qDebug() << "save page/file path" << filePath;
    file.write(page(index)->text().toLatin1());
    file.close();

    page(index)->document()->setModified(false);

    qDebug() << page(index)->name() << "saved";
}

void Editor::savePage(const QString &pageName, const QString &filePath)
{
    int index;
    if((index = hasPage(pageName)) == -1) return;

    savePage(index, filePath);
}

void Editor::splitHorizontal()
{
    split(horizontalSplitter);
}

void Editor::splitVertical()
{
    split(verticalSplitter);
}

void Editor::split(QSplitter *splitter)
{
    if(!m_splitted)
    {
        createSplittedTabs();
        layout->removeWidget(mainTabs);
    }
    else
    {
        horizontalSplitter->close();
        verticalSplitter->close();
        layout->removeWidget(horizontalSplitter);
        layout->removeWidget(verticalSplitter);
    }

    splitter->addWidget(mainTabs);
    splitter->addWidget(splittedTabs);

    QList<int> sizes;
    sizes << 10 << 10;
    splitter->setSizes(sizes);

    layout->addWidget(splitter);

    splitter->show();
    splittedTabs->show();

    m_splitted = true;
}

void Editor::removeSplit()
{
    splittedTabs->hide();
    clearSplittedTabs();

    horizontalSplitter->hide();
    verticalSplitter->hide();

    layout->removeWidget(horizontalSplitter);
    layout->removeWidget(verticalSplitter);

    layout->addWidget(mainTabs);

    m_splitted = false;
}

void Editor::createSplittedTabs()
{
    Page *page, *splittedPage;

    for(int i = 0; i < mainTabs->count(); i++)
    {
        page = qobject_cast<Page *>(mainTabs->widget(i));
        splittedPage = new Page(page->name(), this);
        splittedPage->setPlainText(page->text());
        splittedTabs->addTab(splittedPage, splittedPage->name());

        connect(splittedPage, SIGNAL(textChanged()), this, SLOT(updatePageText()));
        //connect(splittedPage, SIGNAL(focused()), this, SLOT(updateActivePage()));
    }
}

void Editor::updateSplittedTabs()
{
    clearSplittedTabs();
    createSplittedTabs();
}

void Editor::clearSplittedTabs()
{
    Page *page;

    for(int i = 0; i < splittedTabs->count(); i++)
    {
        page = qobject_cast<Page *>(splittedTabs->widget(i));
        page->close();
        delete page;
    }

    splittedTabs->clear();
}

void Editor::updatePageText()
{
    int index, firstLine;
    Page *senderPage = qobject_cast<Page *>(sender());

    if(senderPage != m_activePage) return;

    if(mainTabs->indexOf(senderPage) != -1 && m_splitted)
    {
        index = splittedTabs->hasPage(senderPage->name());
        if(index != -1)
        {
            splittedTabs->page(index)->setPlainText(senderPage->text());
            //firstLine = splittedTabs->page(index)->firstVisibleLine();
            //splittedTabs->page(index)->setFirstVisibleLine(firstLine);
        }
    } else if(splittedTabs->indexOf(senderPage) != -1)
    {
        index = mainTabs->hasPage(senderPage->name());
        if(index != -1)
        {
            mainTabs->page(index)->setPlainText(senderPage->text());
            //firstLine = mainTabs->page(index)->firstVisibleLine();
            //mainTabs->page(index)->setFirstVisibleLine(firstLine);
        }

    }
}

void Editor::tabCloseRequestHandler(int index)
{
    PageTab *senderPageTab = qobject_cast<PageTab *>(sender());
    qDebug() << "tabCloseRequestHandler()" << index << senderPageTab->page(index)->name();
    emit tabCloseRequested(senderPageTab->page(index)->name());
}

void Editor::showSearch()
{
    m_findReplaceDialog->show();
    m_findReplaceDialog->activateWindow();
}


