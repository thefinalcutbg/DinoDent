#include "ProcedureTemplateDialog.h"
#include "Model/KSMP.h"
#include "View/ModalDialogBuilder.h"
ProcedureTemplateDialog::ProcedureTemplateDialog(const ProcedureTemplate* pTemp, int code, QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	procedureType[0] = ui.radioButton;
	procedureType[1] = ui.radioButton_2;
	procedureType[2] = ui.radioButton_3;
	procedureType[3] = ui.radioButton_4;
	procedureType[4] = ui.radioButton_5;
	procedureType[5] = ui.radioButton_6;
	procedureType[6] = ui.radioButton_7;
	procedureType[7] = ui.radioButton_8;
	procedureType[8] = ui.radioButton_9;
	
	for (int i =0; i<9; i++)
	{

		connect(procedureType[i], &QRadioButton::clicked, [=]
			{
				auto& ksmp_ptr = KSMP::getByType(static_cast<ProcedureTemplateType>(i)).at(0);
				ui.ksmpButton->setText(ksmp_ptr->code.c_str());
				ui.ksmpLabel->setText(ksmp_ptr->name.c_str());
				currentType = static_cast<ProcedureTemplateType>(i);
			});
	}

	ui.nameEdit->setInputValidator(&m_validator);

	connect(ui.ksmpButton, &QPushButton::clicked,
		[=] {

			auto result = ModalDialogBuilder::ksmpDialog(KSMP::getByType(pTemp->type), ui.ksmpButton->text().toStdString());
			
			if (result.empty()) return;

			ui.ksmpButton->setText(result.c_str());
			ui.ksmpLabel->setText(KSMP::getByCode(result).name.c_str());
		}
		);

	connect(ui.okButton, &QPushButton::clicked,
		[=] {

			ui.nameEdit->validateInput();
			if (!ui.nameEdit->isValid()) {
				ui.nameEdit->QTextEdit::setFocus();
				return;
			}

			ProcedureTemplate result;

			result.code = ui.codeEdit->value();
			result.name = ui.nameEdit->getText();
			result.type = currentType;
			result.material = ui.materialEdit->getText();
			result.price = ui.priceEdit->value();
			result.diagnosis = ui.diagnosisEdit->getText();
			result.nzok = false;
			result.ksmp = ui.ksmpButton->text().toStdString();
			m_procedureTemplate.emplace(result);

			this->close();
		});

	connect(ui.cancelButton, &QPushButton::clicked,
		[=] {
			this->close();
		});

	ui.nameEdit->setFocus();

	if (pTemp == nullptr) {

		ui.codeEdit->setValue(code);
		procedureType[0]->click();
		return;
	}

	

	//edit mode
	ui.nameEdit->set_Text(pTemp->name);
	ui.priceEdit->setValue(pTemp->price);
	ui.codeEdit->setValue(pTemp->code);
	ui.diagnosisEdit->set_Text(pTemp->diagnosis);
	ui.materialEdit->set_Text(pTemp->material);
	procedureType[static_cast<int>(pTemp->type)]->click();

	if (pTemp->ksmp.empty()) return;

	auto ksmp = KSMP::getByCode(pTemp->ksmp);
	ui.ksmpButton->setText(ksmp.code.c_str());
	ui.ksmpLabel->setText(ksmp.name.c_str());

}

std::optional<ProcedureTemplate> ProcedureTemplateDialog::getProcedureTemplate()
{
	return m_procedureTemplate;
}

ProcedureTemplateDialog::~ProcedureTemplateDialog()
{
}
