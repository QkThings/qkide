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

#include "completer.h"
#include "qkide_global.h"

#include <QAbstractItemView>
#include <QDebug>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QFile>

Completer::Completer(QObject *parent) :
    QCompleter(parent)
{
    QAbstractItemView *p = this->popup();
    p->setFont(QFont(EDITOR_FONT_NAME, EDITOR_FONT_SIZE));

    m_model = new QStandardItemModel(this);
    setModel(m_model);
    updateModel();
}


void Completer::addElements(QList<CodeParser::Element> elements, bool permanent)
{
    foreach(CodeParser::Element el, elements)
        addElement(el, permanent);
    updateModel();
}

void Completer::addElement(const CodeParser::Element &element, bool permanent)
{
    if(permanent)
        m_permanentElements.append(element);
    else
        m_extraElements.append(element);
}

void Completer::clearElements(bool permanent)
{
    m_extraElements.clear();
    if(permanent)
        m_permanentElements.clear();

    updateModel();
}


QList<CodeParser::Element> Completer::allElements()
{
    QList<CodeParser::Element> list;
    list.append(m_permanentElements);
    list.append(m_extraElements);
    return list;
}

QList<CodeParser::Element> Completer::functions()
{
    QList<CodeParser::Element> list;
    foreach(CodeParser::Element el, m_permanentElements)
        if(el.type == CodeParser::Element::Function)
            list.append(el);
    foreach(CodeParser::Element el, m_extraElements)
        if(el.type == CodeParser::Element::Function)
            list.append(el);

    return list;
}

void Completer::createItem(CodeParser::Element &element)
{
    QStandardItem *item = new QStandardItem(element.text);
    switch(element.type)
    {
    case CodeParser::Element::Define:
        item->setIcon(QIcon(":/img/icon_define.png"));
        item->setData(QVariant('d'));
        break;
    case CodeParser::Element::Enum:
        item->setIcon(QIcon(":/img/icon_enum.png"));
        item->setData(QVariant('e'));
        break;
    case CodeParser::Element::Typedef:
        item->setIcon(QIcon(":/img/icon_typedef.png"));
        item->setData(QVariant('t'));
        break;
    case CodeParser::Element::Function:
        item->setIcon(QIcon(":/img/icon_function.png"));
        item->setData(QVariant('f'));
        break;
    default:
        item->setIcon(QIcon(":/img/bullet_white.png"));
        item->setData(QVariant('v'));
    }
    item->setToolTip(element.expression);
    m_model->appendRow(item);
}

void Completer::updateModel()
{
    m_model->clear();
    m_model->setColumnCount(1);

    foreach(CodeParser::Element el, m_extraElements)
        createItem(el);

    foreach(CodeParser::Element el, m_permanentElements)
        createItem(el);
}


