#ifndef PTEXTEDIT_H
#define PTEXTEDIT_H

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
