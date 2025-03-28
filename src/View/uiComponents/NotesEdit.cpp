#include "NotesEdit.h"
#include "Model/Date.h"
#include <QKeyEvent>

NotesEdit::NotesEdit(QWidget* parent) : QTextEdit(parent)
{
}

QString NotesEdit::getCurrentDateStr()
{
    return Date::currentDate().toBgStandard().c_str();
}

void NotesEdit::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) 
    {
        event->ignore();
        return;
    }

    if (event->key() == Qt::Key_Return) {

        if (event->modifiers() == Qt::ShiftModifier) {
            event->setModifiers(Qt::KeyboardModifier::NoModifier);
        }
        else {
            event->ignore();
            return;
        }

    }

    QTextEdit::keyPressEvent(event);

}

void NotesEdit::setText(const std::string& text)
{
    QString qstr = text.c_str();
    
    auto currentDate = getCurrentDateStr();

    if (!qstr.contains(currentDate)) {

        if (text.size()) {
            qstr += "\n\n";
        }

        qstr += getCurrentDateStr();
        qstr += " - ";
    }

    setPlainText(qstr);

    moveCursor(QTextCursor::MoveOperation::End);
}

std::string NotesEdit::getText()
{
    auto text = toPlainText();

    while (text.endsWith('\n') || text.endsWith(' ')) {
        text.removeLast();
    }

    auto dateStr = getCurrentDateStr() + " -";

    if (text.endsWith(dateStr)) {
        text = text.left(text.size() - dateStr.size());
    }

    while (text.endsWith('\n') || text.endsWith(' ')) {
        text.removeLast();
    }

    return text.toStdString();
}
