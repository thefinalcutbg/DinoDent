#pragma once

#include <QWidget>


#include "ui_ListView.h"
#include "IListView.h"
#include "Presenter/ListPresenter/ListPresenter.h"
#include "TeethView/TeethViewScene.h"

#include "View/ProcedureDisplayModel/ProcedureTableModel.h"


class ListView : public QWidget, public IListView
{
    Q_OBJECT

    ListPresenter* presenter;

    TeethViewScene* teethViewScene;

    ContextMenu* contextMenu;

    ProcedureTableModel model;

    void paintEvent(QPaintEvent* event);

    bool m_teethViewFocused {false};
    bool eventFilter(QObject* obj, QEvent* event);
    void nhifChanged();

public:
    ListView(QWidget* parent = Q_NULLPTR);

    void setPresenter(ListPresenter* presenter) override;

    void refresh(const AmbList& ambList, const Patient& patient) override;
    void setAmbListNum(int number) override;
    //IStatusView
    void setCheckModel(const CheckModel& checkModel) override;
    void hideSurfacePanel(bool hidden) override;
    void hideControlPanel(bool hidden) override;
    ISurfacePanel* surfacePanel() override;
    void repaintTooth(const ToothPaintHint& tooth) override;
    void setNotes(const std::array<std::string, 32>& notes) override;
    void disableGraphicsView(bool disabled) override;
    void refreshPriceLabel(double patientPrice, double nzokPrice) override;
    //IProcedureView
    void setSelectedTeeth(std::vector<int> selectedTeeth) override;
    void setProcedures(const std::vector<Procedure>& m) override;
    void hideNhifSheetData() override;
    void setNhifData(const NhifSheetData& data) override;

    ~ListView();

private:
    Ui::ListView ui;
};
