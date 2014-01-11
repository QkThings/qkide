#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

class Highlighter;


class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *document);

    QString& getLanguage();


protected:
    void highlightBlock(const QString &text);


public slots:
    void addKeywords(QStringList keywords, const QColor &color);
    void setLanguage(const QString &language);


    void clearHighlightingRule();


private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    struct Keywords {
        QStringList words;
        QColor color;
    };

    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat multiLineCommentFormat;

    QString curLanguage;


    QList<Keywords> keywords;
};

#endif // HIGHLIGHTER_H
