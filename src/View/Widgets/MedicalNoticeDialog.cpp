#include "MedicalNoticeDialog.h"

#include <QPainter>

#include "View/ModalDialogBuilder.h"
#include "TableViewDialog.h"
#include "Model/FreeFunctions.h"

MedicalNoticeDialog::MedicalNoticeDialog(const MedicalNotice& notice)
{
	ui.setupUi(this);

	setWindowTitle("Медицинска бележка");

    connect(ui.mkbButton, &QPushButton::clicked, this, [&] {

			TableViewDialog d(m_mkbDental, 0, ui.mkbButton->text().toStdString());
			d.setWindowTitle("Международна класификация на болестите");
			d.exec();

			auto result = d.getResult();

			if (result.size()) {
				ui.mkbButton->setText(result.c_str());
			}
		}
	);

	//setter
	m_lrn = notice.lrn.empty() ? FreeFn::getUuid() : notice.lrn;
	m_nrn = notice.nrn;
	ui.issueDateEdit->set_Date(notice.issue_date);
	ui.fromDateEdit->set_Date(notice.from_date);
	ui.toDateEdit->set_Date(notice.to_date);
	ui.institutionEdit->setText(notice.institution.c_str());
	ui.notes->setPlainText(notice.note.c_str());
	ui.mkbButton->setText(notice.mkb.isValid() ? notice.mkb.code().c_str() : "Изберете");

	ui.institutionEdit->setInputValidator(&not_emptyValidator);
	ui.institutionEdit->setErrorLabel(ui.errorLabel);

    connect(ui.okButton, &QPushButton::clicked, this, [&] {
			
			if (m_nrn.size()) {
				ModalDialogBuilder::showMessage("Медицинската бележка е вече изпратена в НЗИС и не може да бъде коригирана");
				return;
			}

			//validations

			if (!ui.institutionEdit->validateInput())
			{
				ui.institutionEdit->setFocus();
				return;
			}

			ICD10 diagnosis = ui.mkbButton->text().toStdString();

			if (!diagnosis.isValid()){
				ModalDialogBuilder::showMessage("Моля, изберете диагноза!");
				return;
			}

			//object construction

			m_result.emplace();

			m_result->nrn = m_nrn;
			m_result->lrn = m_lrn;
			m_result->issue_date = ui.issueDateEdit->getDate();
			m_result->to_date = ui.toDateEdit->getDate();
			m_result->from_date = ui.fromDateEdit->getDate();
			m_result->institution = ui.institutionEdit->getText();
			m_result->note = ui.notes->toPlainText().toStdString();
			m_result->mkb = diagnosis;

			accept();
	
		}
	);
	
}

 
void MedicalNoticeDialog::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}

MedicalNoticeDialog::~MedicalNoticeDialog()
{}
