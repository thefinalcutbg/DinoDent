#pragma once

#include <QDialog>

#include "View/Interfaces/IDosageDialog.h"

#include "ui_DosageDialog.h"
#include "Model/Validators/PerscriptionValidators.h"

class DosagePresenter;

class DosageDialog final: public QDialog, public IDosageDialog
{
	Q_OBJECT

	DosagePresenter* presenter;

	DosageFormValidator form_validator;
	RouteFormValidator route_validator;

public:
	DosageDialog(DosagePresenter* p, QWidget *parent = nullptr);
	~DosageDialog();

private:
	Ui::DosageDialogClass ui;

	void setDurationList(const std::vector<std::string>& list) override;
	void setPeriodList(const std::vector<std::string>& list) override;
	void setDoseFormCompletionList(const std::vector<std::string>& list) override;
	void setDosageUnit(const std::string& unitName) override;
	void setDosageLabel(const std::string& label) override;
	void setRouteList(const std::vector<std::string>& list) override;
	 void setRouteString(const std::string& route) override;
};
