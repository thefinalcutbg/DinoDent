#include "BigPushButton.h"
#include <QPainter>
#include <QIcon>
#include <QEvent>
#include <QPainterPath>
#include "View/Theme.h"
#include <QApplication>
#include <QMenu>

BigPushButton::BigPushButton(QWidget* parent) :
	QPushButton(parent),
	normalColor(Theme::sectionBackground),
	hoverColor(Theme::background)
{
	this->installEventFilter(this);

	auto font = this->font();

	font.setBold(true);

	setFont(font);

#ifdef Q_OS_MAC
    setStyle(Theme::fusionStyle());
#endif

}

void BigPushButton::setHoverColor(const QColor& color)
{
	this->hoverColor = color;
	update();
}

void BigPushButton::setNormalColor(const QColor& color)
{
	normalColor = color;
	update();
}

void BigPushButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

	QPainterPath path;
	path.addRoundedRect(QRectF(rect()), Theme::radius, Theme::radius);

	QColor color{ m_hover || isChecked() ? hoverColor : normalColor };

	painter.fillPath(path, color);

	int padding = 15;

	int refSize = std::min(height(), width());

	QRect iconRect(10, padding, refSize - (padding * 2), refSize - (padding * 2));

	if (!icon().isNull())
		icon().paint(&painter, iconRect);

	bool differentFont = hoverColor == normalColor && m_hover;

	painter.setFont(font());
	painter.setPen(QPen(differentFont ? Theme::fontTurquoise : Theme::fontTurquoiseClicked));

	QRect textRect;
	textRect.setCoords(iconRect.width(), 0, width(), height());

	painter.drawText(textRect, Qt::AlignCenter, text());


}

bool BigPushButton::eventFilter(QObject*, QEvent* e)
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



BigPushButton::~BigPushButton()
{}
