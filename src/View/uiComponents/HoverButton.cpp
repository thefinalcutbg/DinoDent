#include "HoverButton.h"
#include <QPainter>
#include <QPainterPath>
#include <QEvent>
#include <QApplication>
#include "View/Theme.h"

HoverButton::HoverButton(QWidget *parent)
	: QPushButton(parent)
{
	installEventFilter(this);
}


void HoverButton::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	QPainterPath outline;
	outline.addRoundedRect(1, 1, rect().width()-2, rect().height()-2, 7, 7);
	painter.fillPath(outline, m_hover ? Theme::background : Theme::sectionBackground);

	QString paintText = m_hover ? m_hoverText : text();

	QFont font("Segoe UI");
	font.setPointSizeF(9);

	QPen pen;
	pen.setColor(Theme::buttonFrame);
	painter.setPen(pen);
	painter.drawPath(outline);

	pen.setColor(Theme::fontTurquoise);
	painter.setPen(pen);
	painter.setFont(font);
	painter.drawText(rect(), Qt::AlignCenter, paintText);

	painter.end();

}

bool HoverButton::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::HoverEnter) {
		m_hover = true;
		QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
		update();
	}

	if (e->type() == QEvent::HoverLeave) {
		QApplication::restoreOverrideCursor();
		m_hover = false;
		update();
	}

	return false;
}


HoverButton::~HoverButton()
{}
