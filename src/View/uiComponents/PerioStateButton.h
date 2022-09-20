#pragma once
#include <vector>
#include <string_view>
#include "PerioButton.h"

typedef std::vector<std::string_view> PerioButtonStates;

class PerioStateButton : public PerioButton
{
	Q_OBJECT

	short int m_state{ 0 };

	const PerioButtonStates* states;

	void mousePressEvent(QMouseEvent* e);

public:

	PerioStateButton(QWidget *parent);
	

	void setAvailableStates(const PerioButtonStates* states) { this->states = states; }

	void setState(int state);
	int getState() {	return m_state; }

	~PerioStateButton();
};
