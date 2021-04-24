#pragma once

#include <QIcon>

#include <QMenu>
#include "Presenter/ListPresenter/IStatusControl.h";
#include "Model/CheckState.h"


class ContextMenu : public QMenu
{
	Q_OBJECT

	IStatusControl* statusControl;

	std::array<QAction*, 6>surfObt;
	std::array<QAction*, 6> surfCar;
	std::array<QAction*, ActionCount> menuAction;


	template<int CheckStateSize, int QActionSize>
	void setModel(const std::array <CheckState, CheckStateSize>& model, std::array<QAction*, QActionSize>& action);

public:
	ContextMenu(IStatusControl* statusControl);
	void setModel(const CheckModel& checkModel);
	~ContextMenu();
};

