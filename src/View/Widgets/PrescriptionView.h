#pragma once

#include <QWidget>
#include "ui_PrescriptionView.h"

#include "View/Interfaces/IPrescriptionView.h"
#include "View/TableModels/MedicationTableModel.h"

class PrescriptionView final : public QWidget, public IPrescriptionView
{
	Q_OBJECT

	PrescriptionPresenter* presenter{ nullptr };

	MedicationTableModel medModel;

	void dispensationLogic();

	void paintEvent(QPaintEvent* event);

	void sendFemaleProperties();

public:
	PrescriptionView(QWidget *parent = nullptr);
	void setPresenter(PrescriptionPresenter* p) override { presenter = p;}
	IPatientTileInfo* patientTile() override;
	void setMedicationList(const std::vector<std::string>) override;
	void setDispensation(const Dispensation& d) override;
	void setSupplements(const std::string& supplements) override;
	void setDate(const Date& date) override;
	void setNrn(const std::string& nrn) override;
	void setMisc(bool isFemale, bool isPregnant, bool isBreastFeeding) override;

	~PrescriptionView();

private:
	Ui::PrescriptionViewClass ui;





};
