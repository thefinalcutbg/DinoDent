#include "SaveDialog.h"

SaveDialog::SaveDialog(QString title, QWidget *parent)
	: QMessageBox(parent)
{
	setWindowTitle("DinoDent");

	setText(title);
	setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
	setButtonText(QMessageBox::Yes, "Да");
	setButtonText(QMessageBox::No, "Не");
	setButtonText(QMessageBox::Cancel, "Отказ");
	setIcon(QMessageBox::Warning);
}

SaveDialog::~SaveDialog()
{
}
