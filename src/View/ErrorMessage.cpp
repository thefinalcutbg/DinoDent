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
