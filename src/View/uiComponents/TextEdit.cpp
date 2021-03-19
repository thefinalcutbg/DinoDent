#include "TextEdit.h"

TextEdit::TextEdit(QWidget *parent)
	: QTextEdit(parent)
{
	connect(this, &QTextEdit::textChanged, [=] { stateChangedByUser(); });
}

TextEdit::~TextEdit()
{
}

void TextEdit::setFocusAndSelectAll()
{
	setFocus();
	selectAll();
}

void TextEdit::disable(bool disable)
{
	if (disable)
	{
		hide();
	}
	else
	{
		show();
	}
	
}

void TextEdit::setAppearence(bool valid)
{

	if (valid)
		setStyleSheet("");
	else
		setStyleSheet("border: 1px solid red;");
}

void TextEdit::setFieldText(const std::string& text)
{
	blockSignals(true);
	setText(QString::fromStdString(text));
	blockSignals(false);
}

std::string TextEdit::getText()
{
	return toPlainText().toStdString();
}

void TextEdit::keyPressEvent(QKeyEvent* event)
{
	{
		if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Tab)
		{
			if (event->modifiers() & Qt::ShiftModifier) return;

			event->ignore();
		}
		else
		{
			QTextEdit::keyPressEvent(event);
		}
	}
}
