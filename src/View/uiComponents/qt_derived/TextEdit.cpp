#include "TextEdit.h"

TextEdit::TextEdit(QWidget *parent)
	: QTextEdit(parent)
{
	connect(this, &QTextEdit::textChanged, [=] { AbstractUIElement::validateInput(); });
}

TextEdit::~TextEdit()
{
}

void TextEdit::setFocus()
{
	QWidget::setFocus();
	selectAll();
}

void TextEdit::disable(bool disable)
{
	if (disable)
	{
		disabled = true;
		setContextMenuPolicy(Qt::NoContextMenu);
	}
	else
	{
		setContextMenuPolicy(Qt::DefaultContextMenu);
		disabled = false;
	}
		
}

void TextEdit::setValidAppearence(bool valid)
{

	if (valid)
		setStyleSheet("");
	else
		setStyleSheet("border: 1px solid red;");
}

void TextEdit::set_Text(const std::string& text)
{
	QSignalBlocker blocker(this);
	QTextEdit::setText(QString::fromStdString(text));

}

std::string TextEdit::getText()
{
	return toPlainText().toStdString();
}

void TextEdit::keyPressEvent(QKeyEvent* event)
{
	if (disabled) 
	{
		if (event->modifiers() & Qt::ControlModifier
			&& (event->key() == Qt::Key_C || event->key() == Qt::Key_A)
			)
		{
			QTextEdit::keyPressEvent(event);
		}
	}
	else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Tab)
	{
		if (event->modifiers() & Qt::ShiftModifier) return;
	}
	else
	{
			QTextEdit::keyPressEvent(event);
	}

}
