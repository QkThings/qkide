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
}

FindReplaceDialog::~FindReplaceDialog()
{
    delete ui;
}

void FindReplaceDialog::setPage(Page *page)
{
    if(page == 0) return;
    if(m_page != 0) {
        disconnect(this, SIGNAL(find(QString,int)), m_page, SLOT(slotFind(QString,int)));
        disconnect(this, SIGNAL(replace(QString,int)), m_page, SLOT(slotReplace(QString,int)));
        disconnect(this, SIGNAL(replaceAll(QString,int)), m_page, SLOT(slotReplaceAll(QString,int)));
    }
    m_page = page;
    connect(this, SIGNAL(find(QString,int)), m_page, SLOT(slotFind(QString,int)));
    connect(this, SIGNAL(replace(QString,int)), m_page, SLOT(slotReplace(QString,int)));
    connect(this, SIGNAL(replaceAll(QString,int)), m_page, SLOT(slotReplaceAll(QString,int)));
}

void FindReplaceDialog::slotFind()
{
    emit find(ui->find_lineEdit->text(), findFlags());
}

void FindReplaceDialog::slotReplace()
{

}

void FindReplaceDialog::slotReplaceAll()
{

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
