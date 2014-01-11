#include "completer.h"

#include <QAbstractItemView>

Completer::Completer(QObject *parent) :
    QCompleter(parent)
{
    QAbstractItemView *p = this->popup();

    p->setFont(QFont("Consolas", 9));
}
