#include "ProcedureTemplateDialog.h"


ProcedureTemplateDialog::ProcedureTemplateDialog(const ProcedureTemplate* pTemp, QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);



	connect(ui.okButton, &QPushButton::clicked,
		[=] {

			ProcedureTemplate result;

			result.code = ui.codeEdit->value();
			result.name = ui.nameEdit->getText();
			result.type = static_cast<ProcedureTemplateType>(ui.typeEdit->currentIndex());
			result.material = ui.materialEdit->getText();
			result.price = ui.priceEdit->value();
			result.diagnosis = ui.materialEdit->getText();
			result.nzok = false;

			m_procedureTemplate.emplace(result);

			this->close();
		});

	connect(ui.cancelButton, &QPushButton::clicked,
		[=] {
			this->close();
		});

	if (pTemp == nullptr) {
		return;
	}

	//edit mode
	ui.nameEdit->set_Text(pTemp->name);
	ui.priceEdit->setValue(pTemp->price);
	ui.codeEdit->setValue(pTemp->code);
	ui.diagnosisEdit->set_Text(pTemp->diagnosis);
	ui.materialEdit->set_Text(pTemp->material);
	ui.typeEdit->setCurrentIndex(static_cast<int>(pTemp->type));

}

std::optional<ProcedureTemplate> ProcedureTemplateDialog::getProcedureTemplate()
{
	return m_procedureTemplate;
}

ProcedureTemplateDialog::~ProcedureTemplateDialog()
{
}
