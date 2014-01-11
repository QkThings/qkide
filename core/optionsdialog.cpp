#include "optionsdialog.h"
#include "ui_optionsdialog.h"

#include "../qkide.h"
//#include <qextserialenumerator.h>

#include <QDebug>
#include <QFileDialog>
#include <QFileDialog>
#include <QtSerialPort/QSerialPortInfo>

const QString OptionsDialog::emptyRowMsg = tr("(Enter or 'browse' string here)");

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    setup();

    slotRefreshPorts();
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::slotRefreshPorts()
{
    QStringList list;
    foreach(QSerialPortInfo info, QSerialPortInfo::availablePorts())
    {
        list.append(info.portName());
    }
    ui->comboPortName->clear();
    ui->comboPortName->addItems(list);
}

void OptionsDialog::setup()
{
    setWindowFlags(Qt::Dialog);
    connect(ui->refresh_toolButton, SIGNAL(clicked()), this, SLOT(slotRefreshPorts()));
}


bool OptionsDialog::validPath(const QString &path)
{
    if(path == "" || path == emptyRowMsg)
        return false;
    else
        return true;
}
