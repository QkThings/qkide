#ifndef QKPROJECT_H
#define QKPROJECT_H

#include <QObject>
#include <QStringList>

class QkProject : public QObject
{
    Q_OBJECT
public:
    explicit QkProject(const QString &name = QString(), QObject *parent = 0);

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

#endif // QKPROJECT_H
