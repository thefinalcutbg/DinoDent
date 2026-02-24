#include "TextEdit.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QKeyEvent>
#include <QLabel>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QStringListModel>
#include <QTextCursor>

TextEdit::TextEdit(QWidget* parent)
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

void TextEdit::setCompletions(const std::vector<std::string>& completions)
{
	QStringList list;
	list.reserve(static_cast<qsizetype>(completions.size()));

	for (const auto& s : completions)
		list << QString::fromStdString(s);

	if (!completer)
	{
		completer = new QCompleter(this);
		completer->setWidget(this);
		completer->setCaseSensitivity(Qt::CaseInsensitive);
		completer->setCompletionMode(QCompleter::PopupCompletion);
		completer->setFilterMode(Qt::MatchContains);

		connect(completer, QOverload<const QString&>::of(&QCompleter::activated),
			this, &TextEdit::insertCompletion);
	}

	if (completer->model() && completer->model()->parent() == completer)
		delete completer->model();

	completer->setModel(new QStringListModel(list, completer));
}

QString TextEdit::textUnderCursor() const
{
	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);
	return tc.selectedText();
}

void TextEdit::insertCompletion(const QString& completion)
{
	if (!completer || completer->widget() != this)
		return;

	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);
	tc.removeSelectedText();
	tc.insertText(completion);
	setTextCursor(tc);
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
		setStyleSheet("");
	else
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
	if (completer && completer->popup() && completer->popup()->isVisible())
	{
		switch (event->key())
		{
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Escape:
		case Qt::Key_Tab:
		case Qt::Key_Backtab:
			event->ignore();
			return;
		default:
			break;
		}
	}

	if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Escape)
		event->ignore();

	if (disabled)
	{
		if ((event->modifiers() & Qt::ControlModifier) &&
			(event->key() == Qt::Key_C || event->key() == Qt::Key_A))
		{
			QPlainTextEdit::keyPressEvent(event);
		}
		// no editing when disabled
		return;
	}
	else if (event->key() == Qt::Key_Return)
	{
		if (event->modifiers() & Qt::ShiftModifier)
			return;

		return;
	}
	else
	{
		QPlainTextEdit::keyPressEvent(event);
	}

	// Autocomplete popup handling
	if (!completer)
		return;

	const bool hasTextInput = !event->text().isEmpty();
	const int key = event->key();

	const bool isEditingKey =
		hasTextInput ||
		key == Qt::Key_Backspace ||
		key == Qt::Key_Delete;

	if (!isEditingKey)
		return;

	if ((event->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) && !hasTextInput)
		return;

	const QString prefix = textUnderCursor();

	if (prefix.isEmpty())
	{
		completer->popup()->hide();
		return;
	}

	if (completer->completionPrefix() != prefix)
	{
		completer->setCompletionPrefix(prefix);
		completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
	}

	QRect cr = cursorRect();
	cr.setWidth(completer->popup()->sizeHintForColumn(0)
		+ completer->popup()->verticalScrollBar()->sizeHint().width());

	completer->complete(cr);
}