#pragma once

#include <QDialog>
#include "ui_SaveAsDateDialog.h"
#include "Model/Date.h"
#include <optional>

class SaveAsDateDialog : public QDialog
{
	Q_OBJECT

	std::optional<Date> m_date;

public:
	SaveAsDateDialog(const Date& d, const std::string& docName, QWidget* parent = Q_NULLPTR);
	std::optional<Date> getResult(){ return m_date; }
	~SaveAsDateDialog();
private:
	Ui::SaveAsDateDialog ui;
};
