#pragma once

#include "View/uiComponents/ShadowBakeWidget.h"
#include "ui_PrescriptionView.h"

#include "View/TableModels/MedicationTableModel.h"

class PrescriptionPresenter;
struct Dispensation;

class PrescriptionView final : public ShadowBakeWidget
{
	Q_OBJECT

	PrescriptionPresenter* presenter{ nullptr };

	MedicationTableModel medModel;

	void dispensationLogic();

	void sendFemaleProperties();

public:
	PrescriptionView(QWidget *parent = nullptr);
	void setPresenter(PrescriptionPresenter* p) { presenter = p;}
	PatientTileInfo* patientTile();
	void setMedicationList(const std::vector<std::string>);
	void setDispensation(const Dispensation& d);
	void setSupplements(const std::string& supplements);
	void setDate(const Date& date);
	void setNrn(const std::string& nrn);
	void setMisc(bool isFemale, bool isPregnant, bool isBreastFeeding);

	~PrescriptionView();

private:
	Ui::PrescriptionViewClass ui;





};
