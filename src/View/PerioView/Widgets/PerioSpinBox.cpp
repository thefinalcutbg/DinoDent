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

	setMinimumSize(QSize(1, 1));

	QPalette palet = palette();
	palet.setBrush(QPalette::Highlight, QBrush(Qt::white));
	palet.setBrush(QPalette::HighlightedText, QBrush(Qt::black));
	setPalette(palet);

	connect(this, QOverload<int>::of(&QSpinBox::valueChanged),
		[=](int value)
		{
		
			if (value >= redValue && !isRed)
			{
				isRed = true;
				QColor red; red.setRgb(218, 69, 63);
				QPalette palet = palette();
				palet.setBrush(QPalette::HighlightedText, QBrush(QColor(Qt::red)));
				palet.setBrush(QPalette::Text, QBrush(QColor(Qt::red)));
				setPalette(palet);

				auto f = this->font();
				f.setBold(true);
				setFont(f);

			}
			else if (value < redValue && isRed)
			{
				isRed = false;
				QPalette palet = palette();
				palet.setBrush(QPalette::HighlightedText, QBrush(QColor(Qt::black)));
				palet.setBrush(QPalette::Text, QBrush(QColor(Qt::black)));
				setPalette(palet);

				auto f = this->font();
				f.setBold(false);
				setFont(f);
			}

		});

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

void PerioSpinBox::paintEvent(QPaintEvent* event)
{

	QSpinBox::paintEvent(event);

	if (!m_hover || !isEnabled() || isReadOnly()) return;

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


PerioSpinBox::~PerioSpinBox()
{
}
