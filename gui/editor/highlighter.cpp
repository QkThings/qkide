#include "highlighter.h"

#include <QDebug>
#include <QMessageBox>
#include <QDialog>
#include <QFile>
#include <QApplication>

Highlighter::Highlighter(QTextDocument *document) : QSyntaxHighlighter(document)
{
    HighlightingRule rule;

    multiLineCommentFormat.setForeground(Qt::gray);
    //multiLineCommentFormat.setFontWeight(QFont::Bold);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}


QString& Highlighter::getLanguage()
{
    return curLanguage;
}

void Highlighter::addKeywords(QStringList keywords, const QColor &color)
{
    QTextCharFormat keywordFormat;
    HighlightingRule rule;

    keywordFormat.setForeground(QBrush(QColor(color)));

    qDebug() << keywords;

    foreach(QString keyword, keywords) {
        keyword.prepend("\\b");
        keyword.append("\\b");
        rule.format = keywordFormat;
        rule.pattern = QRegExp(keyword);
        highlightingRules.append(rule);
    }
}

void Highlighter::setLanguage(const QString &language)
{
    QString keywordColor;
    QString keyword;
    QTextCharFormat keywordFormat;
    //QFile file(":/syntax/" + language);
    QFile file(":/syntax/qk.kw");
    bool stop = false;
    QString color,weight;
    HighlightingRule rule;

    curLanguage = language;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(new QDialog,
                              tr("Error"),
                              tr("Error load syntax higlighter"),
                              QMessageBox::Ok);

        qApp->quit();
    }

    QTextStream input(&file);

    highlightingRules.clear();

    //keywordFormat.setFontWeight(QFont::Bold);

    while (!input.atEnd())
    {
        keywordColor = input.readLine();

        if (keywordColor.contains("#rule"))
        {
            color = keywordColor.section(',',1,1);
            weight = keywordColor.section(',',2,2);
            keywordFormat.setForeground(QBrush(QColor(color)));
            if(weight.toLower() == "bold")
                keywordFormat.setFontWeight(QFont::Bold);
            else
                keywordFormat.setFontWeight(QFont::Normal);
        }

        while (stop == false)
        {
            keyword = input.readLine();

            if (keyword == "stop")
            {
                stop = true;
            }
            else
            {
                qDebug() << keyword;
                rule.format  = keywordFormat;
                rule.pattern = QRegExp(keyword);
                highlightingRules.append(rule);
            }
        }

        stop = false;
    }
}


void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);

        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

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


void Highlighter::clearHighlightingRule()
{
    highlightingRules.clear();
}
