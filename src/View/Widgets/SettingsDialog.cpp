#include "SettingsDialog.h"


SettingsDialog::SettingsDialog(QDialog*parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle(u8"Настройки");
	setWindowIcon(QIcon(":/icons/icon_settings.png"));
	
	connect(ui.cancelButton, &QPushButton::clicked, [&] {close();});
	connect(ui.applyButton, &QPushButton::clicked, [&] {presenter.applyChanges();});
	connect(ui.okButton, &QPushButton::clicked, [&] {presenter.okPressed();});
	connect(ui.generalSettings->ui.updateMedButton, &QPushButton::clicked, [&] {presenter.updateMedications();});
	connect(ui.generalSettings->ui.updateMkbButton, &QPushButton::clicked, [&] {presenter.updateMkb(); });
	ui.practiceSettings->hidePassword();

	presenter.setView(this);
}


void SettingsDialog::setSettings(const Settings& settings)
{
	ui.generalSettings->ui.autoNraCheck->setChecked(settings.getNraStatusAuto);
	ui.generalSettings->ui.autoPisCheck->setChecked(settings.getPisHistoryAuto);
}

Settings SettingsDialog::getSettings()
{
	return Settings{
		ui.generalSettings->ui.autoPisCheck->isChecked(),
		ui.generalSettings->ui.autoNraCheck->isChecked()
	};
}

void SettingsDialog::setUpdateDate(DynamicNum num, const Date& date)
{
	QString dateStr = date.isDefault() ? u8"Няма" : date.toBgStandard(true).c_str();

	switch (num)
	{
	case DynamicNum::Medication:
		ui.generalSettings->ui.medUpdateLabel->setText(dateStr);
		break;
	case DynamicNum::MKB:
		ui.generalSettings->ui.mkbUpdateLabel->setText(dateStr);
		break;
	}
}

SettingsDialog::~SettingsDialog()
{
}
