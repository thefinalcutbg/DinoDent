#pragma once

#include <QWidget>
#include "ui_PracticeSettings.h"
#include "IPacticeSettings.h"
#include "Presenter/SettingsPresenter/PracticeSettingsPresenter.h"

class PracticeSettings : public QWidget, public IPracticeSettings
{
	Q_OBJECT

	PracticeSettingsPresenter presenter;

	static constexpr int lineEditCount = 8;
	std::array<AbstractLineEdit*, lineEditCount> lineEdits;

public:
	PracticeSettings(QWidget *parent = Q_NULLPTR);
	~PracticeSettings();

private:
	Ui::PracticeSettings ui;

	void setPractice(const Practice& practice) override;
	Practice getPractice() override;
	AbstractLineEdit* lineEdit(PracticeTextFields::Field field) override;
	AbstractDateEdit* dateEdit() override;
	int legalForm() override;
};
