#pragma once

#include <QWidget>
#include "ui_PracticeSettings.h"
#include "View/Interfaces/IPacticeSettings.h"
#include "Presenter/PracticeSettingsPresenter.h"

class PracticeSettings : public QWidget, public IPracticeSettings
{
	Q_OBJECT

	PracticeSettingsPresenter* presenter{nullptr};

	static constexpr int lineEditCount = 10;
	std::array<AbstractLineEdit*, lineEditCount> lineEdits;

	void paintEvent(QPaintEvent* event) override;

public:
	PracticeSettings(QWidget *parent = Q_NULLPTR);
	~PracticeSettings();

	void setPractice(const Practice& practice) override;
	Practice getPractice() override;
	AbstractLineEdit* lineEdit(PracticeTextFields::Field field) override;
	AbstractDateEdit* dateEdit() override;
	int legalForm() override;
	void setPresenter(PracticeSettingsPresenter* presenter) override;
	void hidePassword() override;


private:
	Ui::PracticeSettings ui;


};