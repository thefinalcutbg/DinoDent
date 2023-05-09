#include "SettingsDialog.h"


SettingsDialog::SettingsDialog(QDialog*parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Настройки");
	setWindowIcon(QIcon(":/icons/icon_settings.png"));
	
	connect(ui.cancelButton, &QPushButton::clicked, [&] {close();});
	connect(ui.okButton, &QPushButton::clicked, [&] {presenter.okPressed();});
	connect(ui.generalSettings->ui.updateMedButton, &QPushButton::clicked, [&] {presenter.updateMedications();});
	ui.practiceSettings->hidePassword();

	presenter.setView(this);
}


void SettingsDialog::setSettings(const Settings& settings)
{
	ui.generalSettings->ui.autoHisCheck->setChecked(settings.getHisHistoryAuto);
	ui.generalSettings->ui.autoNraCheck->setChecked(settings.getNraStatusAuto);
	ui.generalSettings->ui.autoPisCheck->setChecked(settings.getPisHistoryAuto);
}

Settings SettingsDialog::getSettings()
{
	return Settings{
		.getHisHistoryAuto = ui.generalSettings->ui.autoHisCheck->isChecked(),
		.getPisHistoryAuto = ui.generalSettings->ui.autoPisCheck->isChecked(),
		.getNraStatusAuto = ui.generalSettings->ui.autoNraCheck->isChecked()
	};
}

void SettingsDialog::setUpdateDate(DynamicNum num, const Date& date)
{
	QString dateStr = date.isDefault() ? "Няма" : date.toBgStandard(true).c_str();

	switch (num)
	{
	case DynamicNum::Medication:
		ui.generalSettings->ui.medUpdateLabel->setText(dateStr);
		break;
	}
}

SettingsDialog::~SettingsDialog()
{
}
