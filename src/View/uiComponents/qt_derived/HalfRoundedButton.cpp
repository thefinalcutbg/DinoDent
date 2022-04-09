#include "HalfRoundedButton.h"
#include <QEvent>
#include <QPainter>
#include <QApplication>

#include "View/Theme.h"

bool HalfRoundedButton::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::HoverEnter) {
		QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
		m_hover = true;
		update();
	}

	if (e->type() == QEvent::HoverLeave) {
		QApplication::restoreOverrideCursor();
		m_hover = false;
		update();
	}

	return false;
}

HalfRoundedButton::HalfRoundedButton(QWidget *parent)
	: QPushButton(parent)
{
	this->installEventFilter(this);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	setCheckable(true);

	auto f = font();
	f.setBold(true);
	setFont(f);
}

void HalfRoundedButton::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	QColor color{ isChecked() ? Theme::fontTurquoiseClicked : Theme::sectionBackground };

	if (m_hover && !isChecked())
	{
		color = Theme::mainBackgroundColor;
	}
	

	QPen pen(Theme::border);
	pen.setCosmetic(true);
	pen.setWidth(2);
	painter.setPen(pen);


	auto path = Theme::getHalfCurvedPath(width(), height());


	if (m_reversed) {
		QTransform mirror(-1, 0, 0, 0, 1, 0, 0, 0, 1);
		painter.setTransform(mirror);
		painter.translate(-width(), 0);
	}

	painter.fillPath(path, color);
	painter.drawPath(path);

	painter.resetTransform();


	painter.setPen(isChecked() || m_hover ? Theme::sectionBackground : Theme::mainBackgroundColor);

	painter.drawText(rect(), Qt::AlignCenter, text());

	painter.end();
}


HalfRoundedButton::~HalfRoundedButton()
{
}
