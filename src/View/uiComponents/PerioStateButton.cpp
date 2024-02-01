#include "PerioStateButton.h"
#include <QMouseEvent>


PerioStateButton::PerioStateButton(QWidget *parent)
	: PerioButton(parent)
{
	setCheckable(true);
	
	auto f = font();
	f.setBold(true);
	setFont(f);
	
    connect(this, &QAbstractButton::clicked,  this, [&] { setState(m_state + 1); });
}

void PerioStateButton::setState(int state)
{
	if (states == nullptr) return;

    if (state < 1 || (std::size_t)state > states->size())
	{
		setChecked(false);
		m_state = 0;
		update();
		return;
	}


	setChecked(true);
	m_state = state;
	setText(states->at(state - 1).data());
	
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
