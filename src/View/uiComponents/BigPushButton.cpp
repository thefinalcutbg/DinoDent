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
	path.addRoundedRect(QRectF(rect()), Theme::radius-3, Theme::radius-3);

	QColor color{ m_hover || isChecked() ? hoverColor : normalColor };

	painter.fillPath(path, color);

	int iconSize = 18;

	QRect iconRect(10, (height()-iconSize)/2, iconSize, iconSize);

	if (!icon().isNull())
		icon().paint(&painter, iconRect);

	bool differentFont = hoverColor == normalColor && m_hover;

	painter.setFont(font());
	painter.setPen(QPen(differentFont ? Theme::fontTurquoise : Theme::fontTurquoiseClicked));

	int xPos =icon().isNull() ? 0 : 18;

	QRect textRect;
	textRect.setRect(xPos, 0, width()-xPos, height());

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

QSize BigPushButton::minimumSizeHint() const
{
	if (icon().isNull()) return QPushButton::minimumSizeHint();

	auto size = QPushButton::minimumSizeHint();

	size.setWidth(size.width() + 18);

	return size;
}



BigPushButton::~BigPushButton()
{}
