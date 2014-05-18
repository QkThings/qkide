#include "page.h"
#include "highlighter.h"
#include "completer.h"
#include "codetip.h"

#include "qkide_global.h"

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
#include <QToolTip>
#include <QKeyEvent>
#include <QAbstractItemModel>
#include <QStandardItemModel>

Page::Page(const QString &name, QWidget *parent) :
    QPlainTextEdit(parent),
    m_name(name),
    m_lastTextCursorPosition(0)
{
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::white);
    p.setColor(QPalette::Text, QColor("#222"));
    setPalette(p);

    setFont(QFont(EDITOR_FONT_NAME, EDITOR_FONT_SIZE));
    QFontMetrics fm(font());
    setTabStopWidth(4*fm.width(" ")+1);

    setWordWrapMode(QTextOption::NoWrap);
    setUndoRedoEnabled(true);

    m_highligher = new Highlighter(this->document());

    m_completer = new Completer();
    m_completer->setWidget(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseSensitive);

    connect(m_completer, SIGNAL(activated(QModelIndex)),
            this, SLOT(insertCompletion(QModelIndex)));


    m_codeTip = new CodeTip(this);
    m_codeTip->hide();

    lineNumberArea = new LineNumberArea(this);
    foldsLine = new FoldsLine(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    //connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()));

    updateLineNumberAreaWidth(0);
    //highlightCurrentLine();

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(braceMatch()));
    //connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));

    connect(this, SIGNAL(textChanged()), this, SIGNAL(keyPressed()));

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
    find(text, (QTextDocument::FindFlag) flags);
}

void Page::slotReplace(const QString &prev, const QString &current, int flags, bool all)
{
    QTextCursor tc = textCursor();
    if(!all)
    {
        if(tc.selectedText() != "")
            textCursor().insertText(current);
    }
    else
    {
        textCursor().movePosition(QTextCursor::Start);
        while(find(prev, (QTextDocument::FindFlag) flags))
            textCursor().insertText(current);
    }
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

void Page::wheelEvent(QWheelEvent *e)
{
    QPlainTextEdit::wheelEvent(e);
    m_codeTip->hide();
}

void Page::mousePressEvent(QMouseEvent *e)
{
    QPlainTextEdit::mousePressEvent(e);
    m_lastTextCursorPosition = textCursor().position();
}

QTextLine Page::currentTextLine(const QTextCursor &cursor)
{
    const QTextBlock block = cursor.block();
    if (!block.isValid())
        return QTextLine();

    const QTextLayout *layout = block.layout();
    if (!layout)
        return QTextLine();

    const int relativePos = cursor.position() - block.position();
    return layout->lineForTextPosition(relativePos);
}

QStringList Page::parseFunctionArgs(const QString &args)
{
    QString arg;
    QStringList list;
    int parenthesesDepth = 0;
    bool isString = false;
    for(int i = 0; i < args.size(); i++)
    {
        char c = args.at(i).toLatin1();

        if(c == '"')
            isString = !isString;
        else if(c == '(')
            parenthesesDepth++;
        else if(c == ')')
            parenthesesDepth--;

        if(c == ',' && !isString && parenthesesDepth == 0)
        {
            list.append(arg);
            arg.clear();
        }
        else
            arg.append(c);
    }
    list.append(arg);
    return list;
}

int Page::functionArg(int cursorPos, QStringList args)
{
    int i, count;
    if(args.count() == 0 || (args.count() == 1 && args[0] == ""))
        return 0;
    for(i = 0, count = 0; i < args.count(); i++)
    {
        count += args[i].size();
        if(cursorPos <= (count+1))
            return i;
    }
    return i-1;
}


bool Page::setFunctionTooltip(bool show)
{
    if(!show)
    {
        m_codeTip->hide();
        return false;
    }

    QTextCursor tc = textCursor();
    int curPos = tc.position();
    tc.movePosition(QTextCursor::StartOfLine);
    int linePos = tc.position();

    curPos -= linePos;

    QString line = lineUnderCursor(tc);

    QVector< QPair<int,int> > parenthesesPairs;

    QList< QPair<int,int> > validPairs;

    parenthesesPairs.resize(32);

    int depthLevel = 0;
    int countForward = 0;
    int countBackward = 0;

    for(int i = 0; i < line.size(); i++)
    {
        char c = line.at(i).toLatin1();
        if(c == '(')
        {
            parenthesesPairs[depthLevel].first = i;
            depthLevel++;
            countForward++;
        }
        else if(c == ')')
        {
            depthLevel--;
            parenthesesPairs[depthLevel].second = i;
            countBackward++;
            validPairs.append(parenthesesPairs[depthLevel]);
        }
    }

    int countPairs = qMin(countForward, countBackward);
    parenthesesPairs.resize(countPairs);

    for(int i = 0; i < validPairs.count(); i++)
    {
        int forwardPos = validPairs[i].first;
        int backwardPos = validPairs[i].second;

        if(curPos >= forwardPos && curPos <= backwardPos)
        {
            QString contents = line.mid(forwardPos + 1, backwardPos - forwardPos - 1);
            QStringList arguments = parseFunctionArgs(contents);

            QTextCursor lineCursor = textCursor();
            lineCursor.setPosition(linePos + forwardPos - 1);
            lineCursor.select(QTextCursor::WordUnderCursor);
            QString functionName = lineCursor.selectedText();

            QList<CodeParser::Element> functions = completer()->functions();
            int idx = CodeParser::hasElement(functionName, functions);
            if(idx != -1)
            {
                QFontMetrics fm(font());
                lineCursor.setPosition(linePos + forwardPos + 1);

                QPoint toolTipPos = viewport()->mapToGlobal(cursorRect(lineCursor).topRight());
                toolTipPos.setY(toolTipPos.y() - (font().pointSize()*2+6));

                QString prototype = functions[idx].prototype;
                QString temp = prototype.mid(prototype.indexOf('('));
                temp.remove(0, 1).chop(1);
                QStringList prototypeArgs = parseFunctionArgs(temp);


                QString codeTipText = prototype;
                int argNum = functionArg(curPos - forwardPos, arguments);

                if(argNum != -1 && argNum < prototypeArgs.count() && prototypeArgs[argNum] != "")
                    codeTipText.replace(prototypeArgs[argNum], "<b>" + prototypeArgs[argNum] + "</b>");

                m_codeTip->setFont(font());
                m_codeTip->setText(codeTipText);
                m_codeTip->move(toolTipPos.x(), toolTipPos.y());
                m_codeTip->show();
                return true;
            }
        }
    }
    m_codeTip->hide();
    return false;
}

void Page::keyPressEvent(QKeyEvent *e)
{
    if(isReadOnly() && (e->modifiers() == Qt::NoModifier))
    {
        qDebug() << "can't edit, read-only file!";
        emit info(tr("This is a read-only project.\n"
                     "To change it you need to re-save it to another location."));
        return;
    }

    if(m_completer->popup()->isVisible())
    {
//        setFunctionTooltip(false);
        switch(e->key())
        {
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

    bool bypassCompleter = false;

    bool requestCompleter = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space);

    if(!requestCompleter)
    {
        QPlainTextEdit::keyPressEvent(e);
    }

    onChar(e->text().at(0).toLatin1());

    //FIXME textChanged should be used instead
//    bool emitKeyPressed = !m_completer->popup()->isVisible() &&
//                           e->modifiers() == Qt::NoModifier &&
//                           (e->key() != Qt::Key_Left && e->key() && Qt::Key_Right && e->key() && Qt::Key_Up && e->key() != Qt::Key_Down);
//    if(emitKeyPressed)
//        emit keyPressed();


    if(e->key() == Qt::Key_Home)
    {
        QString selText;
        QTextCursor tc;

        bool select = e->modifiers() & Qt::ShiftModifier;

        tc = textCursor();
        if(!select) {\
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
    bool editShortcut = ((e->modifiers() & Qt::ControlModifier) &&
                        (e->key() == Qt::Key_C || e->key() == Qt::Key_V || e->key() == Qt::Key_X));
    if ((ctrlOrShift && e->text().isEmpty()) || editShortcut)
        return;

    //static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    static QString eow("~!@#$%^&*()+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor(textCursor());

//    qDebug() << "completionPrefix" << completionPrefix;

    QString input = e->text();
    input = input.remove(' ');
    input = input.remove('\t');
    input = input.remove(QChar(8));
    input = input.remove(QChar(13));
    input = input.remove(QChar(26));
    /*input = input.remove('(');
    input = input.remove(')');
    input = input.remove(';');*/
    if (!requestCompleter && (hasModifier || input.isEmpty() || completionPrefix.length() < 3
                   || eow.contains(input.right(1))))
    {
        m_completer->popup()->hide();
        return;
    }

    completionPrefix.remove('(');
    completionPrefix.remove(')');
    completionPrefix.remove(';');

//    if((e->modifiers() & Qt::ControlModifier) || e->key() == Qt::Key_Escape)
//    {
//        bypassCompleter = true;
//    }

    if (completionPrefix != m_completer->completionPrefix()) {
        m_completer->setCompletionPrefix(completionPrefix);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
                + m_completer->popup()->verticalScrollBar()->sizeHint().width());

    if(!bypassCompleter)
        m_completer->complete(cr); // popup it up!
}

QString Page::lineUnderCursor(const QTextCursor &cursor) const
{
    QTextCursor tc(cursor);
    tc.select(QTextCursor::LineUnderCursor);
    return tc.selectedText();
}

QString Page::textUnderCursor(const QTextCursor &cursor) const
{
    QTextCursor tc(cursor);
    QString text;
    tc.select(QTextCursor::WordUnderCursor);
    text = tc.selectedText();
    int moveBackwards = 0;
    if(text == ")" || text == ",")
        moveBackwards = 2;
    else if(text == ");")
        moveBackwards = 3;

    if(moveBackwards > 0)
    {
        tc.setPosition(tc.position() - moveBackwards, QTextCursor::MoveAnchor);
        tc.select(QTextCursor::WordUnderCursor);
        text = tc.selectedText();
    }

    return text;
}

void Page::focusInEvent(QFocusEvent *e)
{
    QPlainTextEdit::focusInEvent(e);
    m_completer->setWidget(this);
    emit focused();
}

void Page::focusOutEvent(QFocusEvent *e)
{
    QPlainTextEdit::focusInEvent(e);
    m_codeTip->hide();
}

void Page::insertCompletion(const QModelIndex &index)
{
    QString completion = index.data().toString();
    QTextCursor tc = textCursor();
    int extra = completion.length() - m_completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));

    QChar type = m_completer->completionModel()->data(index, Qt::UserRole + 1).toChar();
    qDebug() << "completion type:" << type;
    switch(type.toLatin1())
    {
    case 'f':
        tc.beginEditBlock();
        tc.insertText("()");
        tc.movePosition(QTextCursor::Left);
        tc.endEditBlock();
    default: ;
    }

    setTextCursor(tc);
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
    const QString bracesBegin = "{(";
    const QString bracesEnd = "})";

    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextDocument *doc = document();
    QTextCursor tc = textCursor();

    int braceBeginPos = tc.position();
    QChar braceBeginChar;
    QChar braceEndChar;

    int n = 2;
    while(n--)
    {
       braceBeginChar = doc->characterAt(braceBeginPos);
       if(bracesBegin.contains(braceBeginChar) || bracesEnd.contains(braceBeginChar))
           break;
       braceBeginPos--;
    }
    if(n < 0)
    {
        setExtraSelections(extraSelections);
        return;
    }

    QTextCharFormat braceMatchFormat;
    QFont f(font());
    f.setBold(true);
    braceMatchFormat.setBackground(QBrush(QColor("#FFFF33")));
    braceMatchFormat.setFont(f);

    QTextEdit::ExtraSelection braceBeginSelection;
    QTextEdit::ExtraSelection braceEndSelection;

    braceBeginSelection.cursor = textCursor();
    braceBeginSelection.cursor.setPosition(braceBeginPos);
    braceBeginSelection.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    braceBeginSelection.format = braceMatchFormat;

    bool forward;
    bracesBegin.contains(braceBeginChar) ? forward = true : forward = false;

    forward ? braceBeginPos++ : braceBeginPos--;

    switch(braceBeginChar.toLatin1())
    {
        case '(': braceEndChar = ')'; break;
        case ')': braceEndChar = '('; break;
        case '{': braceEndChar = '}'; break;
        case '}': braceEndChar = '{'; break;
        default: ;
    }

    int braceEndPos = braceBeginPos;
    int braceDepth = 1;
    QChar c;

    while (!(c = doc->characterAt(braceEndPos)).isNull())
    {
        if(c == braceBeginChar)
            braceDepth++;
        else if(c == braceEndChar)
        {
            braceDepth--;
            if(braceDepth == 0)
            {
                braceEndSelection.cursor = textCursor();
                braceEndSelection.cursor.setPosition(braceEndPos);
                braceEndSelection.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                braceEndSelection.format = braceMatchFormat;
                break;
            }
        }

        forward ? braceEndPos++ : braceEndPos--;
    }

    extraSelections.append(braceBeginSelection);
    extraSelections.append(braceEndSelection);
    setExtraSelections(extraSelections);
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

    if(c == '\"')
        insertPlainText("\"");
    else if(c == '(')
        insertPlainText(")");
    else if(c == '{' || c == '}')
        slotUpdateCodeBlocks();
    else if(c == ')' && textUnderCursor(textCursor()) == "())")
        textCursor().deleteChar();

    switch(c)
    {
    case '\"':
    case '(':
        tc.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
        setTextCursor(tc);
        break;
    default: ;
    }

}

void Page::slotUpdateCodeBlocks()
{
    QString in(text());
    QTextStream ts(&in);
    QList<int> stack;
    int lineNumber = 0;

    if(!m_codeBlocks.isEmpty())
        qDeleteAll(m_codeBlocks.begin(), m_codeBlocks.end());

    while(ts.atEnd())
    {
        QString line = ts.readLine();
        lineNumber++;
        if(line.contains("{"))
        {
            stack.append(lineNumber);
        }
        else if(line.contains("}"))
        {
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
    //qDebug() << "cursor pos" << textCursor().position();
    setFunctionTooltip(!m_completer->popup()->isVisible());
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
