#pragma once

#include <QDialog>
#include  <QVBoxLayout>
#include "View/Interfaces/IDetailedStatusView.h"
#include "ui_DetailedStatus.h"

#include "View/TableModels/ProcedureTableModel.h"
#include <QTextEdit>

class DetailedStatusPresenter;

class DetailedStatus final: public QDialog, public IDetailedStatusView
{
	Q_OBJECT

    DetailedStatusPresenter& presenter;

	ProcedureTableModel m_historyModel;

	void sendTableStatesToPresenter();

	//void paintEvent(QPaintEvent* event) override;

public:
    DetailedStatus(DetailedStatusPresenter& presenter);

	void setNotes(const std::string& notes) override;
	void focusNotes(bool focus) override;
	std::string getNotes() override;


	void setHistoryData(const std::vector<Procedure>& history) override;

	~DetailedStatus();

private:
	Ui::DetailedStatus ui;


};
