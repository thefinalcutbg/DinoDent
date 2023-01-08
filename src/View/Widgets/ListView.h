#pragma once

#include <QWidget>


#include "ui_ListView.h"
#include "View/Interfaces/IListView.h"
#include "Presenter/ListPresenter.h"
#include "View/Graphics/TeethViewScene.h"
#include "View/Models/ProcedureTableModel.h"


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

    void setAmbListNum(int number) override;
    void setDateTime(const Date& date, const Time& time) override;
    void showSheetNumber(bool show) override;
    //IStatusView
    void setCheckModel(const CheckModel& checkModel) override;
    void hideSurfacePanel(bool hidden) override;
    void hideControlPanel(bool hidden) override;
    ISurfacePanel* surfacePanel() override;
    IPatientTileInfo* tileInfo() override;
    void repaintTooth(const ToothPaintHint& tooth) override;
    void setNotes(const std::array<std::string, 32>& notes) override;
    void refreshPriceLabel(/*double patientPrice,*/ double nzokPrice) override;
    //IProcedureView
    void setSelectedTeeth(std::vector<int> selectedTeeth) override;
    void setProcedures(const std::vector<Procedure>& m) override;
    void hideNhifSheetData() override;
    void setNhifData(const NhifSheetData& data, bool showUnfav) override;
    void setReferrals(const std::vector<Referral>& referrals) override;
    ~ListView();

private:
    Ui::ListView ui;
};
