#pragma once

#include <QDialog>
#include "ui_SettingsDialog.h"
#include "Presenter/SettingsMainPresenter.h"
#include "View/Interfaces/ISettingsDialog.h"

class SettingsDialog : public QDialog, public ISettingsDialog
{
	Q_OBJECT

	SettingsMainPresenter presenter;

public:
	SettingsDialog(QDialog *parent = Q_NULLPTR);

	void focusTab(SettingsTab tab) { ui.tabWidget->setCurrentIndex(static_cast<int>(tab)); };
	void closeDialog() override { this->close(); };
	IPracticeSettings* practiceView() override { return ui.practiceSettings; };
	void setSettings(const Settings& settings) override;
	Settings getSettings() override;
	void setUpdateDate(DynamicNum num, const Date& date) override;
	~SettingsDialog();

private:
	Ui::SettingsDialog ui;
};
