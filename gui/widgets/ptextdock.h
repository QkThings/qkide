#ifndef TEXTDOCK_H
#define TEXTDOCK_H

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
