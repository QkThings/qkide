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

    void show();

public slots:
    void setPage(Page *page);


signals:
    void find(QString text,int flags);
    void replace(QString prev,QString next,int flags, bool all);
    

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
