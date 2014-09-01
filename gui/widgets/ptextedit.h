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

#include <QTextEdit>

class pTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit pTextEdit(QSize customSizeHint, QWidget *parent = 0);

    QSize sizeHint() const;
    
signals:
    
public slots:

private:
    QSize m_customSizeHint;
    
};

#endif // PTEXTEDIT_H
