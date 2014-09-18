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

#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QStringList>

class Project : public QObject
{
    Q_OBJECT
public:
    explicit Project(const QString &name = QString(), QObject *parent = 0);

    static QString defaultName();

    void setName(const QString name);
    void setPath(const QString path);
    void addFile(const QString fileName);
    void removeFile(const QString fileName);
    bool hasFile(const QString fileName);
    void setReadOnly(bool readOnly);
    bool readOnly();

    QString name();
    QString path();
    QStringList files();
    QStringList files(const QString &ext);
    
signals:
    void opened();
    void filesChanged();
    void closed();

public slots:
    void save();
    void clear();
    void update();
    bool loadFromFile(const QString filePath);
    bool saveToFile(const QString filePath);

private:
    QString m_name;
    QString m_path;
    QStringList m_files;
    QString m_targetName;
    QString m_targetVariant;
    bool m_readOnly;
    
};

#endif // PROJECT_H
