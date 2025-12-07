#include "HalfRoundedButton.h"
#include <QEvent>
#include <QPainter>
#include <QApplication>

#include "View/Theme.h"

bool HalfRoundedButton::eventFilter(QObject*, QEvent* e)
{
    if (e->type() == QEvent::Enter) {
		m_hover = true;
		update();
	}

    if (e->type() == QEvent::Leave) {
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

	setCursor(QCursor(Qt::PointingHandCursor));

	auto f = font();
	f.setBold(true);
	setFont(f);
}

void HalfRoundedButton::paintEvent(QPaintEvent*)
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

	QPainterPath path;

	switch (m_position)
	{
	case Position::Center: path.addRect(this->rect()); break;
	case Position::Left: path = Theme::getHalfCurvedPath(width(), height()); break;
	case Position::Right:
		{
			path = Theme::getHalfCurvedPath(width(), height());
			QTransform mirror(-1, 0, 0, 0, 1, 0, 0, 0, 1);
			painter.setTransform(mirror);
			painter.translate(-width(), 0);
		}
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
