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
#include "ui_codetip.h"

#include <QDebug>

CodeTip::CodeTip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CodeTip)
{
    ui->setupUi(this);

    setWindowFlags(Qt::ToolTip);

    QPalette p(palette());
    p.setColor(QPalette::ToolTipText, Qt::black);
    p.setColor(QPalette::Text, Qt::black);
    setPalette(p);
}

CodeTip::~CodeTip()
{
    delete ui;
}

void CodeTip::setFont(const QFont &font)
{
    ui->text->setFont(font);
}

void CodeTip::setText(const QString &text)
{
    ui->text->setText(text);
    QFontMetrics fm(ui->text->font());
    QString plainText = text;
    plainText.remove(QRegularExpression("(<.+?>)"));
    setFixedWidth(fm.width(plainText)+4);
}
