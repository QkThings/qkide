#include "highlighter.h"

#include <QDebug>
#include <QMessageBox>
#include <QDialog>
#include <QFile>
#include <QApplication>
#include "codeparser.h"
#include "qkide_global.h"

QVector<Highlighter::Rule> Highlighter::m_permanentRules;
QVector<Highlighter::Rule> Highlighter::m_extraRules;
bool Highlighter::m_permanentRule;

Highlighter::Highlighter(QTextDocument *document) : QSyntaxHighlighter(document)
{
    Rule rule;

    multiLineCommentFormat.setForeground(Qt::gray);
    //multiLineCommentFormat.setFontWeight(QFont::Bold);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");

    setupPermanentRules();
}

void Highlighter::setupPermanentRules()
{   
    qDebug() << "Highlighter::setupPermanentRules()";
    QString keywordColor;
    QString keyword;
    QTextCharFormat keywordFormat;
    //QFile file(":/syntax/" + language);
    QFile file(":/syntax/qk.kw");
    bool stop = false;
    QString elementName, color,weight;
    Rule rule;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(new QDialog,
                              tr("Error"),
                              tr("Error load syntax higlighter"),
                              QMessageBox::Ok);

        qApp->quit();
    }

    QTextStream input(&file);

    m_permanentRules.clear();

    //keywordFormat.setFontWeight(QFont::Bold);

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
            {
                stop = true;
            }
            else
            {
                //qDebug() << keyword;
                rule.elementName = elementName;
                rule.format  = keywordFormat;
                rule.pattern = QRegExp(keyword);
                m_permanentRules.append(rule);
            }
        }

        if(rule.pattern.isEmpty())
            m_permanentRules.append(rule);

        qDebug() << "rule" << rule.elementName << rule.format.foreground().color();

        stop = false;
    }
}

void Highlighter::addElements(QList<CodeParser::Element> elements, bool permanent)
{
    foreach(CodeParser::Element el, elements)
        addElement(el, permanent);
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
    rule.pattern = QRegExp(element.text);

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

//void Highlighter::addRule(const QRegExp &regex, SyntaxElement element, const QColor &color)
//{
//    Highlighter::Rule rule;
//    QTextCharFormat textFormat;
//    QColor textColor;

//    switch(element)
//    {
//    case Define:
//        rule.elementName = "define";
//        break;
//    case Function:
//        rule.elementName = "function";

//        break;
//    case Custom:
//    default:
//        rule.elementName = "custom";
//        textColor = color;
//    }
//    switch(element)
//    {
//    case Define:
//    case Function:
//        textColor = elementColor(rule.elementName);
//        break;
//    default:
//        textColor = color;
//    }

//    textFormat.setForeground(QBrush(textColor));
//    rule.format = textFormat;
//    rule.pattern = regex;

//    if(m_permanentRule)
//        m_permanentRules.append(rule);
//    else
//        m_extraRules.append(rule);
//}

//void Highlighter::clearRules()
//{
//    m_extraRules.clear();
//}


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

    //if(index >= 0)
      //  qDebug() << rule.pattern << "---" << text;

    while (index >= 0)
    {
        int length = expression.matchedLength();
        setFormat(index, length, rule.format);
        index = expression.indexIn(text, index + length);
    }
}

void Highlighter::highlightBlock(const QString &text)
{
    if(m_extraRules.isEmpty())
        qDebug() << "extra rules EMPTY";

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
