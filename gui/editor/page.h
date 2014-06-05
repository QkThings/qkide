#ifndef PAGE_H
#define PAGE_H

#include <QTextEdit>
#include <QPlainTextEdit>

class Highlighter;
class Completer;
class QAbstractItemModel;

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class CodeTip;

class LineNumberArea;

class Page : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit Page(const QString &name, QWidget *parent = 0);

    QString name();
    QString text();
    Highlighter* highlighter() { return m_highligher; }
    Completer* completer() { return m_completer; }
    
    void foldsLinePaintEvent(QPaintEvent *event);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);

signals:
    void keyPressed();
    void focused();
    void info(QString);
    
public slots:
    void slotFind(const QString &text, int flags);
    void slotReplace(const QString &prev, const QString &current, int flags, bool all);

private slots:
    void slotUpdateCodeBlocks();
    void slotTextChanged();
    void slotCursorPositionChanged();
    void insertCompletion(const QModelIndex &index);

    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

    void onChar(char c);

    void braceMatch();
    void autoIndent();

private:
    class CodeBlock {
    public:
        int startLine;
        int endLine;
    };
    QList<CodeBlock*> m_codeBlocks;
    enum {
        FoldsLineWidth = 0
    };
    Highlighter *m_highligher;
    Completer *m_completer;
    CodeTip *m_codeTip;
    QString m_name;
    int m_lastTextCursorPosition;

    QWidget *lineNumberArea;
    QWidget *foldsLine;

    int functionArg(int cursorPos, QStringList args);
    QStringList parseFunctionArgs(const QString &args);
    bool setFunctionTooltip(bool show = true);
    QTextLine currentTextLine(const QTextCursor &cursor);
    QString lineUnderCursor(const QTextCursor &cursor) const;
    QString textUnderCursor(const QTextCursor &cursor) const;
    QStringList keywordsFromFile(const QString& fileName);
    QAbstractItemModel *modelFromFiles(const QStringList fileNames);
    QAbstractItemModel *modelFromFile(const QString& fileName);
    
    void insertTabs(int n);
};

class FoldsLine : public QWidget
{
public:
    FoldsLine(Page *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(10, 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        codeEditor->foldsLinePaintEvent(event);
    }

private:
    Page *codeEditor;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(Page *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    Page *codeEditor;
};


#endif // PAGE_H
