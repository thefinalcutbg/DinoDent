#pragma once

#include <QWidget>
#include "ui_PerscriptionView.h"

#include "View/Interfaces/IPerscriptionView.h"
#include "View/Models/MedicationTableModel.h"

class PerscriptionView final : public QWidget, public IPerscriptionView
{
	Q_OBJECT

	PerscriptionPresenter* presenter{ nullptr };

	MedicationTableModel medModel;

	void dispensationLogic();

public:
	PerscriptionView(QWidget *parent = nullptr);
	void setPresenter(PerscriptionPresenter* p) override { presenter = p;}
	void setPatient(const Patient& patient, const Date& currentDate) override;
	void setMedicationList(const std::vector<std::string>) override;
	void setDispensation(const Dispensation& d) override;
	void setSupplements(const std::string& supplements) override;

	~PerscriptionView();

private:
	Ui::PerscriptionViewClass ui;





};
