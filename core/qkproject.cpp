#include "qkproject.h"
#include "../qkide_global.h"

#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QXmlStreamAttribute>
#include <QVariant>
#include <QDir>

QkProject::QkProject(const QString &name, QObject *parent) :
    QObject(parent)
{
    if(name.isEmpty())
        m_name = QkProject::defaultName();
    else
        m_name = name;

    m_readOnly = false;
}

QString QkProject::defaultName()
{
    QString defaultProjectName("project_");
    QString dateTimeStr(QDateTime::currentDateTime().toString("yyMMddhhmmss"));
    defaultProjectName.append(dateTimeStr);
    return defaultProjectName;
}

void QkProject::setName(const QString name)
{
    m_name = name;
}

void QkProject::setPath(const QString path)
{
    m_path = path;
}

void QkProject::addFile(const QString fileName)
{
    if(hasFile(fileName)) return;

    qDebug() << "project add file:" << fileName;
    m_files.append(fileName);

    emit filesChanged();
}

void QkProject::removeFile(const QString fileName)
{
    if(!hasFile(fileName)) return;

    qDebug() << "project remove file:" << fileName;
    m_files.removeOne(fileName);

    emit filesChanged();
}

bool QkProject::hasFile(const QString fileName)
{
    foreach(QString file, m_files)
    {
        if(file.compare(fileName,Qt::CaseInsensitive) == 0)
            return true;
    }
    return false;
}

void QkProject::setReadOnly(bool readOnly)
{
    m_readOnly = readOnly;
}

bool QkProject::readOnly()
{
    return m_readOnly;
}

QString QkProject::name()
{
    return m_name;
}

QString QkProject::path()
{
    return m_path;
}

QStringList QkProject::files()
{
    return m_files;
}

QStringList QkProject::files(const QString &ext)
{
    QStringList list;

    foreach(QString fileName, m_files)
    {
        if(fileName.contains(ext))
            list.append(fileName);
    }

    list.sort();
    return list;
}

void QkProject::update()
{
    if(!m_path.isEmpty())
        saveToFile(m_path + m_name + ".qkpro");
}

void QkProject::clear()
{
    m_name.clear();
    m_path.clear();
    m_files.clear();

    emit closed();
}

void QkProject::save()
{
    /*if(m_path.isEmpty()){
        qDebug() << "can't save files: project path is empty";
        return;
    }

    if(m_files.isEmpty()) {
        qDebug() << "project has no files to be saved";
        return;
    }

    foreach(QString file, m_files) {
        QString filePath = m_path + "/" + file;

        qDebug() << "saveFile" << filePath;

        QFile file(filePath);
        if(!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "Can't open" << filePath;
            return;
        }
        //file.write(page(index)->text().toLatin1());
        file.close();
    }*/
}

bool QkProject::loadFromFile(const QString path)
{
    QFile file(path);
    QString nodeName, attrName, projectName, fileName;
    bool readOnly = false;
    QVariant var;

    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "unable to open:" << path << file.errorString();
        return false;
    }

    clear();

    QXmlStreamReader xml(&file);

    while(!xml.atEnd())
    {
        xml.readNext();

        if(xml.isStartElement())
        {
            nodeName = xml.name().toString();
            //qDebug() << "xml node name:" << nodeName;

            if(nodeName.compare("project") == 0)
            {
                foreach(QXmlStreamAttribute attr, xml.attributes().toList())
                {
                    attrName = attr.name().toString();
                    if(attrName.compare("name") == 0) {
                        projectName = attr.value().toString();
                        qDebug() << "project name:" << projectName;
                    }
                    else if(attrName.compare("readOnly") == 0) {
                        var.setValue(attr.value().toString());
                        readOnly = var.toBool();
                        qDebug() << "project readOnly:" << readOnly;
                    }
                }
            }
            else if(nodeName.compare("file") == 0)
            {
                xml.readNext();
                fileName = xml.text().toString();
                addFile(fileName);
            }
        }
    }
    if(xml.hasError())
    {
        qDebug() << "xml error:" << xml.errorString();
    }else
    {
        QStringList list = path.split("/");
        QString projectPath, last = list.last();
        m_name = projectName;
        projectPath = path;
        projectPath.chop(last.count());
        qDebug() << "project NAME" << projectName;
        qDebug() << "project PATH" << projectPath << path;
        m_path = projectPath;
        m_readOnly = readOnly;
    }

    emit opened();

    return true;
}

bool QkProject::saveToFile(const QString filePath)
{
    QVariant var;

    qDebug() << "save project to file" << filePath;

    if(filePath.isEmpty())
        return false;
    else
    {
        QFile file(filePath);

        if(!file.open(QIODevice::WriteOnly)) {
            qDebug() << "unable to create file:" << file.errorString(); // !!!!!
            return false;
        }

        QXmlStreamWriter stream(&file);
        stream.setAutoFormatting(true);
        stream.writeStartDocument();

        stream.writeStartElement("project");
        stream.writeAttribute("name", m_name);
        var.setValue(m_readOnly);
        stream.writeAttribute("readOnly", var.toString());

        foreach(QString name, m_files){
            stream.writeTextElement("file", name);

            qDebug() << "project save file:" << name;
        }

        stream.writeEndElement(); // bookmark

        stream.writeEndDocument();
    }

    return true;
}
