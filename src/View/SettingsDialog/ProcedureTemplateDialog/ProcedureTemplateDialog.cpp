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

	//new procedure mode:
	if (pTemp == nullptr) {

		procedureType[0]->setChecked(true);
		ui.codeEdit->setValue(code);
		ui.ksmpCheck->setChecked(false);
		ui.ksmpButton->setHidden(true);
		ui.ksmpButton->setText(
			KSMP::getByType(ProcedureType::general)
				.at(0)->code.c_str()
		);
	}
	//edit mode:
	else
	{

		ui.nameEdit->set_Text(pTemp->name);
		ui.priceEdit->setValue(pTemp->price);
		ui.codeEdit->setValue(pTemp->code);
		ui.diagnosisEdit->set_Text(pTemp->diagnosis);
		ui.materialEdit->set_Text(pTemp->material);
		procedureType[static_cast<int>(pTemp->type)]->toggle();

		bool ksmp = !pTemp->ksmp.empty();
		ui.ksmpCheck->setChecked(ksmp);
		ui.ksmpButton->setHidden(!ksmp);

		ui.ksmpButton->setText(
			ksmp ? 
			pTemp->ksmp.c_str() 
			:
			KSMP::getByType(pTemp->type).at(0)->code.c_str()
		);

		
	}


	for (int i =0; i<9; i++)
	{

		connect(procedureType[i], &QRadioButton::toggled, [=]
			{
				auto& ksmp_ptr = KSMP::getByType(static_cast<ProcedureTemplateType>(i)).at(0);
				ui.ksmpButton->setText(ksmp_ptr->code.c_str());
				if (ui.ksmpCheck->isChecked())
				{
					ui.nameEdit->setText(KSMP::getName(ui.ksmpButton->text().toStdString()).c_str());
				}
				
				currentType = static_cast<ProcedureTemplateType>(i);
			});
	}

	ui.nameEdit->setInputValidator(&m_validator);
	
	connect(ui.ksmpCheck, &QCheckBox::clicked, [=](bool checked) {
				
				ui.ksmpButton->setHidden(!checked);

				if (checked) {
					ui.nameEdit->setText(KSMP::getName(ui.ksmpButton->text().toStdString()).c_str());
				}


				
		});

	connect(ui.ksmpButton, &QPushButton::clicked,
		[=] {

			auto result = ModalDialogBuilder::ksmpDialog(KSMP::getByType(currentType), ui.ksmpButton->text().toStdString());
			
			if (result.empty()) return;

			ui.ksmpButton->setText(result.c_str());
			ui.nameEdit->setText(KSMP::getName(result).c_str());
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
			result.ksmp = ui.ksmpCheck->isChecked() ? ui.ksmpButton->text().toStdString() : "";
			m_procedureTemplate.emplace(result);

			this->close();
		});

	connect(ui.cancelButton, &QPushButton::clicked,
		[=] {
			this->close();
		});

	ui.nameEdit->setFocus();


}

std::optional<ProcedureTemplate> ProcedureTemplateDialog::getProcedureTemplate()
{
	return m_procedureTemplate;
}

ProcedureTemplateDialog::~ProcedureTemplateDialog()
{
}
