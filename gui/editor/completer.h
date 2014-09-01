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

    QList<CodeParser::Element> allElements();
    QList<CodeParser::Element> functions();

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
