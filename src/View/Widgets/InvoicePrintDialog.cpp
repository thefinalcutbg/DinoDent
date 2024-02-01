#include "InvoicePrintDialog.h"
#include <QPainter>


InvoicePrintDialog::InvoicePrintDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Принтиране на фактура");

	connect(ui.okButton, &QPushButton::clicked, [&] {
			
		m_result = ui.originalRadio->isChecked() ?
			Result::Original
			:
			Result::Duplicate;

		close();
			
	});
}

void InvoicePrintDialog::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
	painter.fillRect(0, 0, width(), height(), Qt::white);
}

InvoicePrintDialog::~InvoicePrintDialog()
{}
