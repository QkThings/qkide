#include "codetip.h"
#include "ui_codetip.h"

#include <QDebug>

CodeTip::CodeTip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CodeTip)
{
    ui->setupUi(this);

    setWindowFlags(Qt::ToolTip);

    QPalette p(palette());
    p.setColor(QPalette::ToolTipText, Qt::black);
    p.setColor(QPalette::Text, Qt::black);
    setPalette(p);
}

CodeTip::~CodeTip()
{
    delete ui;
}

void CodeTip::setFont(const QFont &font)
{
    ui->text->setFont(font);
}

void CodeTip::setText(const QString &text)
{
    ui->text->setText(text);
    QFontMetrics fm(ui->text->font());
    QString plainText = text;
    plainText.remove(QRegularExpression("(<.+?>)"));
    setFixedWidth(fm.width(plainText)+4);
}
