#pragma once
#include <QDateEdit>
#include "View/Interfaces/AbstractDateEdit.h"

class QLabel;

class DateEdit : public QDateEdit, public AbstractDateEdit
{
	Q_OBJECT
	
protected:
	QLabel* errorLabel;

public:
	DateEdit(QWidget* parent);
    void set_focus() override;
	void setValidAppearence(bool valid) override;
	void set_Date(const Date& date) override;
	Date getDate() override;
    void reset() override;
    void disable(bool disable) override;

	void setErrorLabel(QLabel* errorLabel);
};
