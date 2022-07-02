#pragma once

#include <QDialog>
#include "ui_SettingsDialog.h"
#include "Presenter/SettingsPresenter/SettingsMainPresenter.h"
#include "ISettingsDialog.h"

class SettingsDialog : public QDialog, public ISettingsDialog
{
	Q_OBJECT

	SettingsMainPresenter presenter;

public:
	SettingsDialog(QDialog *parent = Q_NULLPTR);

	void focusTab(SettingsTab tab) { ui.tabWidget->setCurrentIndex(static_cast<int>(tab)); };
	void closeDialog() override { this->close(); };
	IPracticeSettings* practiceView() override { return ui.practiceSettings; };
	IPriceListSettings* priceListView() override { return ui.priceSettings; };
	IPracticeDoctorSettings* practiceDoctorView() override { return ui.practiceDoctorSettings; };
	void setSettings(const Settings& settings) override;
	Settings getSettings() override;

	~SettingsDialog();

private:
	Ui::SettingsDialog ui;
};
