#include "StatusMultiButton.h"

#include <QPainter>
#include <QEvent>
#include <QPainterPath>

#include "View/Theme.h"

StatusMultiButton::StatusMultiButton(QWidget *parent)
	: QPushButton(parent)
{
	installEventFilter(this);

	connect(this, &QPushButton::clicked, [&] {

			m_state = m_state == m_stateNames.size() - 1 ? 0 : m_state+1;
			setText(m_stateNames[m_state]);
			setChecked(m_state);
			
			emit stateChanged(m_state);
	
		});
}

void StatusMultiButton::setStateNames(const std::vector<QString>& names)
{
	m_stateNames = names;
	update();
}

void StatusMultiButton::setCurrentState(int state)
{
	blockSignals(true);

	setChecked(state);

	if (state < m_stateNames.size())
	{
		m_state = state;
		setText(m_stateNames[state]);
	}

	blockSignals(false);

	update();
	
}

bool StatusMultiButton::eventFilter(QObject*, QEvent* e)
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


void StatusMultiButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	QPainterPath outline;
	outline.addRoundedRect(1, 1, rect().width() - 2, rect().height() - 2, 15, 15);
	painter.fillPath(outline, m_hover ? Theme::background : Theme::sectionBackground);

	QPen pen;

    QFont font;

	if (m_state)
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
