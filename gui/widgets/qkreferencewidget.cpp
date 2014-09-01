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
#include "ui_qkreferencewidget.h"
#include "browser.h"

#include "qkide_global.h"

QkReferenceWidget::QkReferenceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QkReferenceWidget)
{
    ui->setupUi(this);

    QStringList refNames;
    refNames << "QkProgram" << "QkPeripheral" << "QkDSP";

    ui->comboRefName->addItems(refNames);
    slotReload();

    connect(ui->comboRefName, SIGNAL(currentIndexChanged(int)), this, SLOT(slotReload()));

    setWindowFlags(Qt::Tool);
    setWindowTitle("QkReference");
    resize(750,600);
}

QkReferenceWidget::~QkReferenceWidget()
{
    delete ui;
}

void QkReferenceWidget::slotReload()
{
    QString refName = ui->comboRefName->currentText().toLower();
    QString urlStr;

    urlStr += "file://" +  qApp->applicationDirPath();

    if(refName == "qkprogram")
        urlStr += QKPROGRAM_DOC_DIR;
    else if(refName == "qkperipheral")
        urlStr += QKPERIPHERAL_DOC_DIR;
    else if(refName == "qkdsp")
        urlStr += QKDSP_DOC_DIR;

    urlStr += "/html/index.html";

    qDebug() << "reference:" << urlStr;
    ui->browser->load(QUrl(urlStr));
}
