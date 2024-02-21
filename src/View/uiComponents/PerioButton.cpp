#include "PerioButton.h"
#include <QEvent>
#include <QPainter>
#include "View/Theme.h"

bool PerioButton::eventFilter(QObject*, QEvent* e)
{

    if (e->type() == QEvent::Enter) {
		m_hover = true;
		update();
	}

    if (e->type() == QEvent::Leave) {
		m_hover = false;
		update();
	}
	
	return false;
}

PerioButton::PerioButton(QWidget *parent)
	: QAbstractButton(parent)
{
	this->installEventFilter(this);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

PerioButton::~PerioButton()
{
}


void PerioButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	QColor color(Qt::GlobalColor::white);
	if (!isEnabled()) color = Theme::background;
	else if (m_hover) color = QColor{ 236, 236, 236 };

	
	QRect rect = this->rect();
	
	painter.fillRect(rect, color);

	QPen pen; 
	pen.setColor(Qt::gray);
	pen.setWidth(1);
	pen.setCosmetic(true);
	painter.setPen(pen);
	painter.drawRect(rect);

	if (isEnabled() && isChecked())
	{
		if(!icon().isNull())
			icon().paint(&painter, rect);
		

		painter.setPen(palette().color(QPalette::ColorRole::Text));
		auto font = painter.font();
		
		painter.drawText(rect, Qt::AlignCenter, text());
		
	}

	if (hasFocus())
	{
		QRectF rect(0, 0, width(), height());

		QPen pen; 
		pen.setColor(QColor(0, 122, 204));
		pen.setCosmetic(true);
		pen.setWidth(2);
		painter.setPen(pen);
		painter.drawRect(rect);
	}
	

}

