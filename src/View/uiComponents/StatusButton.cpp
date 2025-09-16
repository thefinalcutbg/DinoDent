#include "StatusButton.h"
#include "View/Theme.h"
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

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

void StatusButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	QPainterPath outline;
    outline.addRoundedRect(1, 1, rect().width() - 2, rect().height() - 2, 15, 15);
	painter.fillPath(outline, m_hover ? Theme::background : Theme::sectionBackground);

	QPen pen;

    QFont font;

	if (isChecked())
	{
		font.setBold(true);

		pen.setColor(pathology ? Theme::fontRed : Theme::fontTurquoiseClicked);
		painter.setPen(pen);
		painter.drawPath(outline);
		painter.setFont(font);
		painter.drawText(rect(), Qt::AlignCenter, text());
		

	}
	else
	{
		pen.setColor(Theme::buttonFrame);
		painter.setPen(pen);
		painter.drawPath(outline);

		pen.setColor(Theme::fontTurquoise);
		painter.setPen(pen);
		painter.setFont(font);
		painter.drawText(rect(), Qt::AlignCenter, text());
	}	

	painter.end();

}

bool StatusButton::eventFilter(QObject*, QEvent* e)
{
    if (e->type() == QEvent::Enter) {
		m_hover = true;
		update();
	}

    if (e->type() == QEvent::Leave) {
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

void StatusButton::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton) {

		if (menu()) {
			showMenu();
			m_hover = false;
			update();
			return;
		}

		emit clicked();
		return;
	}

	if (e->button() == Qt::RightButton) {
		emit rightClicked();
	}

	QPushButton::mousePressEvent(e);
}
