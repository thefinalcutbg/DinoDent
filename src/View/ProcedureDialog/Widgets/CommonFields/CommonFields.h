#pragma once

#include <QWidget>
#include "ui_CommonFields.h"

#include "ICommonFields.h"

class CommonFields : public QWidget, public ICommonFields
{
	Q_OBJECT

public:
	CommonFields(QWidget *parent = Q_NULLPTR);
	~CommonFields();

private:
	Ui::CommonFields ui;

	// Inherited via ICommonFields
	virtual AbstractLineEdit* manipulationEdit() override;
	virtual AbstractLineEdit* diagnosisEdit() override;
	virtual AbstractSpinBox* priceEdit() override;
	virtual AbstractLineEdit* dateEdit() override;
	virtual void set_hidden(bool hidden) override;
};
