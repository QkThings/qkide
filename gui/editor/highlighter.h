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

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include "codeparser.h"

class Highlighter;

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    enum SyntaxElement
    {
        Custom = 0,
        Define,
        Function
    };
    class Rule
    {
    public:
        QString elementName;
        QRegExp pattern;
        QTextCharFormat format;
    };
    Highlighter(QTextDocument *document);

    bool setSyntax(const QString &filePath);

    void addElements(QList<CodeParser::Element> elements, bool permanent = false);
    void addElement(const CodeParser::Element &element, bool permanent = false);
    void clearElements(bool permanent = false);

    //static void beginPermanentRules() { m_permanentRule = true; }
    //static void addRule(const QRegExp &regex, SyntaxElement element, const QColor &color = QColor());
    //static void clearRules();
    //static void endPermanentRules() { m_permanentRule = false;}

    QColor elementColor(const QString &elementName);

protected:
    void highlightBlock(const QString &text);


public slots:
    //void addKeywords(QStringList keywords, const QColor &color);


private:
    struct Keywords {
        QStringList words;
        QColor color;
    };

    QVector<Rule> m_permanentRules;
    QVector<Rule> m_extraRules;
    bool m_permanentRule;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat multiLineCommentFormat;

    QString curLanguage;

    QList<Keywords> keywords;

//    void setupPermanentRules();
    void applyRuleToText(const Rule &rule, const QString &text);
};

#endif // HIGHLIGHTER_H
