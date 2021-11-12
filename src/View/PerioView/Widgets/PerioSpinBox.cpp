#include "PerioSpinBox.h"
#include <QPainter>
#include <QEvent>
#include "Model/Tooth/ToothUtils.h"
#include <QDebug>

PerioSpinBox::PerioSpinBox(QWidget*parent)
	: QSpinBox(parent)
{
	setMinimum(0);
	setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
	setValue(0);
	setAlignment(Qt::AlignCenter);
	setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
	setFrame(true);

	setStyleSheet(
		"font-weight: normal; "
		"color: black;"
		"selection-color: black;"
		"selection-background-color: white;"
	);

	connect(this, QOverload<int>::of(&QSpinBox::valueChanged),
		[=](int value){ colorCodeChange(); });

	this->installEventFilter(this);
	
}

void PerioSpinBox::disable(bool disabled)
{
	if(!disabled == isEnabled()) return;

	setDisabled(disabled);

	//not adjusting the range produces a bug when range is negative

	if (disabled)
	{
		QSignalBlocker b(this);
		m_min = minimum();
		m_max = maximum();
		setRange(0, 0); 
		setSpecialValueText(".");

	}
	else
	{
		setRange(m_min, m_max);
		setSpecialValueText("");
	}
}

void PerioSpinBox::setValueCustom(int value)
{
	QSignalBlocker b(this);
	setValue(value);
	colorCodeChange();

}

void PerioSpinBox::paintEvent(QPaintEvent* event)
{

	QSpinBox::paintEvent(event);

	if (!m_hover || !hasFocus() || !isEnabled() || isReadOnly()) return;

	QPainter painter(this);

	QRectF rect(0, 0, width(), height());

	QPen pen; pen.setColor(QColor(0, 122, 204));
	pen.setWidth(2);
	painter.setPen(pen);
	painter.drawRect(rect);

}

bool PerioSpinBox::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::HoverEnter) {
		m_hover = true;
		update();
	}

	if (e->type() == QEvent::HoverLeave) {
		m_hover = false;
		update();
	}

	return false;
}

void PerioSpinBox::colorCodeChange()
{
	auto value = this->value();

	if (value >= redValue && !isRed)
	{
		setStyleSheet(
			"font-weight: bold; "
			"color: darkred;"
			"selection-color: darkred;"
			"selection-background-color: white;"
		);

		isRed = true;
	}
	else if (value < redValue && isRed)
	{
		isRed = false;

		setStyleSheet(
			"font-weight: normal; "
			"color: black;"
			"selection-color: black;"
			"selection-background-color: white;"
		);
	}
}


PerioSpinBox::~PerioSpinBox()
{
}
