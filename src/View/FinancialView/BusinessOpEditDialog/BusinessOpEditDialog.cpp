#include "BusinessOpEditDialog.h"

BusinessOpEditDialog::BusinessOpEditDialog(const BusinessOperation& op, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.nameEdit->setText(op.activity_name.c_str());
	ui.codeSpinBox->setValue(op.activity_code);
	ui.priceSpinBox->setValue(op.unit_price);
	ui.quantitySpinBox->setValue(op.quantity);

	connect(ui.cancelButton, &QPushButton::clicked, [=] { reject(); });
	connect(ui.okButton, &QPushButton::clicked,
		[=] {

			auto price = ui.priceSpinBox->value();
			auto quantity = ui.quantitySpinBox->value();

			m_operation.emplace(BusinessOperation{
				ui.codeSpinBox->value(),
				ui.nameEdit->text().toStdString(),
				price,
				quantity,
				price*quantity
			});

			accept();
		}
	);
}


BusinessOpEditDialog::~BusinessOpEditDialog()
{
}