#include "BusinessOperationDialog.h"
#include <QPainter>
#include "Model/Dental/ProcedureCode.h"

QStringList s_completionList;

BuisnessOperationDialog::BuisnessOperationDialog(const std::optional<BusinessOperation>& op)
	: QDialog(nullptr)
{
	ui.setupUi(this);

	setWindowTitle("Добавяне на услуга");

	ui.nameEdit->setFocus();

	if (op.has_value()) {

		setWindowTitle("Редактиране");

		ui.nameEdit->setText(op->activity_name.c_str());
		ui.codeEdit->setText(op->activity_code.c_str());
		ui.nameEdit->setCursorPosition(0);
		ui.priceSpinBox->setValue(op->unit_price);
		ui.quantitySpinBox->setValue(op->quantity);
		ui.priceSpinBox->setFocus();
	}

	ui.nameEdit->setInputValidator(&notEmptyValidator);

	if (s_completionList.isEmpty()) {

		auto& procedures = ProcedureCode::getProcedures();
		s_completionList.reserve(procedures.size());

		for (auto p : procedures)
		{
			QString descr = QString::fromStdString(p.name());

			s_completionList.push_back(descr);
		}
	}

	ui.nameEdit->setCompletions(s_completionList);

	connect(ui.nameEdit, &QLineEdit::textChanged, this, [&](const QString& text) {

		auto pCode = ProcedureCode::fromName(text.toStdString());

		if (!pCode.isValid()) return;

		ui.codeEdit->setText(pCode.ACHICode().c_str());
	});

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

void BuisnessOperationDialog::paintEvent(QPaintEvent* )
{
    QPainter painter(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
}


BuisnessOperationDialog::~BuisnessOperationDialog()
{
}
