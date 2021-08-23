#pragma once

#include <QWidget>
#include <QOpenGLWidget>

#include "ui_ListView.h"
#include "IListView.h"
#include "Presenter/ListPresenter/ListPresenter.h"
#include "TeethView/TeethViewScene.h"

#include "Procedures/ProcedureTableModel.h"


class ListView : public QWidget, public IListView
{
    Q_OBJECT

    ListPresenter* presenter;
    ProcedurePresenter* procedure_presenter;
    TeethViewScene* teethViewScene;

    ContextMenu* contextMenu;

    ProcedureTableModel model;

    void paintEvent(QPaintEvent* event);

public:
    ListView(QWidget* parent = Q_NULLPTR);

    void setPresenter(ListPresenter* presenter) override;

    void setStatusControlPresenter(StatusPresenter* presenter) override;
    void setProcedurePresenter(ProcedurePresenter* presenter) override;

    void refresh(AmbList& ambList, Patient& patient) override;

    //IStatusView
    void setCheckModel(const CheckModel& checkModel) override;

    void hideSurfacePanel(bool hidden) override;
    void hideControlPanel(bool hidden) override;
    ISurfacePanel* surfacePanel() override;
    void repaintTooth(const ToothPaintHint& tooth) override;
    void repaintBridges(const BridgesPaintHint& bridges) override;

    //IProcedureView
    void setSelectedTeeth(std::vector<int> selectedTeeth) override;
    void setProcedures(const std::vector<ProcedureRowData>& m, double patientPrice, double nzokPrice) override;
    AbstractComboBox* taxCombo() override;
    void setUnfav(bool unfav) override;
    ~ListView();

private:
    Ui::ListView ui;
};
