#include "UserButton.h"
#include <QPainter>
#include <QIcon>
#include <QEvent>
#include <QPainterPath>
#include "View/Theme.h"
#include <QApplication>
#include <QMenu>

UserButton::UserButton(QWidget* parent) : QToolButton(parent)
{
	this->installEventFilter(this);

    m_font.setBold(1);

	hoverColor = Theme::background;
}

void UserButton::setHoverColor(const QColor& color)
{
	this->hoverColor = color;
	update();
}

void UserButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

	QPainterPath path;
	path.addRoundedRect(QRectF(rect()), Theme::radius, Theme::radius);

	QColor color{ m_hover || isChecked() ? hoverColor : Theme::sectionBackground };

	painter.fillPath(path, color);

	int padding = 15;

	int refSize = std::min(height(), width());

	QRect iconRect(10, padding, refSize - (padding * 2), refSize - (padding * 2));

	if (!icon().isNull())
		icon().paint(&painter, iconRect);

	painter.setFont(m_font);
	painter.setPen(QPen(Theme::fontTurquoiseClicked));

	QRect textRect;
	textRect.setCoords(iconRect.width(), 0, width(), height());

	painter.drawText(textRect, Qt::AlignCenter, text());


}

bool UserButton::eventFilter(QObject*, QEvent* e)
{
    if (e->type() == QEvent::Enter) {
		m_hover = true;
		QApplication::setOverrideCursor(Qt::PointingHandCursor);
		update();
	}

    if (e->type() == QEvent::Leave) {
		QApplication::restoreOverrideCursor();
		m_hover = false;
		update();
	}

	if (e->type() == QEvent::MouseButtonPress)
	{
		m_hover = false;
		QApplication::restoreOverrideCursor();
		update();
	}


	return false;
}



UserButton::~UserButton()
{}
