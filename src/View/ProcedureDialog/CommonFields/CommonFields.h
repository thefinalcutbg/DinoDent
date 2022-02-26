#pragma once

#include <QWidget>
#include "ui_CommonFields.h"

#include "ICommonFields.h"

class DateEdit;

class CommonFields : public QWidget, public ICommonFields
{
	Q_OBJECT

private:

	DateEdit* externalDateEdit{nullptr};

public:
	CommonFields(QWidget *parent = Q_NULLPTR);
	void setExternalDateEdit(DateEdit* externalDateEdit); //does not take ownership over the DateEdit!
	~CommonFields();

	// Inherited via ICommonFields
	virtual AbstractLineEdit* manipulationEdit() override;
	virtual AbstractLineEdit* diagnosisEdit() override;
	virtual AbstractSpinBox* priceEdit() override;
	virtual AbstractDateEdit* dateEdit() override;
	virtual void set_hidden(bool hidden) override;

	Ui::CommonFields ui;
};
