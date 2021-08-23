#include "DentistMadeWidget.h"

DentistMadeWidget::DentistMadeWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.madeByCheck, &QCheckBox::stateChanged, [&] {emit checked(); });
}


void DentistMadeWidget::setData(const DentistData& data)
{
	QSignalBlocker blocker(ui.madeByCheck);
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
