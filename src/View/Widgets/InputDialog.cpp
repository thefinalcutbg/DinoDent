#include "InputDialog.h"
#include <QPainter> 

void InputDialog::setLabel(const std::string& label)
{
	ui.label->setText(label.c_str());
}

void InputDialog::setTitle(const std::string& title)
{
	setWindowTitle(title.c_str());
}

void InputDialog::setInput(const std::string& input)
{
	ui.lineEdit->setText(input.c_str());
}

void InputDialog::enableNotEmptyValidator(bool enable)
{
	ui.lineEdit->setInputValidator(enable ? &validator : nullptr);
}

InputDialog::InputDialog(bool password, QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	if (password) ui.lineEdit->setEchoMode(QLineEdit::Password);

	connect(ui.okButton, &QPushButton::clicked, [&] { 

			if (ui.lineEdit->validateInput()) {
				ok_pressed = true;
				close();
			}
			else
			{
				ui.lineEdit->setFocus();
			}
		
		});

	connect(ui.cancelButton, &QPushButton::clicked, [&]{ close(); });
}

std::string InputDialog::result()
{
	if (ok_pressed) return ui.lineEdit->getText();

	return std::string{};
}

void InputDialog::paintEvent(QPaintEvent* )
{
    QPainter painter(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
}

InputDialog::~InputDialog()
{}
