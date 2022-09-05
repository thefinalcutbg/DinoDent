#include "LineEdit.h"
#include <QKeyEvent>
#include <QLabel>
LineEdit::LineEdit(QWidget* parent)
	: QLineEdit(parent),
	disabled(0),
	errorLabel(nullptr)
{
	connect(this, &QLineEdit::textEdited, [=] { AbstractLineEdit::validateInput(); });
	connect(this, &QLineEdit::textChanged, [=] { dynamicWidthChange(); });
	connect(this, &QLineEdit::editingFinished, [=] { reformat(); });
}

LineEdit::~LineEdit(){}

void LineEdit::setErrorLabel(QLabel* errorLabel)
{
	this->errorLabel = errorLabel;
}

void LineEdit::setFocus()
{
	QLineEdit::setFocus();
	selectAll();
}


void LineEdit::setValidAppearence(bool valid)
{
	if (valid)
	{
		setStyleSheet("");

		if (errorLabel) {

			errorLabel->setText("");
		}

		return;
	}

	setStyleSheet("border: 1px solid red;");

	if (AbstractUIElement::validator != nullptr && errorLabel != nullptr)
			errorLabel->setText(QString::fromStdString(AbstractUIElement::validator->getErrorMessage()));

}

void LineEdit::set_Text(const std::string& text)
{
	QSignalBlocker b(this);
    QLineEdit::setText(QString::fromStdString(text));
}

std::string LineEdit::getText()
{
	return text().toStdString();
}

void LineEdit::disable(bool disable)
{
	if (disable)
	{
		disabled = true;
		setContextMenuPolicy(Qt::NoContextMenu);
	}
	else
	{
		disabled = false;
		setContextMenuPolicy(Qt::DefaultContextMenu);
	};
	
}

void LineEdit::keyPressEvent(QKeyEvent* event)
{
	if (disabled) {
		if ( event->modifiers() & Qt::ControlModifier
			&& (event->key() == Qt::Key_C || event->key() == Qt::Key_A)
																		)
		{
			QLineEdit::keyPressEvent(event);
		}
		return;
	}
	QLineEdit::keyPressEvent(event);
}


void LineEdit::dynamicWidthChange()
{
	if (sizePolicy().horizontalPolicy() == QSizePolicy::Policy::Expanding) return;

	if(!initialWidth) initialWidth = width();

	int pixelsWide = QFontMetrics(font()).boundingRect(text()).width();

	if (pixelsWide > initialWidth - 6)
		setFixedWidth(pixelsWide + 9);
	else setFixedWidth(initialWidth);
}



