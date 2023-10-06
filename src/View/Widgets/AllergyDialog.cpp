#include "AllergyDialog.h"
#include "View/ModalDialogBuilder.h"

AllergyDialog::AllergyDialog(const Allergy& a) : QDialog(nullptr)
{
	ui.setupUi(this);

	setWindowTitle("Алергия");

	connect(ui.dateCheck, &QCheckBox::stateChanged, this, [&](bool checked) { ui.dateEdit->setDisabled(!checked); });

	ui.dateEdit->setDate(QDate::currentDate());

	ui.descriptionEdit->setText(a.description.c_str());
	ui.categoryCombo->setCurrentIndex(a.category);
	ui.clinicalCombo->setCurrentIndex(a.clinicalStatus);
	ui.verificationCombo->setCurrentIndex(a.verificationStatus);
	ui.dateCheck->setChecked(a.lastOccurance.has_value());

	a.type ? ui.allergyRadio->setChecked(true) : ui.intoleranceRadio->setChecked(true);

	switch (a.criticality)
	{
		case Allergy::Low: ui.lowRiskRadio->setChecked(true); break;
		case Allergy::High: ui.highRiskRadio->setChecked(true); break;
		case Allergy::UnableToAsses: ui.undifinedRiskRadio->setChecked(true); break;
	}

	if (a.lastOccurance) ui.dateEdit->setDate(QDate(a.lastOccurance->year, a.lastOccurance->month, a.lastOccurance->day));

	//getting result
	connect(ui.okButton, &QPushButton::clicked, this, [&]{

			if (ui.descriptionEdit->text().isEmpty()) {
				ModalDialogBuilder::showMessage("Моля, въведете описание на алергията");
				return;
			}

			m_result.emplace();

			m_result->description = ui.descriptionEdit->text().toStdString();
			m_result->type = ui.allergyRadio->isChecked() ? Allergy::Type::allergy : Allergy::Type::intolerance;
			m_result->category = static_cast<Allergy::Category>(ui.categoryCombo->currentIndex());
			m_result->clinicalStatus = static_cast<Allergy::ClinicalStatus>(ui.clinicalCombo->currentIndex());
			m_result->verificationStatus = static_cast<Allergy::VerificationStatus>(ui.verificationCombo->currentIndex());
			
			if (ui.lowRiskRadio->isChecked()) m_result->criticality = Allergy::Low;
			else if (ui.highRiskRadio->isChecked()) m_result->criticality = Allergy::High;
			else m_result->criticality = Allergy::UnableToAsses;

			if (ui.dateCheck->isChecked()) {
				auto d = ui.dateEdit->date();

				m_result->lastOccurance.emplace(Date{ d.day(), d.month(), d.year() });
			}

			close();
		}
	);

}

AllergyDialog::~AllergyDialog()
{}
