#include "PerioStateButton.h"
#include <QMouseEvent>


PerioStateButton::PerioStateButton(QWidget *parent)
	: PerioButton(parent)
{
	setCheckable(true);
	
	auto f = font();
	f.setBold(true);
	setFont(f);

	QPalette palet = palette();
	QColor textColor(Qt::GlobalColor::darkGray);
	palet.setBrush(QPalette::Text, QBrush(textColor));
	setPalette(palet);

	
	connect(this, &QAbstractButton::clicked, [=] { setState(m_state + 1); });
}

void PerioStateButton::setState(int state)
{

	if (state < 1 || state > states->size())
	{
		setChecked(false);
		m_state = 0;
		update();
		return;
	}


	setChecked(true);
	m_state = state;
	setText(states->at(state - 1));
	
	update();
}

void PerioStateButton::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::RightButton)
		setState(0);
	else
		QAbstractButton::mousePressEvent(e);
}


PerioStateButton::~PerioStateButton()
{
}