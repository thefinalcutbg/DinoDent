#include "ShortcutTile.h"

#include <QEvent>
#include <QApplication>
#include <QPainter>

#include "View/Theme.h"


ShortcutTile::ShortcutTile(QWidget* parent) : QPushButton(parent)
{

	this->installEventFilter(this);

	auto font = this->font();
	font.setBold(true);
	setFont(font);
}


void ShortcutTile::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);

	QPainterPath path;
	path.addRoundedRect(rect(), Theme::radius, Theme::radius);

	QColor background = hover ?
		QColor(Theme::inactiveTabBG)
		:
		QColor(Theme::sectionBackground);

	painter.fillPath(path, background);

	QPen pen(Theme::border);
	pen.setCosmetic(true);
	pen.setWidth(2);
	painter.setPen(pen);


	int iconRectSide = clicked ? 35 : 40;

	if (!icon().isNull()) {
		icon().paint(&painter, QRect(width()/2-iconRectSide/2, height()/2-iconRectSide/2-10, iconRectSide, iconRectSide));
	}


	QColor textColor = hover ?
		QColor(Theme::fontTurquoise)
		:
		QColor(Theme::fontTurquoiseClicked)

		;

	painter.setPen(QPen(textColor));
	painter.setFont(font());

	QTextOption textOption;
	textOption.setWrapMode(QTextOption::WordWrap);
	textOption.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	painter.drawText(QRectF(10, 100, width() - 20, 50), text(), textOption);
}

bool ShortcutTile::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::HoverEnter) {
		hover = true;
		QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
		update();
	}

	if (e->type() == QEvent::HoverLeave) {
		QApplication::restoreOverrideCursor();
		hover = false;
		update();
	}

	if (e->type() == QEvent::MouseButtonPress)
	{
		clicked = true;
	}

	if (e->type() == QEvent::MouseButtonRelease)
	{
		clicked = false;
	}

	return false;
}
