#ifndef QKREFERENCEWIDGET_H
#define QKREFERENCEWIDGET_H

#include <QWidget>

namespace Ui {
class QkReferenceWidget;
}

class QkReferenceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QkReferenceWidget(QWidget *parent = 0);
    ~QkReferenceWidget();

private slots:
    void slotReload();


private:
    Ui::QkReferenceWidget *ui;
};

#endif // QKREFERENCEWIDGET_H
