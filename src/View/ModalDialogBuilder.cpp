#include "ModalDialogBuilder.h"

#include "View/ProcedureDialog/ProcedureDialog.h"

void ModalDialogBuilder::openDialog(ProcedureDialogPresenter* p)
{
	ProcedureDialog d(p);
	d.exec();
}

#include "View/PatientDialog/PatientFormDialog.h"

void ModalDialogBuilder::openDialog(PatientDialogPresenter* p)
{
	PatientFormDialog d(p);
	d.exec();
}

#include "View/ProcedureEditDialog/ProcedureEditDialog.h"

void ModalDialogBuilder::openDialog(ProcedureEditorPresenter* p)
{
	ProcedureEditDialog d(p);
	d.exec();
}

#include "View/AllergiesDialog/AllergiesDialog.h"

void ModalDialogBuilder::openDialog(AllergiesDialogPresenter* p)
{
	AllergiesDialog d(p);
	d.exec();
}

#include "View/AmbListSelector/AmbListSelector.h"

void ModalDialogBuilder::openDialog(ListSelectorPresenter* p)
{
	AmbListSelector* d = new AmbListSelector(p);
	d->setAttribute(Qt::WA_DeleteOnClose);
	d->show();
}

#include "View/DetailsView/DetailedStatus.h"

void ModalDialogBuilder::openDialog(DetailedStatusPresenter* p)
{
	DetailedStatus d(p);
	d.exec();

}

#include "View/LoginView/LoginView.h"

void ModalDialogBuilder::openDialog(LoginPresenter* p)
{
	LoginView d(p);
	d.exec();
}

#include <View/ReportDialog/ReportDialog.h>

void ModalDialogBuilder::openDialog(std::optional<ReportDialogResult>& result)
{
	ReportDialog d(result);
	d.exec();
}

#include "View/AddPracticeDialog/AddPracticeDialog.h"

void ModalDialogBuilder::openDialog(AddPracticePresenter* p)
{
	AddPracticeDialog d(p);
	d.exec();
}
#include "View/DoctorSettingsDialog/DoctorSettingsDialog.h"

void ModalDialogBuilder::openDialog(DoctorDialogPresenter* p)
{
	DoctorSettingsDialog d(p);
	d.exec();
}


#include "View/saveAsDialog/SaveAsDialog.h"

long long ModalDialogBuilder::saveAsDocNumber(long long newNum, std::unordered_set<int> existingNumbers, const std::string& docName, int numberCharLength)
{
	SaveAsDialog d(existingNumbers, newNum, docName);
	d.setMaxNumLength(numberCharLength);
	d.exec();
	return d.result();
}

#include "View/SaveAsDateDialog/SaveAsDateDialog.h"

std::optional<Date> ModalDialogBuilder::saveAsDate(const Date& date, const std::string& docName)
{
	SaveAsDateDialog d(date, docName);
	d.exec();

	return d.getResult();
}

#include "View/SettingsDialog/ProcedureTemplateDialog/ProcedureTemplateDialog.h"

std::optional<ProcedureTemplate> ModalDialogBuilder::openProcedureTemplateDialog(const ProcedureTemplate* pTemp, int code)
{
	ProcedureTemplateDialog d(pTemp, code);
	d.exec();
	return d.getProcedureTemplate();
}


#include "View/saveDialog/SaveDialog.h"

DialogAnswer ModalDialogBuilder::openSaveDialog(const std::string& title)
{
	SaveDialog s(QString::fromStdString(title));
	int answer = s.exec();

	switch (answer)
	{
	case QMessageBox::Yes:
		return DialogAnswer::Yes;
	case QMessageBox::No:
		return DialogAnswer::No;
	default:
		return DialogAnswer::Cancel;
	}
}

#include <QFileDialog>

std::optional<std::string> ModalDialogBuilder::getMonthlyNotification()
{
	QString filePath = QFileDialog::getOpenFileName(nullptr, 
		u8"Изберете месечно известие", "", "XML files(*.xml)");

	if (filePath.isEmpty())
		return {};

	return filePath.toStdString();
}

#include "Printer/ProcedurePrintSelectDialog.h"

std::optional<Procedures> ModalDialogBuilder::selectProcedures(const Procedures& procedures, SelectionPref s)
{
	std::vector<Procedure> result;

	{
		ProcedurePrintSelectDialog dialog(procedures);

		switch (s) {
			case SelectionPref::All: break; //everything in the dialog is selected by default;
			case SelectionPref::OnlyNZOK: dialog.selectOnlyWhereNzokIs(true); break;
			case SelectionPref::OnlyPaid: dialog.selectOnlyWhereNzokIs(false); break;
		}

		if (dialog.exec() == QDialog::Rejected) {
			return {};
		}

		auto selectedIndexes = dialog.selectedProcedures();

		for (auto idx : selectedIndexes) {
			result.push_back(procedures[idx]);
		}

	}

	return result;
}

#include "View/PISHistoryDialog/PISHistoryDialog.h"

bool ModalDialogBuilder::pisHistoryDialog(const Procedures& procedures)
{
	PISHistoryDialog d(procedures);
	d.exec();

	return d.applyProcedures;
}

#include "View/FinancialView/BusinessOpEditDialog/BusinessOpEditDialog.h"

std::optional<BusinessOperation> ModalDialogBuilder::editBusinessOperation(const BusinessOperation& op)
{
	BusinessOpEditDialog d(op);
	d.exec();
	return d.getResult();
}

#include "View/FinancialView/BusinessOpAddDialog/BusinessOpAddDialog.h"

std::optional<BusinessOperation> ModalDialogBuilder::addBusinessOperation(const std::vector<ProcedureTemplate>& priceList)
{
	BusinessOpAddDialog d(priceList);
	d.exec();

	return d.getResult();
}

bool ModalDialogBuilder::askDialog(const std::string& questionText)
{

	QMessageBox msg;

	msg.setWindowTitle("Torque");

	msg.setText(QString::fromStdString(questionText));
	msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
	msg.setButtonText(QMessageBox::Yes, "Да");
	msg.setButtonText(QMessageBox::No, "Не");
	msg.setIcon(QMessageBox::Question);

	return msg.exec() == QMessageBox::Yes;
}

void ModalDialogBuilder::showError(const std::string& error)
{

	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setWindowTitle("Възникна грешка");
	msgBox.setText(QString::fromStdString(error));
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.exec();

}

#include "View/ReportDialog/ReportErrorDialog/ReportErrorDialog.h"

void ModalDialogBuilder::showErrorList(const std::string& errors)
{
	ReportErrorDialog d(errors);
	d.exec();
}

void ModalDialogBuilder::showMessage(const std::string& message)
{
	QMessageBox msgBox;
	msgBox.setModal(true);
	msgBox.setText(QString::fromStdString(message));
	msgBox.exec();
}

#include "View/SettingsDialog/SettingsDialog.h"

void ModalDialogBuilder::openSettingsDialog()
{
	SettingsDialog d;
	d.exec();
}

std::optional<std::string> ModalDialogBuilder::getFileNamePath(const std::string& filename)
{	
	QString dirPath = QFileDialog::getSaveFileName(nullptr, u8"Запазване на финансовия документ",
		QString::fromStdString(filename),
		"XML (*xml)");
		
	if (dirPath.isEmpty())
		return {};

	return dirPath.toStdString();


}

#include <QDesktopServices>

void ModalDialogBuilder::openExplorer(const std::string& path)
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(path)));
}

#include <QInputDialog>

std::optional<std::string> ModalDialogBuilder::getStringInput(const std::string& dialogName, const std::string& fieldName)
{
	bool ok;

	QString text = QInputDialog::getText(NULL, dialogName.data(),
		fieldName.data(),
		QLineEdit::Password, QString(), &ok);

	if (text.isEmpty()) return {};

	return text.toStdString();

}
