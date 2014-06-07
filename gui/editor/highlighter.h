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
