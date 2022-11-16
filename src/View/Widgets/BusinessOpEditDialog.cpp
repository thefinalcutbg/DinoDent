#include "BusinessOpEditDialog.h"
#include <QPainter>
BusinessOpEditDialog::BusinessOpEditDialog(const BusinessOperation& op, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Редактиране");

	ui.nameEdit->setText(op.activity_name.c_str());
	ui.codeSpinBox->setValue(std::stoi(op.activity_code));
	ui.priceSpinBox->setValue(op.unit_price);
	ui.quantitySpinBox->setValue(op.quantity);

	connect(ui.cancelButton, &QPushButton::clicked, [=] { reject(); });
	connect(ui.okButton, &QPushButton::clicked,
		[=] {

			auto price = ui.priceSpinBox->value();
			auto quantity = ui.quantitySpinBox->value();

			m_operation.emplace(
				std::to_string(ui.codeSpinBox->value()),
				ui.nameEdit->text().toStdString(),
				price,
				quantity
			);

			accept();
		}
	);
}


void BusinessOpEditDialog::paintEvent(QPaintEvent* e)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	painter.end();
}


BusinessOpEditDialog::~BusinessOpEditDialog()
{
}
