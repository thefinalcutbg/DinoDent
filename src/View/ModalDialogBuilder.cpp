#include "ModalDialogBuilder.h"

#include "View/Widgets/ProcedureDialog.h"

void ModalDialogBuilder::openDialog(ProcedureDialogPresenter* p)
{
	ProcedureDialog d(p);
	d.exec();
}

#include "View/Widgets/PatientFormDialog.h"

void ModalDialogBuilder::openDialog(PatientDialogPresenter* p)
{
	PatientFormDialog d(p);
	d.exec();
}

#include "View/Widgets/ProcedureEditDialog.h"

void ModalDialogBuilder::openDialog(ProcedureEditorPresenter* p)
{
	ProcedureEditDialog d(p);
	d.exec();
}

#include "View/Widgets/AllergiesDialog.h"

void ModalDialogBuilder::openDialog(AllergiesDialogPresenter* p)
{
	AllergiesDialog d(p);
	d.exec();
}

#include "View/Widgets/AmbListSelector.h"

void ModalDialogBuilder::openDialog(ListSelectorPresenter* p)
{
	AmbListSelector* d = new AmbListSelector(p);
	d->setAttribute(Qt::WA_DeleteOnClose);
	d->show();
}

#include "View/Widgets/DetailedStatus.h"

void ModalDialogBuilder::openDialog(DetailedStatusPresenter* p)
{
	DetailedStatus d(p);
	d.exec();

}

#include "View/Widgets/LoginView.h"

void ModalDialogBuilder::openDialog(LoginPresenter* p)
{
	LoginView d(p);
	d.exec();
}


#include "View/Widgets/PracticeDialog.h"

void ModalDialogBuilder::openDialog(PracticeDialogPresenter* p)
{
	PracticeDialog d(p);
	d.exec();
}

#include "View/Widgets/DoctorSettingsDialog.h"

void ModalDialogBuilder::openDialog(DoctorDialogPresenter* p)
{
	DoctorSettingsDialog d(p);
	d.exec();
}

#include "View/Widgets/MedicationDialog.h"

void ModalDialogBuilder::openDialog(MedicationPresenter* p)
{
	MedicationDialog d(p);
	d.exec();
}

#include "View/Widgets/DosageDialog.h"

void ModalDialogBuilder::openDialog(DosagePresenter* p)
{
	DosageDialog d(p);
	d.exec();
}

#include "View/Widgets/PracticeSelectorView.h"

void ModalDialogBuilder::openDialog(PracticeManagerPresenter* p)
{	
	PracticeSelectorView d(p);
	d.exec();
}



#include "View/Widgets/ProcedureTemplateDialog.h"

std::optional<ProcedureTemplate> ModalDialogBuilder::openProcedureTemplateDialog(const ProcedureTemplate* pTemp, int code)
{
	ProcedureTemplateDialog d(pTemp, code);
	d.exec();
	return d.getProcedureTemplate();
}


#include "View/Widgets/SaveDialog.h"

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

std::optional<std::string> ModalDialogBuilder::getMonthNotifFromFile()
{
	QString filePath = QFileDialog::getOpenFileName(nullptr, 
		u8"Изберете месечно известие", "", "XML files(*.xml)");

	if (filePath.isEmpty())
		return {};

	return filePath.toStdString();
}

#include "View/Widgets/ProcedurePrintSelectDialog.h"

std::optional<std::vector<Procedure>> ModalDialogBuilder::selectProcedures(const std::vector<Procedure>& procedures, SelectionPref s)
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

#include "View/Widgets/PISHistoryDialog.h"

bool ModalDialogBuilder::pisHistoryDialog(
	const std::vector<Procedure>& procedures, 
	const std::vector<std::string>& payment_status
)
{
	PISHistoryDialog d(procedures, payment_status);
	d.exec();

	return d.applyProcedures;
}

#include "View/Widgets/BusinessOpEditDialog.h"

std::optional<BusinessOperation> ModalDialogBuilder::editBusinessOperation(const BusinessOperation& op)
{
	BusinessOpEditDialog d(op);
	d.exec();
	return d.getResult();
}

#include "View/Widgets/BusinessOpAddDialog.h"

std::optional<BusinessOperation> ModalDialogBuilder::addBusinessOperation(const std::vector<ProcedureTemplate>& priceList)
{
	BusinessOpAddDialog d(priceList);
	d.exec();

	return d.getResult();
}

#include "View/Widgets/RecipientEditView.h"

std::optional<Recipient> ModalDialogBuilder::editRecipient(const Recipient& r)
{
	RecipientEditView d(r);
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

#include "View/Widgets/ReportErrorDialog.h"

void ModalDialogBuilder::showMultilineDialog(const std::string& errors)
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

#include "View/Widgets/SettingsDialog.h"

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

#include <QProcess>
void ModalDialogBuilder::openExplorer(const std::string& path)
{
	#ifdef _WIN32    //Code for Windows
		QProcess::startDetached("explorer.exe", { "/select,", QDir::toNativeSeparators(path.c_str()) });
	#elif defined(__APPLE__)    //Code for Mac
		QProcess::execute("/usr/bin/osascript", { "-e", "tell application \"Finder\" to reveal POSIX file \"" + path + "\"" });
		QProcess::execute("/usr/bin/osascript", { "-e", "tell application \"Finder\" to activate" });
	#endif

}

MonthNotifLoad ModalDialogBuilder::monthNotifLoadDialog()
{
	QMessageBox msg;

	msg.setText(u8"Зареждане на месечно известие от:");

	msg.setStandardButtons(QMessageBox::Yes | QMessageBox::YesAll | QMessageBox::No);
	msg.setButtonText(QMessageBox::Yes, u8"ПИС");
	msg.setButtonText(QMessageBox::YesAll, u8"XML файл");
	msg.setButtonText(QMessageBox::No, u8"Отказ");
	msg.setIcon(QMessageBox::Icon::Question);

	switch (msg.exec()) {
		case QMessageBox::Yes: return MonthNotifLoad::FromPIS;
		case QMessageBox::YesAll: return MonthNotifLoad::FromFile;
		default: return MonthNotifLoad::Rejected;
	}
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

#include "View/Widgets/PinPromptDialog.h"

std::string ModalDialogBuilder::pinPromptDialog(const std::string& pem)
{
	PinPromptDialog d(pem);
	d.exec();

	return d.getResult().toStdString();
}

#include "View/Widgets/KSMPDialog.h"

std::string ModalDialogBuilder::ksmpDialog(KsmpList& list, const std::string& preSelectCode)
{
	KSMPDialog d(list, preSelectCode);
	d.exec();


	return d.getResult();
}

#include "View/Widgets/PisDialog.h"

void ModalDialogBuilder::pisDialog(MonthNotifPresenter* presenter)
{
	PisDialog d;
	d.monthNotifView()->setPresenter(presenter);
	d.exec();
}

#include "View/Widgets/InputDialog.h"

std::string ModalDialogBuilder::inputDialog(const std::string& text, const std::string& title, bool asPassword)
{
	InputDialog d(asPassword);
	d.setLabel(text);
	d.setTitle(title);
	d.exec();
	return d.result();
}



