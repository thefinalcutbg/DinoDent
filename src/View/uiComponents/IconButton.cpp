#include "IconButton.h"
#include <QPainter>
#include <QIcon>
#include <QEvent>
#include <QPainterPath>
#include "View/Theme.h"
#include <QApplication>
IconButton::IconButton(QWidget *parent)
	: 
	QPushButton(parent), 
	hoverColor(Theme::background), 
	backgroundColor(Theme::sectionBackground)
{
	this->installEventFilter(this);
}

void IconButton::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

	QPainterPath path;
	path.addEllipse(rect());

	QColor color = (m_hover || isChecked()) ?
			hoverColor : backgroundColor ;

	painter.fillPath(path, color);
	
	int padding = m_clicked && m_hover ? 15 : 12;

	QRect iconRect(rect().center(), QSize(rect().width()-padding, rect().height()-padding));
	iconRect.moveCenter(rect().center());

	if (!icon().isNull())
		icon().paint(&painter, iconRect);
	
}

bool IconButton::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::HoverEnter) {
		m_hover = true;
		QApplication::setOverrideCursor(Qt::PointingHandCursor);
		update();
	}

	if (e->type() == QEvent::HoverLeave) {
		m_hover = false;
		QApplication::restoreOverrideCursor();
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


void IconButton::setHoverColor(const QColor& color)
{
	hoverColor = color;
	update();
}

void IconButton::setBackgroundColor(const QColor& color)
{
	backgroundColor = color;
	update();
}

IconButton::~IconButton()
{
}
