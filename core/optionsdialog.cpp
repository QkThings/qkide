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

    connect(ui->build_toolchain_comboTargetName, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateInterface()));
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::setup()
{
    setWindowFlags(Qt::Tool);
}

void OptionsDialog::setTargets(const QMap<QString, Target> &targets)
{
    m_targets = targets;
    ui->build_toolchain_comboTargetName->clear();
    foreach(QString targetName, targets.keys())
        ui->build_toolchain_comboTargetName->addItem(targetName);
}


bool OptionsDialog::validPath(const QString &path)
{
    if(path == "" || path == emptyRowMsg)
        return false;
    else
        return true;
}

void OptionsDialog::updateInterface()
{
    QString targetName = ui->build_toolchain_comboTargetName->currentText();
    Target target = m_targets[targetName];

    ui->build_toolchain_comboTargetVariant->clear();
    foreach(Target::Variant variant, target.variants)
        ui->build_toolchain_comboTargetVariant->addItem(variant.name);
}
