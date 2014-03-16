#include "completer.h"

#include <QAbstractItemView>
#include <QDebug>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QFile>

Completer::Completer(QObject *parent) :
    QCompleter(parent)
{
    QAbstractItemView *p = this->popup();
    p->setFont(QFont("Monaco", 9));

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

void Completer::createItem(CodeParser::Element &element)
{
    QStandardItem *item = new QStandardItem(element.text);
    switch(element.type)
    {
    case CodeParser::Element::Define:
        item->setIcon(QIcon(":/img/icon_define.png"));
        break;
    case CodeParser::Element::Enum:
        item->setIcon(QIcon(":/img/icon_enum.png"));
        break;
    case CodeParser::Element::Typedef:
        item->setIcon(QIcon(":/img/icon_typedef.png"));
        break;
    case CodeParser::Element::Function:
        item->setIcon(QIcon(":/img/icon_function.png"));
        break;
    default:
        item->setIcon(QIcon(":/img/bullet_white.png"));
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


