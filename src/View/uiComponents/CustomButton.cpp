#include "CustomButton.h"
#include <QPainter>
#include <QIcon>
#include <QEvent>
#include <QPainterPath>
#include "View/Theme.h"
#include <QApplication>

CustomButton::CustomButton(QWidget* parent)
	:
	QPushButton(parent),
	hoverColor(Theme::background),
	backgroundColor(Theme::sectionBackground)
{
	m_font.setBold(1);

	this->installEventFilter(this);

#ifdef Q_OS_MAC
    setStyle(Theme::fusionStyle());
#endif

}

void CustomButton::paintEvent(QPaintEvent*)
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

bool CustomButton::eventFilter(QObject*, QEvent* e)
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


void CustomButton::setHoverColor(const QColor& color)
{
	hoverColor = color;
	update();
}

void CustomButton::setBackgroundColor(const QColor& color)
{
	backgroundColor = color;
	update();
}

CustomButton::~CustomButton()
{
}
