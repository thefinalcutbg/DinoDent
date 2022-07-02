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

SettingsDialog::~SettingsDialog()
{
}
