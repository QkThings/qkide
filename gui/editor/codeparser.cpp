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

#include "codeparser.h"
#include "qkide_global.h"

#include <QProcess>
#include <QDebug>
#include <QApplication>
#include <QFile>
#include <QTimer>
#include <QEventLoop>
#include <QRegularExpression>

CodeParser::CodeParser(QObject *parent) :
    QObject(parent)
{

}

void CodeParser::run()
{
    parse();
}

void CodeParser::parse(const QString &path)
{
    m_path = path;
    parse();
}

void CodeParser::parse()
{
    QString path = m_path;
    QString program = QApplication::applicationDirPath() + CTAGS_EXE;
    QStringList arguments;
    //QString output = "-f " + TAGS_DIR + "/tags";
    QString tagsDir = TAGS_DIR;
    QString output = "-f " + qApp->applicationDirPath() + TAGS_DIR + "/tags";
    arguments << output << "--languages=-Make" << "--c-kinds=+p-m" << "-R" << path;
    //arguments << "-R" << "--c-kinds=+l" << "--languages=-Make" << path << "--verbose";
    //arguments << "-R" << path;

    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    //process.setWorkingDirectory(path);    

    qDebug() << __FUNCTION__ << program << arguments;


    process.start(program, arguments);
    process.waitForFinished(30000);

    QFile tags(QApplication::applicationDirPath() + TAGS_DIR + "/tags");
    if(!tags.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "failed to open tags file:" << tags.fileName();
    }

    clear();
    while(!tags.atEnd())
    {
        QString line = tags.readLine();
        if(line[0] == '!' || line[0] == '_')
            continue;

        QRegularExpression re("(\\/\\^.*\\$\\/;\")|([^\\t]+)");
        QRegularExpressionMatchIterator it = re.globalMatch(line);
        QStringList fields;
        while (it.hasNext())
        {
            QRegularExpressionMatch match = it.next();
            if (match.hasMatch())
                 fields.append(match.captured());
        }

        Element el;
        el.text = fields[0].remove('"');
        el.fileName = fields.at(1);
        el.expression = fields[2].remove('"');
        el.local = false;
        el.type = Element::Unknown;

        QString typeStr = fields.at(3);
        typeStr.remove('\n');
        if(typeStr == "f" || typeStr == "p")
        {
            el.type = Element::Function;
            el.prototype = el.expression;
            el.prototype.remove('$');
            el.prototype.remove('^');
            el.prototype.remove('/');
            el.prototype.remove(';');
        }
        else if(typeStr == "d")
            el.type = Element::Define;
        else if(typeStr == "v")
            el.type = Element::Variable;
        else if(typeStr == "t")
            el.type = Element::Typedef;
        else if(typeStr == "e")
            el.type = Element::Enum;

        switch(el.type)
        {
        case Element::Function:
            if(hasElement(el.text, m_functions) == -1)
                m_functions.append(el);
            break;
        case Element::Define:
            if(!el.text.endsWith("_H") && hasElement(el.text, m_defines) == -1)
                m_defines.append(el);
            break;
        case Element::Enum:
            if(hasElement(el.text, m_enums) == -1)
                m_enums.append(el);
            break;
        case Element::Variable:
            if(hasElement(el.text, m_variables) == -1)
                m_variables.append(el);
            break;
        case Element::Typedef:
            if(hasElement(el.text, m_types) == -1)
                m_types.append(el);
            break;
        case Element::Unknown:
        default:
            ;//qDebug() << "unkown code element type" << typeStr << " el.text =" << el.text;
        }

    }

//    qDebug() << "Functions:";
//    foreach(const Element &el, m_functions)
//        qDebug() << el.text;
//    qDebug() << "Defines:";
//    foreach(const Element &el, m_defines)
//        qDebug() << el.text;
//    qDebug() << "Variables:";
//    foreach(const Element &el, m_variables)
//        qDebug() << el.text;

    emit parsed();
}

QList<CodeParser::Element> CodeParser::allElements()
{
    QList<Element> allElements;
    allElements.append(m_defines);
    allElements.append(m_enums);
    allElements.append(m_types);
    allElements.append(m_functions);
    allElements.append(m_variables);

    return allElements;
}

void CodeParser::clear()
{
    m_defines.clear();
    m_enums.clear();
    m_types.clear();
    m_functions.clear();
    m_variables.clear();
}

int CodeParser::hasElement(const QString &text, const QList<Element> &list)
{
    for(int i = 0; i < list.size(); i++)
    {
        if(list[i].text == text)
            return i;
    }
    return -1;
}

CodeParserThread::CodeParserThread(QObject *parent) :
    QThread(parent)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(200);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotParse()));
}

void CodeParserThread::startTimer()
{
    m_timer->start();
}

void CodeParserThread::slotParse()
{
    if(m_parser != 0)
        m_parser->parse(m_path);
}

void CodeParserThread::slotParsed()
{    
}

void CodeParserThread::run()
{
    CodeParser parser;
    m_parser = &parser;
    //connect(m_parser, SIGNAL(parsed()), this, SLOT(slotParsed()), Qt::DirectConnection);
    connect(m_parser, SIGNAL(parsed()), this, SIGNAL(parsed()));
    exec();
}
