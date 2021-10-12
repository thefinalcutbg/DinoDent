#pragma once

#include <QDialog>
#include  <QVBoxLayout>
#include "IDetailedStatusView.h"
#include "ui_DetailedStatus.h"
#include "View/ToothPaintDevices/ToothPainter.h"
#include "Model/Tooth/Enums.h"
#include "View/uiComponents/qt_derived/Widgets/ImplantView.h"
#include "View/uiComponents/qt_derived/Widgets/ObturationWidget.h"
#include "View/uiComponents/qt_derived/Widgets/CrownWidget.h"
#include "View/uiComponents/qt_derived/Widgets/PathologyWidget.h"
#include "View/uiComponents/qt_derived/Widgets/DentistMadeWidget.h"
#include "View/ProcedureDisplayModel/ProcedureTableModel.h"
#include <QTextEdit>

class DetailedStatusPresenter;

class DetailedStatus final: public QDialog, public IDetailedStatusView
{
	Q_OBJECT

	DetailedStatusPresenter* presenter;

	inline static QString surfName[surfaceCount]
	{ u8"Оклузално", u8"Медиално", u8"Дистално",
		u8"Букално", u8"Лингвално", u8"Цервикално" };

	QVBoxLayout* layout;

	ObturationWidget* obtWidget;
	CrownWidget* crownWidget;
	ImplantView* implantWidget;
	DentistMadeWidget* dentistWidget;
	PathologyWidget* pathologyWidget;
	QTextEdit* notesWidget;

	ProcedureTableModel m_historyModel;

	void paintEvent(QPaintEvent* event) override;

public:
	DetailedStatus(DetailedStatusPresenter* presenter);

	void setCheckModel(const CheckModel& checkModel) override;
	void disableItem(int index, bool disabled) override;
	void paintTooth(const ToothPaintHint& hint) override;


	void clearData() override;
	void disableDetails(bool disabled) override;
	 
	void setData(const ImplantData& data) override;
	void setData(const DentistData& data) override;
	void setData(const CrownData& data) override;
	void setData(const ObturationData& data) override;
	void setData(const PathologyData& data) override;
	void setData(const std::string& notesData) override;

	ObturationData getObturationData() override;
	ImplantData getImplantData() override;
	bool getDentistData() override;
	CrownData getCrownData() override;
	int getPathologyData() override;
	std::string getNotes() override;

	void setHistoryData(const std::vector<Procedure>& history) override;

	~DetailedStatus();

private:
	Ui::DetailedStatus ui;




};
