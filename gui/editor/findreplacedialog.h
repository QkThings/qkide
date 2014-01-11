#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>

class Page;

namespace Ui {
class FindReplaceDialog;
}

class FindReplaceDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FindReplaceDialog(QWidget *parent = 0);
    ~FindReplaceDialog();

public slots:
    void setPage(Page *page);


signals:
    void find(QString,int);
    void replace(QString,int);
    void replaceAll(QString,int);
    

private slots:
    void slotFind();
    void slotReplace();
    void slotReplaceAll();
    void slotUpdateDirection();
private:
    Ui::FindReplaceDialog *ui;
    int findFlags();

    Page *m_page;
};

#endif // FINDREPLACEDIALOG_H
