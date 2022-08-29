#pragma once
#include "customdateedit.h"
#include "View/Interfaces/AbstractDateEdit.h"

class QLabel;

class DateEdit : public CustomDateEdit, public AbstractDateEdit
{
	Q_OBJECT
	
protected:
	QLabel* errorLabel;

public:
	DateEdit(QWidget* parent);
	void setFocus() override;
	void setValidAppearence(bool valid) override;
	void set_Date(const Date& date) override;
	Date getDate() override;
	void reset();
	void disable(bool disable);

	void setErrorLabel(QLabel* errorLabel);
};