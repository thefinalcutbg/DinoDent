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

    auto r = rect();

	if (m_hover) {
        QPainterPath path;
        path.addRoundedRect(r.x()+10, r.y()+2, r.width()-20, r.height()-4, Theme::radius/2, Theme::radius/2);
        painter.fillPath(path, Theme::background);
	}

	QPen pen(Theme::border);
	pen.setCosmetic(true);
	pen.setWidth(2);
	painter.setPen(pen);

    painter.setPen(isChecked() ? Theme::fontTurquoiseClicked : Qt::lightGray);

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
