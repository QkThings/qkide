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

#include <QWizardPage>

class QLabel;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QComboBox;

class NewProjectPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit NewProjectPage(const QString &path = QString(), QWidget *parent = 0);

    void initializePage();

signals:
    

public slots:

private slots:
    void slotBrowse();
    void slotValidatePath();

private:
    QLabel *projectNameLabel;
    QLineEdit *projectNameEdit;

    QLabel *createInLabel;
    QLineEdit *createInEdit;
    QPushButton *browseButton;

    QCheckBox *defaultPathCheck;
    QLabel *warningLabel;
    
    QLabel *targetLabel;
    QComboBox *targetCombo;

    QComboBox *comboTargetName;
    QComboBox *comboTargetVariant;

    QString m_defaultProjectPath;
};

#endif // NEWPROJECTPAGE_H
