#pragma once

#include <QDialog>
#include "ui_PISHistoryDialog.h"
#include "View/TableModels/PISDentalActivitiesModel.h"
#include "Model/Dental/Procedure.h"

class PISHistoryDialog : public QDialog
{
	Q_OBJECT

	PisDentalActivitiesModel model;
	

public:
	bool applyProcedures{ false };
	PISHistoryDialog(const std::vector<Procedure> procedures, const std::vector<std::string>& payment_status, QWidget *parent = Q_NULLPTR);
	~PISHistoryDialog();

private:
	Ui::PISHistoryDialog ui;
};
