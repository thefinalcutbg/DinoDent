#pragma once

#include <QWidget>
#include "ui_CommonFields.h"

#include "View/Interfaces/ICommonFields.h"

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
	AbstractLineEdit* procedureNameEdit() override;
	AbstractLineEdit* diagnosisEdit() override;
//	AbstractSpinBox* priceEdit() override;
	AbstractDateEdit* dateEdit() override;
	void setKSMPCode(const std::string& code) override;
	void enableKSMP(bool enabled) override;
	std::string getKSMPCode() override;
	void set_hidden(bool hidden) override;

	Ui::CommonFields ui;
};
