#include "page.h"
#include "highlighter.h"
#include "completer.h"

#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QTextCursor>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QStringListModel>
#include <QKeyEvent>
#include <QFile>
#include <QScrollBar>
#include <QTextStream>
#include <QPainter>
#include <QBrush>
#include <QTextBlock>
#include <QListWidget>

Page::Page(const QString &name, QWidget *parent) :
    QPlainTextEdit(parent),
    m_name(name),
    m_lastTextCursorPosition(0)
{
    m_highligher = new Highlighter(this->document());
    m_highligher->setLanguage("qk");

//    m_highligher->addKeywords(keywordsFromFile(":/completer/keywords1.txt"), QColor("#3171E8"));
//    m_highligher->addKeywords(keywordsFromFile(":/completer/keywords2.txt"), QColor("#FF6200"));
//    m_highligher->addKeywords(keywordsFromFile(":/completer/keywords3.txt"), QColor("#3171E8"));

#ifdef Q_OS_LINUX
    QFont font("Monospace", 9);
#else
    QFont font("Consolas", 10);
#endif
    setFont(font);

    QFontMetrics fm(font);
    setTabStopWidth(2*fm.width(" "));

    setWordWrapMode(QTextOption::NoWrap);
    setUndoRedoEnabled(true);

    m_completer = new Completer();
    m_completer->setWidget(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseSensitive);
    QStringList keywordsFileNames;
    keywordsFileNames.append(":/completer/keywords1.txt");
    keywordsFileNames.append(":/completer/keywords2.txt");
    m_completer->setModel(modelFromFiles(keywordsFileNames));

    QObject::connect(m_completer, SIGNAL(activated(QString)),
                     this, SLOT(insertCompletion(QString)));

    lineNumberArea = new LineNumberArea(this);
    foldsLine = new FoldsLine(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    //connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()));

    updateLineNumberAreaWidth(0);
    //highlightCurrentLine();

    //connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(braceMatch()));
    //connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));

    setFrameStyle(QFrame::NoFrame);
}

QString Page::name()
{
    return m_name;
}

QString Page::text()
{
    return toPlainText();
}

void Page::slotFind(const QString &text, int flags)
{
    qDebug() << "slotFind()" << text;
    find(text, (QTextDocument::FindFlag) flags);

}

void Page::slotReplace(const QString &text, int flags)
{

}

void Page::slotReplaceAll(const QString &text, int flags)
{

}

QStringList Page::keywordsFromFile(const QString &fileName)
{
    QStringList list;
    QFile file(fileName);
    if(file.open(QFile::ReadOnly)){
        QTextStream input(&file);
        list.clear();
        while(!input.atEnd()) {
            QString line = input.readLine();
            list.append(line.remove("\n"));
        }
        file.close();
    }
    return list;
}

QAbstractItemModel* Page::modelFromFiles(const QStringList fileNames)
{
    QStringList words;
    QString fileName;

    foreach(fileName, fileNames) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly)) {

            while (!file.atEnd()) {
                QByteArray line = file.readLine();
                if (!line.isEmpty())
                    words << line.trimmed();
            }
            file.close();
        }
    }

    return new QStringListModel(words, m_completer);
}

QAbstractItemModel* Page::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(m_completer);

    QStringList words;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << line.trimmed();
    }
    file.close();

    return new QStringListModel(words, m_completer);
}

void Page::mousePressEvent(QMouseEvent *e)
{
    QPlainTextEdit::mousePressEvent(e);
    m_lastTextCursorPosition = textCursor().position();
}

void Page::keyPressEvent(QKeyEvent *e)
{
    qDebug() << "keyPressEvent()";

    if(isReadOnly() && (e->modifiers() == Qt::NoModifier)) {
        qDebug() << "can't edit, read-only file!";
        emit info(tr("This is a read-only project.\n"
                     "To change it you need to re-save it to another location."));
        return;
    }

    if(m_completer->popup()->isVisible()) {

        switch(e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
             e->ignore();
             return; // let the completer do default behavior
        default: break;
        }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space);
    if(!isShortcut)
        QPlainTextEdit::keyPressEvent(e);

    onChar(e->text().at(0).toLatin1());

    if(e->key() == Qt::Key_Home) {
        QString selText;
        QTextCursor tc;

        bool select = e->modifiers() & Qt::ShiftModifier;

        tc = textCursor();
        if(!select) {
            tc.setPosition(m_lastTextCursorPosition, QTextCursor::MoveAnchor);
            tc.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
        }

        bool test = false;
        selText = tc.selectedText();
        if(!select & selText.isEmpty()) {
            test = true;
        }
        else {
            selText = selText.remove(' ');
            selText = selText.remove('\t');
            if(!selText.isEmpty())
                test = true;
        }

        qDebug() << "selText" << selText;
        if(test) {

            int startPos, indentPos, cursorPos, endPos;

            tc = textCursor();
            tc.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            startPos = tc.position();
            tc.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
            endPos = tc.position();

            //QString line = tc.selectedText();
            indentPos = startPos;
            QString esc = "\n\t ";
            while(esc.contains(document()->characterAt(indentPos)))
                indentPos++;

            cursorPos = m_lastTextCursorPosition;


            QTextCursor::MoveMode moveMode;
            if(select)
                moveMode = QTextCursor::KeepAnchor;
            else
                moveMode = QTextCursor::MoveAnchor;

            qDebug() << startPos << indentPos << cursorPos << endPos;

            if(select)
                tc.setPosition(textCursor().selectionEnd(), QTextCursor::MoveAnchor);

            if(cursorPos == startPos) {
                tc.setPosition(indentPos, moveMode);
            }
            else if(cursorPos == indentPos) {
                tc.setPosition(startPos, moveMode);
            }
            else if(cursorPos > indentPos) {
                tc.setPosition(indentPos, moveMode);
            }

            setTextCursor(tc);
        }
    }
    m_lastTextCursorPosition = textCursor().position();
    // Tabs as spaces
    /*if(e->key() == Qt::Key_Tab) {
        textCursor().deletePreviousChar();
        insertPlainText("  ");
    }*/
    if(e->key() == Qt::Key_Return)
        autoIndent();

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (ctrlOrShift && e->text().isEmpty())
        return;

    //static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    static QString eow("~!@#$%^&*()+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    QString input = e->text();
    input = input.remove(' ');
    input = input.remove('\t');
    input = input.remove(QChar(8));
    input = input.remove(QChar(13));
    input = input.remove(QChar(26));
    /*input = input.remove('(');
    input = input.remove(')');
    input = input.remove(';');*/
    if (!isShortcut && (hasModifier || input.isEmpty() || completionPrefix.length() < 3
                   || eow.contains(input.right(1)))) {
        m_completer->popup()->hide();
        return;
    }

    qDebug() << input;
    qDebug() << (int)input.at(0).unicode();
    qDebug() << input.isEmpty();
    qDebug() << "completionPrefix" << completionPrefix;

    completionPrefix.remove('(');
    completionPrefix.remove(')');
    completionPrefix.remove(';');

    if (completionPrefix != m_completer->completionPrefix()) {
        m_completer->setCompletionPrefix(completionPrefix);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
             + m_completer->popup()->verticalScrollBar()->sizeHint().width());
    m_completer->complete(cr); // popup it up!
}

QString Page::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void Page::focusInEvent(QFocusEvent *e)
{
    m_completer->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
    emit focused();
}

void Page::insertCompletion(const QString &completion)
{
    QTextCursor tc = textCursor();
    int extra = completion.length() - m_completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
    /*if(completion.contains("qk_")) {
        insertPlainText("();");
        tc = textCursor();
        tc.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, 2);
        setTextCursor(tc);
    }*/
}

int Page::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    if(digits < 3)
        digits = 3;

    int space = 5 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}



void Page::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth()+FoldsLineWidth, 0, 0, 0);
}



void Page::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) {
        lineNumberArea->scroll(0, dy);
        foldsLine->scroll(0, dy);
    }
    else {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
        foldsLine->update(lineNumberAreaWidth(), rect.y(), FoldsLineWidth, rect.height());
        foldsLine->show();
        foldsLine->repaint();
    }

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}



void Page::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    foldsLine->setGeometry(QRect(lineNumberAreaWidth(),cr.top(), FoldsLineWidth, cr.height()));
}

void Page::braceMatch()
{
    qDebug() << "braceMatch()";

    static int lastBeginPos = 0;
    static int lastEndPos = 0;

    const QString braceBeginChar = "{(";
    const QString braceEndChar = "})";

    QTextCharFormat normalFormat;
    QFont f(font());
    f.setBold(false);
    normalFormat.setFont(f);

    QTextCharFormat braceMatchFormat;
    f.setBold(true);
    braceMatchFormat.setBackground(QBrush(QColor(Qt::yellow)));
    braceMatchFormat.setFont(f);

    QTextDocument *doc = document();

    static QTextCursor braceBeginCursor;
    static QTextCursor braceEndCursor;

    setUndoRedoEnabled(false);

    if (!braceBeginCursor.isNull() || !braceEndCursor.isNull()) {
            braceBeginCursor.setCharFormat(QTextCharFormat());
            braceEndCursor.setCharFormat(QTextCharFormat());
            //braceBeginCursor = braceEndCursor = QTextCursor();
    }

    QTextCursor tc = textCursor();

    int pos = tc.position();
    QChar braceChar;

    int n = 2;
    while(n--) {
       braceChar = doc->characterAt(pos);
       qDebug() << "braceChar" << braceChar << "pos" << pos;
       if(braceBeginChar.contains(braceChar) || braceEndChar.contains(braceChar))
           break;
       pos--;
    }
    qDebug() << "n" << n;
    if(n < 0) {
        setUndoRedoEnabled(true);
        return;
    }

    qDebug() << "brace found!";

    braceBeginCursor = textCursor();
    braceBeginCursor.setPosition(pos);
    braceBeginCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

    lastBeginPos = pos;

    bool forward;
    if(braceBeginChar.contains(braceChar))
        forward = true;
    else
        forward = false;

    if (forward)
        pos++;
    else
        pos--;

    QChar matchBrace;
    switch(braceChar.toLatin1()) {
        case '(': matchBrace = ')'; break;
        case ')': matchBrace = '('; break;
        case '{': matchBrace = '}'; break;
        case '}': matchBrace = '{'; break;
        default: ;
    }

    int braceDepth = 1;

    QChar c;

    while (!(c = doc->characterAt(pos)).isNull()) {
        if (c == braceChar) {
            braceDepth++;
        } else if (c == matchBrace) {
            braceDepth--;
            if (braceDepth == 0) {
                lastEndPos = pos;
                braceEndCursor = textCursor();
                braceEndCursor.setPosition(pos);
                braceEndCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                braceEndCursor.setCharFormat(braceMatchFormat);
                break;
            }
        }
        forward ? pos++ : pos--;
    }
    braceBeginCursor.setCharFormat(braceMatchFormat);

    setUndoRedoEnabled(true);

    return;
}

void Page::autoIndent()
{
    qDebug() << "autoIndent()";

    QString esc = "\n\t ";
    QTextCursor tc = textCursor();
    int returnPos = tc.position();

    int linePos = returnPos - 1;
    tc.setPosition(linePos);
    while(linePos >= 0 && esc.contains(document()->characterAt(linePos).isLetterOrNumber()))
        linePos--;

    tc.setPosition(linePos);
    tc.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    int startPos = tc.position();
    int indentPos = startPos;
    while(esc.contains(document()->characterAt(indentPos)))
        indentPos++;

    tc.setPosition(startPos, QTextCursor::MoveAnchor);
    tc.setPosition(indentPos, QTextCursor::KeepAnchor);
    QString str = tc.selectedText();
    int tabs = str.count('\t');
    tabs += str.count(' ')/2;

    tc.setPosition(returnPos);
    setTextCursor(tc);
    insertTabs(tabs);
}

void Page::insertTabs(int n)
{
    while(n--)
        insertPlainText("\t");
        //insertPlainText("  ");
}

void Page::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor("#EDEFFD");
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void Page::onChar(char c)
{
    QTextCursor tc = textCursor();
    if(c == '\"') {
        insertPlainText("\"");
        tc.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
        setTextCursor(tc);
    }
    else if(c == '{' || c == '}')
        slotUpdateCodeBlocks();

}

void Page::slotUpdateCodeBlocks()
{
    QString in(text());
    QTextStream ts(&in);
    QList<int> stack;
    int lineNumber = 0;

    if(!m_codeBlocks.isEmpty())
        qDeleteAll(m_codeBlocks.begin(), m_codeBlocks.end());

    while(ts.atEnd()) {
        QString line = ts.readLine();
        lineNumber++;
        if(line.contains("{")){
            stack.append(lineNumber);
        }
        else if(line.contains("}")){
            if(!stack.isEmpty()) {
                CodeBlock *cb = new CodeBlock;
                cb->startLine =  stack.last();
                cb->endLine = lineNumber;
                m_codeBlocks.append(cb);
            }
        }
    }
}

void Page::slotTextChanged()
{
    document()->setModified();

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
    QString str = cursor.selectedText();
    qDebug() << "str" << str;
    //char c = str.at(0).toLatin1();
    //onChar(c);
}

void Page::slotCursorPositionChanged()
{
    qDebug() << "cursor pos" << textCursor().position();
}

void Page::foldsLinePaintEvent(QPaintEvent *event)
{
    QPainter painter(foldsLine);
    painter.fillRect(event->rect(), QColor("#ddd"));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    int lineNumber;

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            lineNumber = blockNumber + 1;
            QString dash;
            if(block.text().contains("{"))
                dash = "-";
            else
                dash = " ";
            painter.setPen(QColor("#aaa"));
            painter.setFont(font());
            painter.drawText(foldsLine->rect().x(),top,foldsLine->rect().width(),fontMetrics().height(),
                             Qt::AlignCenter, dash);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void Page::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor("#eee"));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor("#888888"));
            painter.setFont(font());
            painter.drawText(-2, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
