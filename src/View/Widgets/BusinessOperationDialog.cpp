#include "BusinessOperationDialog.h"
#include <QPainter>

BuisnessOperationDialog::BuisnessOperationDialog(const BusinessOperation& op)
	: QDialog(nullptr)
{
	ui.setupUi(this);

	setWindowTitle("Редактиране");

	ui.nameEdit->setText(op.activity_name.c_str());
	ui.codeEdit->setText(op.activity_code.c_str());
	ui.priceSpinBox->setValue(op.unit_price);
	ui.quantitySpinBox->setValue(op.quantity);

	ui.nameEdit->setInputValidator(&notEmptyValidator);

    connect(ui.cancelButton, &QPushButton::clicked, this, [&] { reject(); });
    connect(ui.okButton, &QPushButton::clicked, this, [&]
         {

			ui.nameEdit->validateInput();

			if (!ui.nameEdit->isValid()) return;

			auto price = ui.priceSpinBox->value();
			auto quantity = ui.quantitySpinBox->value();

			m_operation.emplace(
				ui.codeEdit->text().toStdString(),
				ui.nameEdit->text().toStdString(),
				price,
				quantity
			);

			accept();
		}
	);
}

BuisnessOperationDialog::BuisnessOperationDialog() : QDialog(nullptr)
{
	ui.setupUi(this);

	setWindowTitle("Добавяне на услуга");

	ui.nameEdit->setInputValidator(&notEmptyValidator);

    connect(ui.cancelButton, &QPushButton::clicked, this, [&]{ reject(); });
    connect(ui.okButton, &QPushButton::clicked, this, [&]
        {

			ui.nameEdit->validateInput();

			if (!ui.nameEdit->isValid()) return;

			auto price = ui.priceSpinBox->value();
			auto quantity = ui.quantitySpinBox->value();

			m_operation.emplace(
				ui.codeEdit->text().toStdString(),
				ui.nameEdit->text().toStdString(),
				price,
				quantity
			);

			accept();
		}
	);
}


void BuisnessOperationDialog::paintEvent(QPaintEvent* )
{
    QPainter painter(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
}


BuisnessOperationDialog::~BuisnessOperationDialog()
{
}
