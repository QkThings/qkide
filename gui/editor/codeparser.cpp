#include "codeparser.h"
#include "qkide_global.h"

#include <QProcess>
#include <QDebug>
#include <QApplication>
#include <QFile>
#include <QTimer>
#include <QEventLoop>

CodeParser::CodeParser(QObject *parent) :
    QObject(parent)
{

}

void CodeParser::parse(const QString &path)
{
    qDebug() << __FUNCTION__ << path;
    QString program = QApplication::applicationDirPath() + CTAGS_EXE;
    QStringList arguments;
    //QString output = "-f " + TAGS_DIR + "/tags";
    QString tagsDir = TAGS_DIR;
    QString output = "-f " + tagsDir.remove(0,1) + "/tags";
    arguments << output << "--languages=-Make" << "--c-kinds=+p-m" << "-R" << path;
    //arguments << "-R" << "--c-kinds=+l" << "--languages=-Make" << path << "--verbose";
    //arguments << "-R" << path;

    QProcess process(this);
    process.setProcessChannelMode(QProcess::MergedChannels);
    //process.setWorkingDirectory(path);

    qDebug() << program << arguments;

    process.start(program, arguments);
    process.waitForFinished(30000);

    //qDebug() << process.readAll();

    QFile tags(QApplication::applicationDirPath() + TAGS_DIR + "/tags");
    if(!tags.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "failed to open tags file:" << tags.fileName();
    }

    clear();
    while(!tags.atEnd())
    {
        QString line = tags.readLine();
        if(line[0] == '!' || line[0] == '_')
            continue;
        QStringList fields = line.split('\t');
        //qDebug() << "fields" << fields << "count:" << fields.count();
        Element el;
        el.text = fields[0].remove('"');
        el.fileName = fields.at(1);
        el.expression = fields[2].remove('"');
        el.local = false;
        el.type = Element::Unknown;

        QString typeStr = fields.at(3);
        typeStr.remove('\n');
        if(typeStr == "f" || typeStr == "p")
            el.type = Element::Function;
        else if(typeStr == "d")
            el.type = Element::Define;
        else if(typeStr == "v")
            el.type = Element::Variable;
        else if(typeStr == "t")
            el.type = Element::Typedef;
        else if(typeStr == "e")
            el.type = Element::Enum;

        switch(el.type)
        {
        case Element::Function:
            if(!hasElement(el.text, m_functions))
                m_functions.append(el);
            break;
        case Element::Define:
            if(!el.text.endsWith("_H") && !hasElement(el.text, m_defines))
                m_defines.append(el);
            break;
        case Element::Enum:
            if(!hasElement(el.text, m_enums))
                m_enums.append(el);
            break;
        case Element::Variable:
            if(!hasElement(el.text, m_variables))
                m_variables.append(el);
            break;
        case Element::Typedef:
            if(!hasElement(el.text, m_types))
                m_types.append(el);
            break;
        case Element::Unknown:
        default:
            ;//qDebug() << "unkown code element type" << typeStr << " el.text =" << el.text;
        }

    }

//    qDebug() << "Functions:";
//    foreach(const Element &el, m_functions)
//        qDebug() << el.text;
//    qDebug() << "Defines:";
//    foreach(const Element &el, m_defines)
//        qDebug() << el.text;
//    qDebug() << "Variables:";
//    foreach(const Element &el, m_variables)
//        qDebug() << el.text;

    emit parsed();
}

QList<CodeParser::Element> CodeParser::allElements()
{
    QList<Element> allElements;
    allElements.append(m_defines);
    allElements.append(m_enums);
    allElements.append(m_types);
    allElements.append(m_functions);
    allElements.append(m_variables);
    return allElements;
}

void CodeParser::clear()
{
    m_defines.clear();
    m_enums.clear();
    m_types.clear();
    m_functions.clear();
    m_variables.clear();
}

bool CodeParser::hasElement(const QString &text, const QList<Element> &list)
{
    foreach(const Element &el, list)
        if(el.text == text)
            return true;
    return false;
}

CodeParserThread::CodeParserThread(QObject *parent) :
    QThread(parent)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(200);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotParse()));
}

void CodeParserThread::startTimer()
{
    qDebug() << "TIMER STARTED";
    m_timer->start();
    //QEventLoop eventLoop;
    //connect(m_timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    //m_timer->start();
    //eventLoop.exit();
}

void CodeParserThread::slotParse()
{
    if(m_parser != 0)
        m_parser->parse(m_path);
}

void CodeParserThread::slotParsed()
{
    qDebug() << "parsed";
}

void CodeParserThread::run()
{
    CodeParser parser;
    m_parser = &parser;
    //connect(m_parser, SIGNAL(parsed()), this, SLOT(slotParsed()), Qt::DirectConnection);
    connect(m_parser, SIGNAL(parsed()), this, SIGNAL(parsed()));
    exec();
}
