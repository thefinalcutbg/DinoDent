#pragma once
#include <QDateEdit>
#include "../AbstractLineEdit.h"

class DateLineEdit : public QDateEdit, public AbstractLineEdit
{
	Q_OBJECT

protected:
	void stateChangedByUser();
public:
	DateLineEdit(QWidget* parent);
	void setFocusAndSelectAll() override;
	void setAppearence(bool valid) override;
	void setFieldText(const std::string& text) override;
	std::string getText() override;
	void reset();
	void disable(bool disable);
};