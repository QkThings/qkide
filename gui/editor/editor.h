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

#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include <QRegExp>
#include "page.h"
#include "highlighter.h"

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
