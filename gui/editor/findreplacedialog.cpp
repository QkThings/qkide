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

#include "findreplacedialog.h"
#include "ui_findreplacedialog.h"

#include <QTextDocument>
#include <QPlainTextEdit>
#include "page.h"

FindReplaceDialog::FindReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindReplaceDialog)
{
    ui->setupUi(this);
    m_page = 0;
    setWindowFlags(Qt::Tool);
    setWindowTitle(tr("Find and Replace"));

    connect(ui->find_pushButton, SIGNAL(clicked()), this, SLOT(slotFind()));
    connect(ui->replace_pushButton, SIGNAL(clicked()), this, SLOT(slotReplace()));
    connect(ui->replaceAll_pushButton, SIGNAL(clicked()), this, SLOT(slotReplaceAll()));

    ui->find_lineEdit->setFocus();
}

FindReplaceDialog::~FindReplaceDialog()
{
    delete ui;
}

void FindReplaceDialog::show()
{
    ui->find_lineEdit->setFocus();
    QDialog::show();
}

void FindReplaceDialog::setPage(Page *page)
{
    if(page == 0) return;
    if(m_page != 0) {
        disconnect(this, SIGNAL(find(QString,int)), m_page, SLOT(slotFind(QString,int)));
        disconnect(this, SIGNAL(replace(QString,QString,int,bool)), m_page, SLOT(slotReplace(QString,QString,int,bool)));
    }
    m_page = page;
    connect(this, SIGNAL(find(QString,int)), m_page, SLOT(slotFind(QString,int)));
    connect(this, SIGNAL(replace(QString,QString,int,bool)), m_page, SLOT(slotReplace(QString,QString,int,bool)));
}

void FindReplaceDialog::slotFind()
{
    emit find(ui->find_lineEdit->text(), findFlags());
}

void FindReplaceDialog::slotReplace()
{
    emit replace(ui->find_lineEdit->text(),
                 ui->replace_lineEdit->text(),
                 findFlags(),
                 false);
}

void FindReplaceDialog::slotReplaceAll()
{
    emit replace(ui->find_lineEdit->text(),
                 ui->replace_lineEdit->text(),
                 findFlags(),
                 true);
}

void FindReplaceDialog::slotUpdateDirection()
{

}

int FindReplaceDialog::findFlags()
{
    int flags = 0;
    if(ui->case_checkBox->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if(ui->up_radioButton->isChecked())
        flags |= QTextDocument::FindBackward;
    if(ui->whole_checkBox->isChecked())
        flags |= QTextDocument::FindWholeWords;
    return flags;
}
