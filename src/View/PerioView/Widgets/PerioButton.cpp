#include "PerioButton.h"
#include <QEvent>
#include <QPainter>

bool PerioButton::eventFilter(QObject* obj, QEvent* e)
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

PerioButton::PerioButton(QWidget *parent)
	: QAbstractButton(parent)
{
	
	//setMinimumSize(1, 1);
	//setMaximumSize(200, 200);
	this->installEventFilter(this);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	
}


PerioButton::~PerioButton()
{
}
#include <QDebug>

void PerioButton::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	QColor color(Qt::GlobalColor::white);
	if (!isEnabled()) color = QColor(Qt::GlobalColor::lightGray);
	else if (m_hover) color = QColor{ 236, 236, 236 };

	
	QRectF rect(0, 0, width(), height());

	painter.fillRect(rect, color);

	QPen pen; pen.setColor(QColor(Qt::GlobalColor::gray));
	painter.setPen(pen);
	painter.drawRect(rect);



	if (isEnabled() && isChecked())
	{
		if(!icon().isNull())
			icon().paint(&painter, rect.toRect());
		
		auto fontRect = fontMetrics().boundingRect(text());

		int x = (width() - fontRect.width()) / 2;
		int y = (height() + fontRect.height()) / 2;

		painter.setPen(palette().color(QPalette::ColorRole::Text));
		painter.drawText(x, y, text());
		
	}

	if (hasFocus())
	{
		QRectF rect(0, 0, width(), height());

		QPen pen; pen.setColor(QColor(0, 122, 204));
		pen.setWidth(2);
		painter.setPen(pen);
		painter.drawRect(rect);
	}
	

}

