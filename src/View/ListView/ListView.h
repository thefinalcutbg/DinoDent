#pragma once

#include <QWidget>

#include "ui_ListView.h"
#include "IListView.h"
#include "Presenter/ListPresenter/ListPresenter.h"
#include "teethView/teethviewscene.h"
#include "View/AllergiesDialog/AllergiesDialog.h"
#include "View/ProcedureDialog/ProcedureDialog.h"

#include "Procedures/ProcedureTableModel.h"

class ListView : public QWidget, public IListView
{
	Q_OBJECT


	ListPresenter presenter;
	TeethViewScene* teethViewScene;
	AllergiesDialog allergiesDialog;
	ContextMenu* contextMenu;

	ProcedureTableModel model;

	void paintEvent(QPaintEvent* event);
	
	void updateSelectedTeeth();

public:
	ListView(Database* database, QWidget* parent = Q_NULLPTR);

	ListPresenter* Presenter();

	void refresh(AmbList& ambList, Patient& patient, std::vector<int>& selectedIndexes) override;
	void setCheckModel(const CheckModel& checkModel) override;
	void repaintTooth(const Tooth& tooth) override;
	void repaintBridges(const std::array<BridgeAppearance, 32> bridges) override;
	void updateControlPanel(const Tooth* tooth) override;
	void setManipulations(const std::vector<RowData>& m) override;
	void openProcedureDialog(ProcedureDialogPresenter *p) override;
	~ListView();

private:
	Ui::ListView ui;
};
