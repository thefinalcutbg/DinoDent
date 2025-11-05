#pragma once

#include <QDialog>
#include  <QVBoxLayout>
#include <QTextEdit>

#include "ui_DetailedStatus.h"

#include "View/TableModels/ProcedureTableModel.h"


class DetailedStatusPresenter;

class DetailedStatus final: public QDialog
{
	Q_OBJECT

    DetailedStatusPresenter& presenter;

	ProcedureTableModel m_historyModel;

	void sendTableStatesToPresenter();

	//void paintEvent(QPaintEvent* event) override;

public:
    DetailedStatus(DetailedStatusPresenter& presenter);

	void setNotes(const std::string& notes) ;
	void focusNotes(bool focus) ;
	std::string getNotes() ;
	void setHistoryData(const std::vector<Procedure>& history, Procedure::DatabaseSource checkboxValue) ;

	~DetailedStatus();

private:
	Ui::DetailedStatus ui;


};
