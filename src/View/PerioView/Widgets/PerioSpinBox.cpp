#include "PerioSpinBox.h"
#include <QPainter>



PerioSpinBox::PerioSpinBox(QWidget*parent)
	: QSpinBox(parent)
{
	setMinimum(0);
	setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);

	setAlignment(Qt::AlignCenter);
	setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
	setFrame(true);
	
	QPalette palet = palette();
	palet.setBrush(QPalette::Highlight, QBrush(Qt::white));
	palet.setBrush(QPalette::HighlightedText, QBrush(Qt::black));
	setPalette(palet);

	connect(this, QOverload<int>::of(&QSpinBox::valueChanged),
		[&](int value)
		{
			if (value >= redValue && !isRed)
			{
				isRed = true;
				QColor red; red.setRgb(218, 69, 63);
				QPalette palet = palette();
				palet.setBrush(QPalette::HighlightedText, QBrush(red));
				palet.setBrush(QPalette::Text, QBrush(red));
				setPalette(palet);

				auto f = font();
				f.setBold(true);
				setFont(f);

			}
			else if (value < redValue && isRed)
			{
				isRed = false;
				QPalette palet = palette();
				palet.setBrush(QPalette::HighlightedText, QBrush(Qt::black));
				palet.setBrush(QPalette::Text, QBrush(Qt::black));
				setPalette(palet);

				auto f = font();
				f.setBold(false);
				setFont(f);
			}

		});

	
}

void PerioSpinBox::disable(bool disabled)
{
	QSignalBlocker b(this);

	setDisabled(disabled);
	if (disabled)
	{
		m_min = minimum();
		m_max = maximum();
		m_value = value();
		setRange(0, 0); //not adjusting the range produces a bug

		setSpecialValueText(".");
		emit valueChanged(0);
	}
	else
	{

		setSpecialValueText("");
		setRange(m_min, m_max);
		setValue(m_value);
		emit valueChanged(m_value);
	}
}

void PerioSpinBox::paintEvent(QPaintEvent* event)
{

	QSpinBox::paintEvent(event);

}


PerioSpinBox::~PerioSpinBox()
{
}
