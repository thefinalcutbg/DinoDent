#include "SettingsDialog.h"
#include <QPainter>


SettingsDialog::SettingsDialog(QDialog*parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Настройки");
	setWindowIcon(QIcon(":/icons/icon_settings.png"));
	
	connect(ui.cancelButton, &QPushButton::clicked, [&] {close();});
	connect(ui.okButton, &QPushButton::clicked, [&] {presenter.okPressed();});
	connect(ui.updateMedButton, &QPushButton::clicked, [&] {presenter.updateMedications();});
	ui.practiceSettings->hidePassword();

	presenter.setView(this);
}


void SettingsDialog::setSettings(const Settings& settings)
{
	ui.autoHisCheck->setChecked(settings.getHisHistoryAuto);
	ui.autoNraCheck->setChecked(settings.getNraStatusAuto);
	ui.autoPisCheck->setChecked(settings.getPisHistoryAuto);
	ui.timeoutSpin->setValue(settings.timeout);
}

Settings SettingsDialog::getSettings()
{
	return Settings{
		.getHisHistoryAuto = ui.autoHisCheck->isChecked(),
		.getPisHistoryAuto = ui.autoPisCheck->isChecked(),
		.getNraStatusAuto = ui.autoNraCheck->isChecked(),
		.timeout = ui.timeoutSpin->value()
	};
}

void SettingsDialog::setUpdateDate(DynamicNum num, const Date& date)
{
	QString dateStr = date.isDefault() ? "Няма" : date.toBgStandard(true).c_str();

	switch (num)
	{
	case DynamicNum::Medication:
		ui.medUpdateLabel->setText(dateStr);
		break;
	}
}

void SettingsDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::white);
}

SettingsDialog::~SettingsDialog()
{
}
