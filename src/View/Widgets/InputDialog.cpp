#include "InputDialog.h"


void InputDialog::setLabel(const std::string& label)
{
	ui.label->setText(label.c_str());
}

void InputDialog::setTitle(const std::string& title)
{
	setWindowTitle(title.c_str());
}

InputDialog::InputDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.textEdit->setInputValidator(&validator);

	connect(ui.okButton, &QPushButton::clicked, [&] { 

			if (ui.textEdit->validateInput()) {
				ok_pressed = true;
				close();
			}
			else
			{
				ui.textEdit->setFocus();
			}
		});

	connect(ui.cancelButton, &QPushButton::clicked, [&]{ close(); });
}

std::string InputDialog::result()
{
	if (ok_pressed) return ui.textEdit->getText();

	return std::string{};
}

InputDialog::~InputDialog()
{}
