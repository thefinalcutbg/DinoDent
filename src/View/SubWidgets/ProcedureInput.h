#pragma once

#include <QWidget>
#include "ui_ProcedureInput.h"

#include "View/Interfaces/IProcedureInput.h"

class DateEdit;


class ProcedureInput : public QWidget, public IProcedureInput
{
	Q_OBJECT


public:
	ProcedureInput(QWidget *parent = Q_NULLPTR);
	//void setExternalDateEdit(DateEdit* externalDateEdit); //does not take ownership over the DateEdit!
	~ProcedureInput();

	// Inherited via IProcedureInput
	AbstractLineEdit* diagnosisEdit() override;
	AbstractDateEdit* dateEdit() override;
	std::string getNotes() override;

	QDateEdit* qDateEdit();

	Ui::ProcedureInput ui;

	// Inherited via IProcedureInput
	virtual AbstractRangeEdit* rangeWidget() override;
	virtual AbstractSurfaceSelector* surfaceSelector() override;
	virtual void setErrorMsg(const std::string& error) override;
	virtual void setLayout(WidgetLayout layout) override;
	virtual void setNotes(const std::string& notes) override;
	void setNhifLayout(bool nhif) override;
	FinancingSource getFinancingSource() override;
	void setFinancingSource(FinancingSource s) override;
	void setHyperdonticState(bool checked) override;
	bool onHyperdontic() override;
	
};
