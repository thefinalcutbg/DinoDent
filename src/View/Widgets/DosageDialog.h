#pragma once

#include <QDialog>

#include "View/Interfaces/IDosageDialog.h"

#include "ui_DosageDialog.h"
#include "Model/Validators/PrescriptionValidators.h"

class DosagePresenter;

class DosageDialog final: public QDialog, public IDosageDialog
{
	Q_OBJECT

	DosagePresenter* presenter;

	RouteFormValidator route_validator;
	WhenTextValidator when_validator;

public:
	DosageDialog(DosagePresenter* p, QWidget* parent = nullptr);
	~DosageDialog();

private:
	Ui::DosageDialogClass ui;

	void setBoundsList(const std::vector<std::string>& list) override;
	void setPeriodList(const std::vector<std::string>& list) override;

	void setDoseFormCompletionList(const std::vector<std::string>& list) override;
	void setWhenFormCompletionList(const std::vector<std::string>& list) override;

	void setDosageUnit(const std::string& unitName) override;

	void setRouteList(const std::vector<std::string>& list) override;

	 void setRouteString(const std::string& route) override;

	 void setWhenTags(const std::vector<std::string>& tags, bool offsetAllowed) override;

	 void setParsed(const std::string& parsed) override;

	 void setDosage(const Dosage& d) override;

	 bool fieldsAreValid() override;
	 void closeUi() override {close();}
};
