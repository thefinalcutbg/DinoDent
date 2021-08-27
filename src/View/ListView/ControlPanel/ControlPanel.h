#pragma once

#include <QWidget>
#include "ui_ControlPanel.h"

class StatusPresenter;

class ControlPanel : public QWidget
{
	Q_OBJECT

	std::array<StatusButton*, statusCount> statusButtons;
	StatusPresenter* presenter;

public:
	ControlPanel(QWidget *parent = Q_NULLPTR);
	~ControlPanel();

	void setPresenter(StatusPresenter* presenter);
	void hideCommonButtons(bool hidden);
	void setModel(const CheckModel& checkModel);

private:
	Ui::ControlPanel ui;
};
