#pragma once
#include <vector>
#include <memory>

#include "PerioButton.h"

typedef std::shared_ptr<std::vector<QString>> SharedStates;

class PerioStateButton : public PerioButton
{
	Q_OBJECT

	short int m_state{ 0 };

	SharedStates states;

	void mousePressEvent(QMouseEvent* e);

public:

	PerioStateButton(QWidget *parent);
	

	void setAvailableStates(SharedStates states) { this->states = states; }

	void setState(int state);
	short int getState() {	return m_state; }

	~PerioStateButton();
};
