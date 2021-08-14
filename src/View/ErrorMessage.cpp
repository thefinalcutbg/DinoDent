#include "ErrorMessage.h"
#include <QMessageBox>

void showError(const std::string& error)
{
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setWindowTitle("Възникна грешка");
	msgBox.setText(QString::fromStdString(error));
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.exec();
}

bool questionDialog(const std::string& question)
{
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setText(QString::fromStdString(question));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setButtonText(QMessageBox::Yes, u8"Да");
	msgBox.setButtonText(QMessageBox::No, u8"Не");
	
	return msgBox.exec() == QMessageBox::Yes;
}
