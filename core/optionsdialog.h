#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include "qkutils.h"

using namespace QkUtils;

class QkIDE;

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();

    void setTargets(const QMap<QString,Target> &targets);

public slots:

private slots:
    void updateInterface();
    
public:
    Ui::OptionsDialog *ui;

private:
    static const QString emptyRowMsg;

    QMap<QString,Target> m_targets;

    void setup();
    bool validPath(const QString &path);
};

#endif // OPTIONSDIALOG_H
