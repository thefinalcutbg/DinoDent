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
	
	setIconSize(QSize(18, 18));

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
	path.addRoundedRect(QRectF(rect()), Theme::radius-3, Theme::radius-3);

	QColor color{ m_hover || isChecked() ? hoverColor : Theme::sectionBackground };

	painter.fillPath(path, color);

	int iconSize = 18;

	QRect iconRect(10, (height() - iconSize) / 2, iconSize, iconSize);

	if (!icon().isNull())
		icon().paint(&painter, iconRect);

	painter.setFont(m_font);
	painter.setPen(QPen(Theme::fontTurquoiseClicked));

	int xPos = 10;
	xPos =+ icon().isNull() ? 0 : 28;

	QRect textRect(xPos, 0, width()-xPos, height());
	painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text());


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

QSize UserButton::minimumSizeHint() const
{
	if (icon().isNull()) return QToolButton::minimumSizeHint();

	auto size = QToolButton::minimumSizeHint();

	size.setWidth(size.width() + 8);

	return size;
}



UserButton::~UserButton()
{}
