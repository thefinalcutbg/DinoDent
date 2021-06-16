#include "LineEdit.h"

LineEdit::LineEdit(QWidget* parent)
	: QLineEdit(parent),
	defaultWidth(width()),
	defaultWidthSet(0),
	disabled(0)
{
	connect(this, &QLineEdit::textEdited, [=] { stateChangedByUser(); });
	connect(this, &QLineEdit::textChanged, [=] { dynamicWidthChange(); });
	connect(this, &QLineEdit::editingFinished, [=] { reformat(); });
}

LineEdit::~LineEdit(){}

void LineEdit::setFocusAndSelectAll()
{
	setFocus();
	selectAll();
}


void LineEdit::setAppearence(bool valid)
{
	if(valid)
		setStyleSheet("");
	else
		setStyleSheet("border: 1px solid red;");
}

void LineEdit::setFieldText(const std::string& text)
{
	QSignalBlocker b(this);
	setText(QString::fromStdString(text));


}

std::string LineEdit::getText()
{
	return text().toStdString();
}

void LineEdit::disable(bool disable)
{
	if (disable) disabled = true;
	else disabled = false;
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

void LineEdit::stateChangedByUser()
{
	forceValidate();
}

void LineEdit::dynamicWidthChange()
{
	if (!defaultWidthSet)
	{
		defaultWidth = width(); defaultWidthSet = true;
	};

	QFontMetrics fm(font());
	int pixelsWide = fm.width(text());
	if (pixelsWide > defaultWidth - 6)
		setFixedWidth(pixelsWide + 9);
	else setFixedWidth(defaultWidth);
}



