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

	void focusTab(SettingsTab tab);
	void disableTab(SettingsTab tab);
	void closeDialog() final { this->close(); };
	IPracticeSettings* practiceView() final { return ui.practiceSettings; };
	void setSettings(const Settings& settings) final;
	Settings getSettings() final;
	void setUpdateDate(DynamicNum num, const Date& date) final;
	virtual void setPkcs11List(const std::vector<std::string>& list) final;
	std::vector<std::string> getPkcs11List() final;
	void setDebug(bool showRequests, bool showReplies) final;
	bool showRequests() final;
	bool showReplies() final;
	~SettingsDialog();

private:
	Ui::SettingsDialog ui;
};
