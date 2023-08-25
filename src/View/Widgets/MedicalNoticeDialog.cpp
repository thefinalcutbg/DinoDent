#include "MedicalNoticeDialog.h"

#include <QPainter>

#include "View/ModalDialogBuilder.h"
#include "TableViewDialog.h"
#include "Model/FreeFunctions.h"

MedicalNoticeDialog::MedicalNoticeDialog()
{
	ui.setupUi(this);

	setWindowTitle("Медицинска бележка");

	connect(ui.mkbButton, &QPushButton::clicked, [=] {

			TableViewDialog d(m_mkbDental, 0, ui.mkbButton->text().toStdString());
			d.setWindowTitle("Международна класификация на болестите");
			d.exec();

			auto result = d.getResult();

			if (result.size()) {
				ui.mkbButton->setText(result.c_str());
			}
		}
	);

	ui.fromDateEdit->set_Date(Date::currentDate());
	ui.toDateEdit->set_Date(Date::currentDate());
	ui.issueDateEdit->set_Date(Date::currentDate());

	ui.institutionEdit->setInputValidator(&not_emmptyValidator);
	ui.institutionEdit->setErrorLabel(ui.errorLabel);

	connect(ui.okButton, &QPushButton::clicked, [=] {
			
			//validations

			if (!ui.institutionEdit->validateInput())
			{
				ui.institutionEdit->setFocus();
				return;
			}

			MKB diagnosis = ui.mkbButton->text().toStdString();

			if (!diagnosis.isValid()){
				ModalDialogBuilder::showError("Моля, изберете диагноза!");
				return;
			}

			if (ui.locationCombo->currentIndex() == 1) {
				ModalDialogBuilder::showError("Мястото за лечение не може да е стационар!");
			}

			//object construction

			m_result.emplace();

			m_result->issue_date = ui.issueDateEdit->getDate();
			m_result->to_date = ui.toDateEdit->getDate();
			m_result->from_date = ui.fromDateEdit->getDate();
			m_result->institution = ui.institutionEdit->getText();
			m_result->note = ui.notes->toPlainText().toStdString();
			m_result->location = static_cast<MedicalNotice::Location>(ui.locationCombo->currentIndex() + 1);
			m_result->mkb = ui.mkbButton->text().toStdString();
			m_result->lrn = FreeFn::getUuid();

			accept();
	
		}
	);
	
}
 
void MedicalNoticeDialog::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}

MedicalNoticeDialog::~MedicalNoticeDialog()
{}
