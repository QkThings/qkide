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
#include "ui_optionsdialog.h"

#include "../qkide.h"

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
    foreach(Target::Board variant, target.boards)
        ui->build_toolchain_comboTargetVariant->addItem(variant.name);
}
