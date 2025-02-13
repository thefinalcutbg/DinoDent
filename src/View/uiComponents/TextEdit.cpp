#include "TextEdit.h"
#include <QKeyEvent>
#include <QLabel>
TextEdit::TextEdit(QWidget *parent)
	: QPlainTextEdit(parent)
{
    connect(this, &QPlainTextEdit::textChanged, this, [&] { AbstractUIElement::validateInput(); });
	setTabChangesFocus(true);
}

TextEdit::~TextEdit()
{
}

void TextEdit::setErrorLabel(QLabel* errorLabel)
{
	this->errorLabel = errorLabel;
}

void TextEdit::set_focus()
{
	QPlainTextEdit::setFocus();
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
		setStyleSheet("");	else
		setStyleSheet("border: 1px solid red;");

	if (errorLabel == nullptr || AbstractUIElement::validator == nullptr) return;

	if (valid)
		errorLabel->setText("");
	else
	{
		if (AbstractUIElement::validator)
			errorLabel->setText(QString::fromStdString(AbstractUIElement::validator->getErrorMessage()));
	}

}

void TextEdit::set_Text(const std::string& text)
{
	QSignalBlocker blocker(this);
	QPlainTextEdit::setPlainText(QString::fromStdString(text));

}

std::string TextEdit::getText()
{
	return toPlainText().toStdString();
}

void TextEdit::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Escape) event->ignore();

	if (disabled) 
	{
		if (event->modifiers() & Qt::ControlModifier
			&& (event->key() == Qt::Key_C || event->key() == Qt::Key_A)
			)
		{
			QPlainTextEdit::keyPressEvent(event);
		}
	}
	else if (event->key() == Qt::Key_Return)
	{
		if (event->modifiers() & Qt::ShiftModifier) return;
	}
	else
	{
		QPlainTextEdit::keyPressEvent(event);
	}
}
