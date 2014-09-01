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

#include <QDebug>
#include <QMessageBox>
#include <QDialog>
#include <QFile>
#include <QApplication>
#include "codeparser.h"
#include "qkide_global.h"

Highlighter::Highlighter(QTextDocument *document) : QSyntaxHighlighter(document)
{
    multiLineCommentFormat.setForeground(QColor("#999999"));

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");

    QString defaultSyntaxPath = qApp->applicationDirPath() + THEME_DIR +
                                "/syntax/white.syntax";

    setSyntax(defaultSyntaxPath);
}

bool Highlighter::setSyntax(const QString &filePath)
{
    QString keywordColor;
    QString keyword;
    QTextCharFormat keywordFormat;

    QFile file(filePath);
    qDebug() << __FUNCTION__ << file.fileName();

    bool stop = false;
    QString elementName, color,weight;
    Rule rule;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(new QDialog,
                              tr("Error"),
                              tr("Error load syntax higlighter"),
                              QMessageBox::Ok);
    }

    QTextStream input(&file);

    m_permanentRules.clear();

    while (!input.atEnd())
    {
        keywordColor = input.readLine();

        if (keywordColor.contains("#rule"))
        {
            elementName = keywordColor.section(',',1,1);
            color = keywordColor.section(',',2,2);
            weight = keywordColor.section(',',3,3);
            keywordFormat.setForeground(QBrush(QColor(color)));
            if(weight.toLower() == "bold")
                keywordFormat.setFontWeight(QFont::Bold);
            else
                keywordFormat.setFontWeight(QFont::Normal);
        }

        rule.elementName = elementName;
        rule.format  = keywordFormat;
        rule.pattern = QRegExp();

        while (stop == false)
        {
            keyword = input.readLine();

            if (keyword == "stop")
                stop = true;
            else
            {
                rule.elementName = elementName;
                rule.format  = keywordFormat;
                rule.pattern = QRegExp(keyword);
                m_permanentRules.append(rule);
            }
        }

        if(rule.pattern.isEmpty())
            m_permanentRules.append(rule);

        stop = false;
    }
}

void Highlighter::addElements(QList<CodeParser::Element> elements, bool permanent)
{
    foreach(CodeParser::Element el, elements)
    {
        if(el.type != CodeParser::Element::Function) // skip functions
            addElement(el, permanent);
    }
}

void Highlighter::addElement(const CodeParser::Element &element, bool permanent)
{
    Highlighter::Rule rule;
    QTextCharFormat textFormat;
    QColor textColor;

    switch(element.type)
    {
    case CodeParser::Element::Define:
        rule.elementName = "define";
        break;
    case CodeParser::Element::Enum:
        rule.elementName = "enum";
        break;
    case CodeParser::Element::Function:
        rule.elementName = "function";
        break;
    case CodeParser::Element::Typedef:
        rule.elementName = "typedef";
        break;
    case Custom:
    default:
        rule.elementName = "custom";
    }

    switch(element.type)
    {
    case CodeParser::Element::Define:
    case CodeParser::Element::Enum:
    case CodeParser::Element::Function:
    case CodeParser::Element::Typedef:
        textColor = elementColor(rule.elementName);
        break;
    default:
        textColor = QColor("#111");
    }

    textFormat.setForeground(QBrush(textColor));
    rule.format = textFormat;
    QString regExpStr = "\\b(" + element.text + ")\\b";
    rule.pattern = QRegExp(regExpStr);

    if(permanent)
        m_permanentRules.append(rule);
    else
        m_extraRules.append(rule);
}

void Highlighter::clearElements(bool permanent)
{
    m_extraRules.clear();
    if(permanent)
        m_permanentRules.clear();
}

QColor Highlighter::elementColor(const QString &elementName)
{
    foreach(const Rule &rule, m_permanentRules)
        if(rule.elementName == elementName)
            return rule.format.foreground().color();
    return QColor();
}

void Highlighter::applyRuleToText(const Rule &rule, const QString &text)
{
    if(rule.pattern.isEmpty())
        return;
    QRegExp expression(rule.pattern);
    int index = expression.indexIn(text);

    while (index >= 0)
    {
        int length = expression.matchedLength();
        setFormat(index, length, rule.format);
        index = expression.indexIn(text, index + length);
    }
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const Rule &rule, m_extraRules)
        applyRuleToText(rule, text);

    foreach (Rule rule, m_permanentRules)
        applyRuleToText(rule, text);

    setCurrentBlockState(0);

    int startIndex = 0;

    if (previousBlockState() != 1)
    {
        startIndex = commentStartExpression.indexIn(text);
    }

    while (startIndex >= 0)
    {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;

        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }

        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
     }
}
