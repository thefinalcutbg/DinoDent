#pragma once

#include <QDialog>
#include  <QVBoxLayout>
#include "View/Interfaces/IDetailedStatusView.h"
#include "ui_DetailedStatus.h"
#include "View/Graphics/ToothPainter.h"
#include "Model/Dental/Enums.h"

#include "View/TableModels/ProcedureTableModel.h"
#include <QTextEdit>

class DetailedStatusPresenter;

class DetailedStatus final: public QDialog, public IDetailedStatusView
{
	Q_OBJECT

	DetailedStatusPresenter* presenter;

	inline static QString surfName[surfaceCount]
	{ "Оклузално", "Медиално", "Дистално",
		"Букално", "Лингвално", "Цервикално" };


	ProcedureTableModel m_historyModel;

	//void paintEvent(QPaintEvent* event) override;

public:
	DetailedStatus(DetailedStatusPresenter* presenter);

	void setCheckModel(const CheckModel& checkModel, const CheckModel& dsn) override;
	void disableItem(int index, bool disabled) override;
	void paintTooth(const ToothPaintHint& hint) override;
	void setNotes(const std::string& notes) override;
	void setData(const std::string& notesData) override;

	std::string getNotes() override;


	void setHistoryData(const std::vector<Procedure>& history) override;

	~DetailedStatus();

private:
	Ui::DetailedStatus ui;


};
