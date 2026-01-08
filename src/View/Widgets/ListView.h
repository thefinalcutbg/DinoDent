#pragma once

#include <QWidget>
#include <vector>

#include "ui_ListView.h"

#include "Model/Dental/Procedure.h"
#include "Model/Referrals/Referral.h"

#include "Presenter/ListPresenter.h"

#include "View/Graphics/TeethViewScene.h"
#include "View/TableModels/ProcedureTableModel.h"

struct CheckModel;
struct AmbList;
struct ToothPaintHint;
class ISurfacePanel;
class ListPresenter;
class AbstractComboBox;
struct NhifSheetData;
struct MedicalNotice;
class Date;
class Time;

class ListView : public QWidget
{
    Q_OBJECT

    ListPresenter* presenter;

    TeethViewScene* teethViewScene;

    ContextMenu* contextMenu;

    ProcedureTableModel model;

    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

    void nhifChanged();

public:

    struct HisButtonProperties {
        bool hideSpinBox{ false };
        const std::string buttonText;
        const std::string hoverText;
    };

    ListView(QWidget* parent = Q_NULLPTR);

    void setPresenter(ListPresenter* presenter);
    void focusTeethView();
    void setDateTime(const std::string& time8601);
    void setTreatmentEnd(const std::string& time8601);
	void setSignature(const std::vector<unsigned char>& s);
    void setCheckModel(const CheckModel& checkModel, const CheckModel& dsnCheckModel);
    void hideSurfacePanel(bool hidden);
    void hideControlPanel(bool hidden);
    SurfacePanel* surfacePanel();
    PatientTileInfo* tileInfo();
    void repaintTooth(const ToothPaintHint& tooth);
    void setNotes(const std::array<std::string, 32>& notes);
    void setSelectedTeeth(const std::vector<int>& selectedTeeth);
    void setProcedures(const std::vector<Procedure>& m);
    void hideNhifSheetData();
    void setNhifData(const NhifSheetData& data, bool showUnfav);
    void setAdditionalDocuments(const std::vector<Referral>& referrals, const std::vector<MedicalNotice>& notices);
    void setHisButtonText(const HisButtonProperties& prop);
    void showAddPlannedButton(bool show);

    ~ListView();

private:
    Ui::ListView ui;
};
