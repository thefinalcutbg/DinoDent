#pragma once

#include <QDialog>
#include "ui_InputDialog.h"
#include "Model/Validators/CommonValidators.h"

class InputDialog : public QDialog
{
	Q_OBJECT

	NotEmptyValidator validator;
	bool ok_pressed{ false };

	void paintEvent(QPaintEvent* e) override;

public:
	InputDialog(bool password, QWidget *parent = nullptr);

	void setLabel(const std::string& label);
	void setTitle(const std::string& title);

	//returns empty string if canceled
	std::string result();
	~InputDialog();

private:
	Ui::InputDialogClass ui;
};
