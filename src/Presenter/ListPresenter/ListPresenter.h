#pragma once

#include <vector>
#include <memory>
#include "View/ListView/IListView.h"

#include "Database/DbAmbList.h"

#include "Presenter/TabPresenter/TabInstance.h"
#include "SurfacePanel/SurfacePanelPresenter.h"
#include "CheckState.h"
#include "Network/ReplyHandlers/DentalActivitiesHandler.h"

typedef std::vector<int> SelectedTeethIdx;
typedef std::vector<Tooth*> SelectedTeeth;


class TabPresenter;

class ListPresenter : public TabInstance
{
    SurfacePanelPresenter surf_presenter;

    SelectedTeethIdx m_selectedIndexes;
    SelectedTeeth m_selectedTeeth;

    DentalActivitiesHandler<ListPresenter> handler{this};

    bool m_showCurrentStatus{ false };

    CheckModel m_checkModel;

    IListView* view;

    bool m_openHistoryDialogOnReply { false };
    bool awaitingPisHistoryReply{ false };

    TabPresenter* tabPresenter;

    void requestPisActivities();
    bool isValid();
    void refreshProcedureView();
    void statusChanged();

public:

    AmbList m_ambList;

    ListPresenter(ITabView* tabView, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient, long long rowId = 0);

    void chargeChanged(int index);

    long long rowID() const override;
    bool save() override;
    bool saveAs() override;
    bool isNew() override;
    void print() override;
    TabName getTabName() override;
    void setCurrent() override;

    void openPatientDialog();
    void openAllergiesDialog();

    void setCaries(int surface);
    void setObturation(int surface);
    void setMainStatus(int code);
    void setOther(int code);
    
    void setSelectedTeeth(const std::vector<int>& SelectedIndexes);

    void setPISActivities(const std::optional<Procedures>& pisProcedures);

    void openPisHistory();
    void openDetails(int toothIdx);
    void openDetails();

    void addToProcedureList(const std::vector<Procedure>& new_mList);

    void addProcedure();
    void editProcedure(int index);
    void deleteProcedure(int index);
    void setfullCoverage(bool unfav);
    void createInvoice();
    void createPerioMeasurment();
    void showCurrentStatus(bool show);

    ~ListPresenter();
};

