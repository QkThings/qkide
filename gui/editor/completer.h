#ifndef COMPLETER_H
#define COMPLETER_H

#include <QCompleter>
#include "codeparser.h"

class QStandardItemModel;

class Completer : public QCompleter
{
    Q_OBJECT
public:
    explicit Completer(QObject *parent = 0);

public slots:
    void addElements(QList<CodeParser::Element> elements, bool permanent = false);
    void clearElements(bool permanent = false);

private slots:

signals:


private:
    void addElement(const CodeParser::Element &element, bool permanent = false);
    void updateModel();
    void createItem(CodeParser::Element &element);

    QList<CodeParser::Element> m_extraElements;
    QList<CodeParser::Element> m_permanentElements;
    QStandardItemModel *m_model;
};

#endif // COMPLETER_H
