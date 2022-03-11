#include "StatusButton.h"
#include "View/Theme.h"
#include "QPainter"
#include "QEvent"
StatusButton::StatusButton(QWidget* parent) : QPushButton(parent), checkState(CheckState::unchecked)
{
	setCheckable(true);
	setFocusPolicy(Qt::NoFocus);
	installEventFilter(this);
}

void StatusButton::setCheckState(const CheckState& checkState)
{
	this->checkState = checkState;

	if (checkState == CheckState::checked) {
		setChecked(1);
		return;
	}

	setChecked(0);
}

void StatusButton::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	QPainterPath outline;
	

	QPen pen;
	pen.setColor(pathology ? Theme::fontRed : QColor(183, 221, 225));

	QFont font("Segoe UI");
	font.setPointSizeF(10);

	if (isChecked())
	{
		font.setBold(true);
		pen.setColor(QColor(237, 141, 126));
	}

	outline.addRoundedRect(1, 1, rect().width() - 2, rect().height() - 2, 15, 15);

	if (m_hover) {
		painter.fillPath(outline, Theme::background);
	}

	painter.setPen(pen);

	painter.setFont(font);

	painter.drawPath(outline);

	painter.setPen(isChecked() ? QPen(Theme::fontRed) : QPen(Theme::fontTurquoise));

	painter.drawText(rect(), Qt::AlignCenter, text());


	painter.end();

}

bool StatusButton::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::HoverEnter) {
		m_hover = true;
		update();
	}

	if (e->type() == QEvent::HoverLeave) {
		m_hover = false;
		update();
	}

	if (e->type() == QEvent::MouseButtonPress)
	{
		m_clicked = true;
	}

	if (e->type() == QEvent::MouseButtonRelease)
	{
		m_clicked = false;
	}

	return false;
}
