#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include <QWizard>

class QkIDE;

class ProjectWizard : public QWizard
{
    Q_OBJECT
public:
    explicit ProjectWizard(QkIDE *ide, QWidget *parent = 0);
    
    void accept();

    QString projectName;
    QString createIn;
    bool    saveDefaultPath;

signals:
    
public slots:

private slots:

private:
    QkIDE *m_ide;

};

#endif // PROJECTWIZARD_H
