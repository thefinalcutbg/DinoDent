#pragma once

#include <QWidget>
#include "ui_ControlPanel.h"

class ListPresenter;

class ControlPanel : public QWidget
{
	Q_OBJECT

	std::array<StatusButton*, statusCount> statusButtons;
	ListPresenter* presenter;

public:
	ControlPanel(QWidget *parent = Q_NULLPTR);
	~ControlPanel();

	void setPresenter(ListPresenter* presenter);
	void hideCommonButtons(bool hidden);
	void setModel(const CheckModel& checkModel);

private:
	Ui::ControlPanel ui;
};
