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

#include <QDockWidget>

class pTextEdit;
class QTextEdit;

class pTextDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit pTextDock(const QString & title, const QColor &backgroundColor = Qt::white, QWidget *parent = 0);
    ~pTextDock();

    QSize sizeHint() const;

    QTextEdit *textEdit();
    void setDefaultTextColor(const QColor &color);
    void append(const QString &text, const QColor &color);
    void append(const QString &text);
    QString text();
    void clear();

    
signals:
    
public slots:

private:
    pTextEdit *m_textEdit;
    QColor m_defaultTextColor;
    
};

#endif // TEXTDOCK_H
