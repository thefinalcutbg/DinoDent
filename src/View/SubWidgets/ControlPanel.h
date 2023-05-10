#pragma once

#include <QWidget>
#include "ui_ControlPanel.h"


class DsnMenu;
class ListPresenter;

class ControlPanel : public QWidget
{
	Q_OBJECT

	ListPresenter* presenter;

	DsnMenu* menu;

public:
	ControlPanel(QWidget *parent = Q_NULLPTR);
	~ControlPanel();

	void setPresenter(ListPresenter* presenter);
	void hideCommonButtons(bool hidden);
	void setModel(const CheckModel& checkModel, const CheckModel& dsn);

private:
	Ui::ControlPanel ui;
};
