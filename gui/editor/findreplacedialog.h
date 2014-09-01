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
