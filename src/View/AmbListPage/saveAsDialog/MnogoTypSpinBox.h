#pragma once

#include <QSpinBox>
#include <QLabel>

class MnogoTypSpinBox : public QSpinBox
{
	Q_OBJECT

	std::map <int, bool>* validNumber;
	QLabel* msgLabel;
	void validate();
	bool validState;

public:
	MnogoTypSpinBox(QWidget* parent = 0);
	bool isValid();
	void setMap(std::map <int, bool>* existingNumbersMap);
	void setLabelMsg(QLabel* label);
};

