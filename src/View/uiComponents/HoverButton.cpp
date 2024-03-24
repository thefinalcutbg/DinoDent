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

#ifdef Q_OS_MAC
    setStyle(Theme::fusionStyle());
#endif

}


void HoverButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	QPainterPath outline;
	outline.addRoundedRect(1, 1, rect().width()-2, rect().height()-2, 10, 10);
	painter.fillPath(outline, m_hover ? Theme::background : Theme::sectionBackground);

	QString paintText = m_hover && m_hoverText.size() ? m_hoverText : text();

	QPen pen;
	pen.setColor(Theme::buttonFrame);
	pen.setCosmetic(true);
	painter.setPen(pen);
	painter.drawPath(outline);

	pen.setColor(Theme::fontTurquoise);
	painter.setPen(pen);
    painter.setFont(QFont());
	painter.drawText(rect(), Qt::AlignCenter, paintText);

	painter.end();

}

bool HoverButton::eventFilter(QObject*, QEvent* e)
{
    if (e->type() == QEvent::Enter) {
		m_hover = true;
		QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
		update();
	}

    if (e->type() == QEvent::Leave) {
		QApplication::restoreOverrideCursor();
		m_hover = false;
		update();
	}

	return false;
}


HoverButton::~HoverButton()
{}
