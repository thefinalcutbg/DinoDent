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
#include "View/DoctorSettingsDialog/DoctorSettingsDialog.h"

void ModalDialogBuilder::openDialog(DoctorDialogPresenter* p)
{
	DoctorSettingsDialog d(p);
	d.exec();
}

#include "View/InvoiceDialog/InvoiceDialog.h"

void ModalDialogBuilder::openDialog(InvoicePresenter* p)
{
	InvoiceDialog d(p);
	d.exec();
}

#include "View/saveAsDialog/SaveAsDialog.h"

int ModalDialogBuilder::saveAsAmbSheetNumber(int newNum, std::unordered_set<int> existingNumbers)
{
	SaveAsDialog d(existingNumbers, newNum);
	return d.exec();
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
	msgBox.setText(QString::fromStdString(message));
	msgBox.exec();
}

#include "View/SettingsDialog/SettingsDialog.h"

void ModalDialogBuilder::openSettingsDialog()
{
	SettingsDialog d;
	d.exec();
}
