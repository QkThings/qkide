#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

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

public slots:

private slots:
    void slotRefreshPorts();
    
public:
    Ui::OptionsDialog *ui;

private:
    static const QString emptyRowMsg;

    void setup();
    bool validPath(const QString &path);
};

#endif // OPTIONSDIALOG_H
