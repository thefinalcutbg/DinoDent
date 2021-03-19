#pragma once

#include <QWidget>
#include "ui_ListView.h"
#include "IListView.h"
#include "Presenter/ListPresenter/ListPresenter.h"
#include "teethView/teethviewscene.h"
#include "View/AllergiesDialog/AllergiesDialog.h"
#include "View/ProcedureDialog/ProcedureDialog.h"

class ListView : public QWidget, public IListView
{
	Q_OBJECT


	ListPresenter presenter;
	TeethViewScene* teethViewScene;
	AllergiesDialog allergiesDialog;
	ContextMenu* contextMenu;
	ProcedureDialog procedureDialog;

	void paintEvent(QPaintEvent* event);
	
	void updateSelectedTeeth();

public:
	ListView(Database* database, QWidget* parent = Q_NULLPTR);

	ListPresenter* Presenter();

	void refresh(AmbList& ambList, Patient& patient, std::vector<int>& selectedIndexes);
	void setCheckModel(const CheckModel& checkModel);
	void repaintTooth(const Tooth& tooth);
	void repaintBridges(const std::array<BridgeAppearance, 32> bridges);
	void updateControlPanel(const Tooth* tooth);
	~ListView();

private:
	Ui::ListView ui;
};
