#ifndef CODETIP_H
#define CODETIP_H

#include <QWidget>

namespace Ui {
class CodeTip;
}

class CodeTip : public QWidget
{
    Q_OBJECT

public:
    explicit CodeTip(QWidget *parent = 0);
    ~CodeTip();

    void setFont(const QFont &font);
    void setText(const QString &text);

private:
    Ui::CodeTip *ui;

};

#endif // CODETIP_H
