#pragma once

#include <QWidget>
#include "ui_ControlPanel.h"

#include "Presenter/ListPresenter/IStatusControl.h"

class ControlPanel : public QWidget
{
	Q_OBJECT

	std::array<StatusButton*, 16> statusButtons;
	IStatusControl* statusControl;

public:
	ControlPanel(QWidget *parent = Q_NULLPTR);
	~ControlPanel();

	void setStatusControl(IStatusControl* statusControl);

	void setModel(const CheckModel& checkModel);

private:
	Ui::ControlPanel ui;
};
