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
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
	setFrame(true);

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

	//QSpinBox::paintEvent(event);

	QPen pen(Qt::lightGray);
	pen.setWidth(1);
	pen.setCosmetic(true);

	QBrush background = this->isEnabled() ? QBrush(Qt::white) : QBrush(Qt::lightGray);

	QRectF rect(0, 0, width(), height());

	QPainter painter(this);

	painter.fillRect(rect, background);

	painter.setPen(pen);
	painter.drawRect(rect);
	if (!m_hover || !hasFocus() || !isEnabled() || isReadOnly()) {
		return;
	}


	

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
			"background-color: white;"
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
			"background-color: white;"
		);
	}
}


PerioSpinBox::~PerioSpinBox()
{
}
