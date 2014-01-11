#ifndef PMINITABLEWIDGET_H
#define PMINITABLEWIDGET_H

#include <QTableWidget>

class pMiniTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    enum RowColorHint
    {
        defaultRowColorHint = 0,
        addRowColorHint,
        removeRowColorHint,
        changeRowColorHint
    };

    explicit pMiniTableWidget(QWidget *parent = 0);

    void removeAll();
    void addItem(int row, int column, const QString &text);
    void setItemText(int row, int column, const QString &text);
    QString itemText(int row, int column);
    QStringList itemText(int column);
    void setCurrentRow(int row);
    void setRowColorHint(int row, RowColorHint hint);
    void setRowColor(int row, QColor color);
    RowColorHint rowColorHint(int row);
    QList<int> rows(RowColorHint hint);
    int hasText(const QString &text, int column, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    
signals:
    
public slots:

protected:
    const QColor m_defaultRowColor;
    const QColor m_addRowColor;
    const QColor m_removeRowColor;
    const QColor m_changeRowColor;
    
};

#endif // PMINITABLEWIDGET_H
