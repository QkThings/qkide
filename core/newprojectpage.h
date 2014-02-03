#ifndef NEWPROJECTPAGE_H
#define NEWPROJECTPAGE_H

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

    QString m_defaultProjectPath;
};

#endif // NEWPROJECTPAGE_H
