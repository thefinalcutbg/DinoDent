#pragma once

#include <QDialog>
#include  <QVBoxLayout>
#include "View/Interfaces/IDetailedStatusView.h"
#include "ui_DetailedStatus.h"
#include "View/Graphics/ToothPainter.h"
#include "Model/Dental/Enums.h"
#include "View/SubWidgets/ImplantView.h"
#include "View/SubWidgets/ObturationWidget.h"
#include "View/SubWidgets/CrownWidget.h"
#include "View/SubWidgets/PathologyWidget.h"
#include "View/SubWidgets/DentistMadeWidget.h"
#include "View/SubWidgets/PostWidget.h"
#include "View/Models/ProcedureTableModel.h"
#include <QTextEdit>

class DetailedStatusPresenter;

class DetailedStatus final: public QDialog, public IDetailedStatusView
{
	Q_OBJECT

	DetailedStatusPresenter* presenter;

	inline static QString surfName[surfaceCount]
	{ "Оклузално", "Медиално", "Дистално",
		"Букално", "Лингвално", "Цервикално" };

	QVBoxLayout* layout;

	ObturationWidget* obtWidget;
	CrownWidget* crownWidget;
	ImplantView* implantWidget;
	DentistMadeWidget* dentistWidget;
	PathologyWidget* pathologyWidget;
	PostWidget* postWidget;
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
	void setData(const Pathology& data) override;
	void setData(const std::string& notesData) override;
	void setData(const PostData& data) override;

	ObturationData getObturationData() override;
	ImplantData getImplantData() override;
	bool getDentistData() override;
	CrownData getCrownData() override;
	int getDiagnosisIndex() override;
	std::string getNotes() override;
	PostData getPostData() override;

	void setHistoryData(const std::vector<Procedure>& history) override;

	~DetailedStatus();

private:
	Ui::DetailedStatus ui;




};
