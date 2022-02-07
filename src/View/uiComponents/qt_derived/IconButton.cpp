#include "IconButton.h"
#include <QPainter>
#include <QIcon>
#include <QEvent>
#include <QPainterPath>
IconButton::IconButton(QWidget *parent)
	: QPushButton(parent)
{
	this->installEventFilter(this);
}

void IconButton::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

	if (m_hover)
	{
		QPainterPath path;
		path.addRoundedRect(rect(), 4, 4);

		QColor color = m_clicked ?
			QColor(210, 210 ,210)
			:
			QColor{ 235, 239, 239 };
			
		painter.fillPath(path, color);	
	}

	auto iconRect = rect();
	

	if (!icon().isNull())
		icon().paint(&painter, 12, 12, 26, 26);
	/*
	painter.setPen(QPen(Qt::black));

	QFontMetrics m(font());
	auto text_w = m.boundingRect(text()).width();
	auto text_h = m.boundingRect(text()).height();

	painter.drawText()
	*/


}

bool IconButton::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::HoverEnter) {
		m_hover = true;
		update();
	}

	if (e->type() == QEvent::HoverLeave) {
		m_hover = false;
		update();
	}

	if (e->type() == QEvent::MouseButtonPress)
	{
		m_clicked = true;
	}

	if (e->type() == QEvent::MouseButtonRelease)
	{
		m_clicked = false;
	}

	return false;
}


IconButton::~IconButton()
{
}
