#pragma once

#include <QWidget>
#include <QOpenGLWidget>

#include "ui_ListView.h"
#include "IListView.h"
#include "Presenter/ListPresenter/ListPresenter.h"
#include "TeethView/TeethViewScene.h"
#include "View/AllergiesDialog/AllergiesDialog.h"
#include "View/ProcedureDialog/ProcedureDialog.h"

#include "Procedures/ProcedureTableModel.h"


class ListView : public QWidget, public IListView
{
    Q_OBJECT

    ListPresenter presenter;
    ProcedurePresenter* procedure_presenter;
    TeethViewScene* teethViewScene;
    AllergiesDialog allergiesDialog;
    ContextMenu* contextMenu;

    ProcedureTableModel model;

    void paintEvent(QPaintEvent* event);

public:
    ListView(QWidget* parent = Q_NULLPTR);

    ListPresenter* Presenter();

    void setStatusControlPresenter(StatusPresenter* presenter) override;
    void setProcedurePresenter(ProcedurePresenter* presenter) override;
    void refresh(AmbList& ambList, Patient& patient) override;
    void setCheckModel(const CheckModel& checkModel) override;
    void hideSurfacePanel(bool hidden) override;
    ISurfacePanel* surfacePanel() override;
    void repaintTooth(const ToothPaintHint& tooth) override;
    void repaintBridges(const BridgesPaintHint& bridges) override;

    void setSelectedTeeth(std::vector<int> selectedTeeth) override;
    void setProcedures(const std::vector<RowData>& m) override;
    void openProcedureDialog(ProcedureDialogPresenter *p) override;
    virtual void setUnfav(bool unfav) override;
    ~ListView();

private:
    Ui::ListView ui;
};
