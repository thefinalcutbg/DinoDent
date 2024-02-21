#include "ToothButton.h"
#include <QPainter>
#include <QEvent>
#include "View/Theme.h"
#include <QApplication>

ToothButton::ToothButton(QWidget *parent)
	: QPushButton(parent)
{
	this->installEventFilter(this);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	setCheckable(true);

	auto f = font();
	f.setBold(true);
	setFont(f);
}

bool ToothButton::eventFilter(QObject*, QEvent* e)
{
    if (e->type() == QEvent::Enter) {
		QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
		m_hover = true;
		update();
	}

    if (e->type() == QEvent::Leave) {
		QApplication::restoreOverrideCursor();
		m_hover = false;
		update();
	}

	return false;
}

void ToothButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	QColor color{ isChecked() ? Theme::sectionBackground : Theme::background };

	if (m_hover) {
		color = Theme::background;
	}

	QPen pen(Theme::border);
	pen.setCosmetic(true);
	pen.setWidth(2);
	painter.setPen(pen);

	switch (m_layoutPos)
	{
		case LayoutPosition::Center:
		{
			QRect border_rect{ rect() };
			painter.fillRect(border_rect, color);
			painter.drawRect(border_rect);
		}
			break;
		case LayoutPosition::Left:
		{
			auto path = Theme::getOneCornerCurvedRect(width(), height());
			painter.fillPath(path, color);
			painter.drawPath(path);
		}
			break;
		case LayoutPosition::Right:
		{

			QTransform mirror(-1, 0, 0, 0, 1, 0, 0, 0, 1);
			painter.setTransform(mirror);
			painter.translate(-width(), 0);

			auto path = Theme::getOneCornerCurvedRect(width(), height());
			painter.fillPath(path, color);
			painter.drawPath(path);

			painter.resetTransform();
		}
			break;
	}


	painter.setPen(isChecked() || m_hover ? Theme::fontTurquoiseClicked : Qt::lightGray);

	painter.drawText(rect(), Qt::AlignCenter, text());

	painter.end();
}


void ToothButton::setLayoutPos(LayoutPosition p)
{
	this->m_layoutPos = p;
	update();
}


ToothButton::~ToothButton()
{
}
