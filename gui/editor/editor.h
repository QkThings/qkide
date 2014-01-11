#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include "page.h"

class QSplitter;
class QTabWidget;
class QLayout;
class PageTab;
class FindReplaceDialog;

class Editor : public QWidget
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = 0);
    ~Editor();

    void setCurrentPage(int index);
    void closeAllPages();
    int hasPage(const QString &pageName);
    bool hasModifiedPages();
    int countPages();
    QList<Page*> pages();
    Page* page(int index);
    Page* currentPage();

signals:
    void tabCloseRequested(QString);

public slots:
    void addTab(const QString &tabName, QWidget *widget);
    Page* addPage(const QString &tabName, const QString &pageName);
    Page* addPage(const QString &pageName);
    void removePage(const QString &pageName);
    void savePage(int index, const QString &filePath);
    void savePage(const QString &pageName, const QString &filePath);
    void splitHorizontal();
    void splitVertical();
    void removeSplit();
    void showSearch();

private slots:
    void updateActivePage();
    void updatePageText();
    void tabCloseRequestHandler(int index);

private:
    PageTab *mainTabs;
    PageTab *splittedTabs;

    QSplitter *horizontalSplitter;
    QSplitter *verticalSplitter;

    QLayout *layout;

    Page *m_activePage;

    bool m_splitted;

    void split(QSplitter *splitter);
    void createSplittedTabs();
    void updateSplittedTabs();
    void clearSplittedTabs();

signals:
    void updatePageText(QString, QString);

private:
    FindReplaceDialog *m_findReplaceDialog;

};

#endif // EDITOR_H
