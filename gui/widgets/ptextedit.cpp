#include "ptextedit.h"

pTextEdit::pTextEdit(QSize customSizeHint, QWidget *parent) :
    QTextEdit(parent)
{
    m_customSizeHint = customSizeHint;
    setWordWrapMode(QTextOption::NoWrap);
}

QSize pTextEdit::sizeHint() const
{
    return m_customSizeHint;
}
