#pragma once

#include <QWidget>
#include "ui_ControlPanel.h"

class IStatusControl;

class ControlPanel : public QWidget
{
	Q_OBJECT

	std::array<StatusButton*, 18> statusButtons;
	IStatusControl* statusControl;

public:
	ControlPanel(QWidget *parent = Q_NULLPTR);
	~ControlPanel();

	void setStatusControl(IStatusControl* statusControl);

	void setModel(const CheckModel& checkModel);

private:
	Ui::ControlPanel ui;
};
