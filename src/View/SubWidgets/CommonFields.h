#pragma once

#include <QWidget>
#include "ui_CommonFields.h"

#include "View/Interfaces/ICommonFields.h"

class DateEdit;


class CommonFields : public QWidget, public ICommonFields
{
	Q_OBJECT


public:
	CommonFields(QWidget *parent = Q_NULLPTR);
	//void setExternalDateEdit(DateEdit* externalDateEdit); //does not take ownership over the DateEdit!
	~CommonFields();

	// Inherited via ICommonFields
	AbstractLineEdit* diagnosisEdit() override;
	AbstractDateEdit* dateEdit() override;
	std::string getNotes() override;

	QDateEdit* qDateEdit();

	Ui::CommonFields ui;

	// Inherited via ICommonFields
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
