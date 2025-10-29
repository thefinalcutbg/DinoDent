#pragma once

#include <QDialog>

#include "ui_DosageDialog.h"
#include "Model/Validators/PrescriptionValidators.h"

class DosagePresenter;
struct Dosage;
class LineEdit;

class DosageDialog final: public QDialog
{
	Q_OBJECT

    DosagePresenter& presenter;

	RouteFormValidator route_validator;
	WhenTextValidator when_validator;

    LineEdit* whenLine;

public:
    DosageDialog(DosagePresenter& p, QWidget* parent = nullptr);

	void setBoundsList(const std::vector<std::string>& list);
	void setPeriodList(const std::vector<std::string>& list);

	void setDoseFormCompletionList(const std::vector<std::string>& list);
	void setWhenFormCompletionList(const std::vector<std::string>& list);

	void setDosageUnit(const std::string& unitName);

	void setRouteList(const std::vector<std::string>& list);

	void setRouteString(const std::string& route);

	void setWhenTags(const std::vector<std::string>& tags, bool offsetAllowed);

	void setParsed(const std::string& parsed);

	void setDosage(const Dosage& d);

	bool fieldsAreValid();

	~DosageDialog();

private:
	Ui::DosageDialogClass ui;


};
