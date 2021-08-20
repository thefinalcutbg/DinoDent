#include "DentistMadeWidget.h"

DentistMadeWidget::DentistMadeWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}



void DentistMadeWidget::setData(const DentistData& data)
{
	ui.madeByCheck->setText(QString::fromStdString(data.dentistName));
	ui.madeByCheck->setEnabled(data.isCurrentUser);
	ui.madeByCheck->setChecked(data.checked);
}

bool DentistMadeWidget::userChecked()
{
	return ui.madeByCheck->isChecked();
}

DentistMadeWidget::~DentistMadeWidget()
{
}
