#include <QTextEdit>

#include "qkide_global.h"
#include "ptextdock.h"
#include "ptextedit.h"

#include <QDebug>

pTextDock::pTextDock(const QString &title, const QColor &backgroundColor, QWidget *parent) :
    QDockWidget(title, parent),
    m_defaultTextColor(Qt::black)
{   
    m_textEdit = new pTextEdit(QSize(100, 120), this);
    m_textEdit->setTextColor(m_defaultTextColor);
    m_textEdit->setFont(QFont(EDITOR_FONT_NAME));
    m_textEdit->setReadOnly(false);
    m_textEdit->setFrameStyle(QFrame::NoFrame);

    QPalette p;
    p.setColor(QPalette::Base, backgroundColor);
    m_textEdit->setPalette(p);

    setWidget(m_textEdit);
}

pTextDock::~pTextDock()
{
    delete m_textEdit;
}


QSize pTextDock::sizeHint() const
{
    return m_textEdit->sizeHint();
}

QTextEdit* pTextDock::textEdit()
{
    return m_textEdit;
}

void pTextDock::setDefaultTextColor(const QColor &color)
{
    m_defaultTextColor = color;
    m_textEdit->setTextColor(m_defaultTextColor);
}

void pTextDock::append(const QString &text, const QColor &color)
{
    m_textEdit->setTextColor(color);
    m_textEdit->append(text);

    QTextCursor tc = m_textEdit->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.movePosition(QTextCursor::StartOfLine);
    m_textEdit->setTextCursor(tc);
}

void pTextDock::append(const QString &text)
{   
    append(text, m_defaultTextColor);
}

QString pTextDock::text()
{
    return m_textEdit->toPlainText();
}

void pTextDock::clear()
{
    m_textEdit->clear();
}
