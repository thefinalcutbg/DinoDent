#include "PerioButton.h"
#include <QPainter>

PerioButton::PerioButton(QWidget *parent)
	: QAbstractButton(parent)
{
	
	setMinimumSize(1, 1);
	setMaximumSize(200, 200);
	
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


PerioButton::~PerioButton()
{
}


void PerioButton::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	QColor color;
	if (!isEnabled()) color = QColor(Qt::GlobalColor::lightGray);
	else color = QColor(Qt::GlobalColor::white);
	

	QRectF rect(0, 0, width(), height());

	painter.fillRect(rect, color);

	QPen pen; pen.setColor(QColor(Qt::GlobalColor::gray));
	painter.setPen(pen);
	painter.drawRect(rect);

	if (isEnabled() && isChecked())
	{
		if(!icon().isNull())
			icon().paint(&painter, rect.toRect());

		int font_px_height = fontMetrics().height();
		int font_px_width = fontMetrics().width(m_text);

		int x = (width() - font_px_width) / 2;
		int y = font_px_height;
		painter.setPen(palette().color(QPalette::ColorRole::Text));
		painter.drawText(x, y, m_text);
	}
	

}

