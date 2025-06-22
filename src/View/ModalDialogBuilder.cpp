#include "ModalDialogBuilder.h"

#include "View/Widgets/ProcedureDialog.h"

#include <QScreen>
#include <QWidget>
#include <QGuiApplication>
#include <QProcess>

void ModalDialogBuilder::openDialog(ProcedureDialogPresenter& p)
{
	ProcedureDialog d(p);
	d.exec();
}

#include "View/Widgets/PatientFormDialog.h"

void ModalDialogBuilder::openDialog(PatientDialogPresenter& p)
{
	PatientFormDialog d(p);
	d.exec();
}

#include "View/Widgets/ProcedureEditDialog.h"

void ModalDialogBuilder::openDialog(ProcedureEditorPresenter& p)
{
	ProcedureEditDialog d(p);
	d.exec();
}

#include "View/Widgets/BrowserDialog.h"

void ModalDialogBuilder::openBrowserDialog()
{
    BrowserDialog d;
	d.exec();
}

#include "View/Widgets/DetailedStatus.h"

void ModalDialogBuilder::openDialog(DetailedStatusPresenter& p)
{
	DetailedStatus d(p);
	d.exec();

}

#include "View/Widgets/LoginView.h"

void ModalDialogBuilder::openDialog(LoginPresenter& p)
{
	LoginView d(p);
	d.exec();
}

#include "View/Widgets/DoctorSettingsDialog.h"

void ModalDialogBuilder::openDialog(DoctorDialogPresenter& p)
{
    DoctorSettingsDialog d(p);
	d.exec();
}

#include "View/Widgets/MedicationDialog.h"

void ModalDialogBuilder::openDialog(MedicationPresenter& p)
{
    MedicationDialog d(p);
	d.exec();
}

#include "View/Widgets/DosageDialog.h"

void ModalDialogBuilder::openDialog(DosagePresenter& p)
{
    DosageDialog d(p);
	d.exec();
}

#include "View/Widgets/PracticeSelectorView.h"

void ModalDialogBuilder::openDialog(PracticeManagerPresenter& p)
{	
	PracticeSelectorView d(p);
	d.exec();
}

#include "View/Widgets/ReferralDialog.h"

void ModalDialogBuilder::openDialog(ReferralPresenter& p)
{
	ReferralDialog d(p);

	d.exec();
}

#include "View/Widgets/StatisticDialogView.h"

void ModalDialogBuilder::openDialog(StatisticDialogPresenter& p)
{
	StatisticDialogView d(p);
	d.exec();
}

#include "View/Widgets/FiscalReceiptDialog.h"

void ModalDialogBuilder::openDialog(FiscalReceiptPresenter& p)
{
	FiscalReceiptDialog d(p);
	d.exec();
}

#include "View/Widgets/SaveDialog.h"

DialogAnswer ModalDialogBuilder::YesNoCancelDailog(const std::string& title)
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

#include "View/Widgets/ProcedurePrintSelectDialog.h"

std::optional<std::vector<Procedure>> ModalDialogBuilder::selectProcedures(const std::vector<Procedure>& procedures, FinancingSource source)
{
	std::vector<Procedure> result;

	ProcedurePrintSelectDialog dialog(procedures);

	dialog.selectFinancingSource(source);

	if (dialog.exec() == QDialog::Rejected) {
		return {};
	}

	auto selectedIndexes = dialog.selectedProcedures();

	for (auto idx : selectedIndexes) {
		result.push_back(procedures[idx]);
	}

	return result;
}

#include <QFileDialog>
/*
std::optional<std::string> ModalDialogBuilder::getMonthNotifFromFile()
{
	QString filePath = QFileDialog::getOpenFileName(nullptr, 
		"Изберете месечно известие", "", "XML files(*.xml)");

	if (filePath.isEmpty())
		return {};
	return filePath.toStdString();
}
*/

#include "View/Widgets/BusinessOperationDialog.h"

std::optional<BusinessOperation> ModalDialogBuilder::editBusinessOperation(const BusinessOperation& op)
{
	BuisnessOperationDialog d(op);
	d.exec();
	return d.getResult();
}

std::optional<BusinessOperation> ModalDialogBuilder::addBusinessOperation()
{
	BuisnessOperationDialog d;
	d.exec();

	return d.getResult();
}

#include "View/Widgets/MedicalNoticeDialog.h"
std::optional<MedicalNotice> ModalDialogBuilder::openDialog(const MedicalNotice& notice)
{
	MedicalNoticeDialog d(notice);
	d.exec();
	return d.getResult();
}

#include "View/SubWidgets/SnapshotViewer.h"
void ModalDialogBuilder::showSnapshots(const std::vector<HisSnapshot>& snapshots)
{
	QDialog d;
	d.setWindowTitle("Резултат от лечение");
	QVBoxLayout* mainLayout = new QVBoxLayout(&d);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	d.setLayout(mainLayout);
	SnapshotViewer* v = new SnapshotViewer(&d);
	mainLayout->addWidget(v);
	v->setSnapshots(snapshots);
	d.exec();
}

bool ModalDialogBuilder::askDialog(const std::string& questionText)
{
	QMessageBox msg;

	msg.setWindowTitle("DinoDent");

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

#include "View/Widgets/MultilineDialog.h"

std::optional<std::string> ModalDialogBuilder::showMultilineDialog(const std::string& text, const std::string& title, bool enableEdit)
{
	MultilineDialog d(text);

	if (enableEdit) {
		d.enableEditing();
	}

	d.setWindowTitle(title.c_str());
	d.exec();

	return d.getResult();
}

void ModalDialogBuilder::showMessage(const std::string& message)
{
	QMessageBox msgBox;
	msgBox.setModal(true);
	msgBox.setText(QString::fromStdString(message));
	msgBox.exec();
}

#include "View/Widgets/SettingsDialog.h"

void ModalDialogBuilder::openSettingsDialog(SettingsTab tab)
{
	SettingsDialog d;
	d.focusTab(tab);
	d.exec();
}

#include "View/Widgets/ButtonDialog.h"

int ModalDialogBuilder::openButtonDialog(const std::vector<std::string>& buttonNames, const std::string& title, const std::string& description)
{
	ButtonDialog d(buttonNames, title, description);

	d.exec();

	return d.getResult();
}
/*
std::optional<std::string> ModalDialogBuilder::getFileNamePath(const std::string& filename)
{	
	QString dirPath = QFileDialog::getSaveFileName(nullptr, "Запазване на финансовия документ",
		QString::fromStdString(filename),
		"XML (*xml)");
		
	if (dirPath.isEmpty())
		return {};

	return dirPath.toStdString();


}
*/
void ModalDialogBuilder::saveFile(const std::string& data, const std::string& filename)
{
	
	QString dirPath = QFileDialog::getSaveFileName(nullptr, "Запазване на финансовия документ",
		QString::fromStdString(filename),
		"XML (*xml)");
		
	if (dirPath.isEmpty()) return;

	QFile file(dirPath);

	//Open the file
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

	QTextStream out(&file);

	out << data.c_str();

	file.close();

	openExplorer(dirPath.toStdString());
}

std::optional<std::string> ModalDialogBuilder::openFile()
{
	QString filePath = QFileDialog::getOpenFileName(nullptr,
		"Изберете месечно известие", "", "XML files(*.xml)");

	if (filePath.isEmpty())
		return {};

	QFile file(filePath);

	//Open the file
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return {};

	QString result = file.readAll();

	file.close();

	return result.toStdString();
}

std::vector<std::string> ModalDialogBuilder::openFiles()
{

	QStringList pathList = QFileDialog::getOpenFileNames(nullptr,
		"Изберете XML файлове", "", "XML files(*.xml)");

	if (pathList.isEmpty())
		return {};

	std::vector<std::string> result;

	for (auto& filePath : pathList)
	{
		QFile file(filePath);
		//Open the file
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;

		QString contents = file.readAll();

		file.close();

		result.push_back(contents.toStdString());
	}

	return result;
}

std::optional<std::string> ModalDialogBuilder::getFilePath(const std::string& filename)
{
	QString dirPath = QFileDialog::getSaveFileName(nullptr, "Запазване на фискалния опис",
		QString::fromStdString(filename),
		"PDF (*pdf)");

	if (dirPath.isEmpty()) return{};

	return dirPath.toStdString();
}

#include "View/Widgets/ErrorDetailedDialog.h"

void ModalDialogBuilder::showDetailedError(const std::string& descr, const std::string& details)
{
	ErrorDetailedDialog d(descr, details);
	d.exec();
}

#include "View/Widgets/AllergyDialog.h"

std::optional<Allergy> ModalDialogBuilder::openAllergyDialog(const Allergy& a)
{
	AllergyDialog d(a);
	d.exec();

	return d.getResult();
}

#include "View/Widgets/ICD10Dialog.h"

ICD10 ModalDialogBuilder::icdDialog(const ICD10& icd)
{
	ICD10Dialog d(icd);
	d.exec();
	return d.getResult();
}

void ModalDialogBuilder::openExplorer(const std::string& path)
{
	#ifdef _WIN32    //Code for Windows
		QProcess::startDetached("explorer.exe", { "/select,", QDir::toNativeSeparators(path.c_str()) });
	#elif defined(__APPLE__)    //Code for Mac
        QString pathstr = "tell application \"Finder\" to reveal POSIX file \"";
        pathstr += path;
        pathstr += "\"" ;
        QProcess::execute("/usr/bin/osascript", { "-e", pathstr });
		QProcess::execute("/usr/bin/osascript", { "-e", "tell application \"Finder\" to activate" });
    #endif
        Q_UNUSED(path)
        return;
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

std::string ModalDialogBuilder::pinPromptDialog(const std::string& pem, const std::string& driver)
{
	PinPromptDialog d(pem, driver);
	d.exec();

	return d.getResult().toStdString();
}

#include "View/Widgets/PisDialog.h"

void ModalDialogBuilder::pisDialog(MonthNotifPresenter* presenter)
{
	PisDialog d;
	d.monthNotifView()->setPresenter(presenter);
	d.exec();
}

void ModalDialogBuilder::displayPixmap(const QPixmap& px)
{
	auto dlg = new QDialog;

	auto label = new QLabel(dlg);
	label->setPixmap(px);
	label->setScaledContents(true);
	label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	auto lay = new QVBoxLayout(dlg);
	lay->addWidget(label);
	lay->setContentsMargins(0, 0, 0, 0);

	dlg->resize(px.size());
	dlg->exec();
}

#include "View/Widgets/InputDialog.h"

std::string ModalDialogBuilder::inputDialog(
	const std::string& text, 
	const std::string& title, 
	const std::string& input, 
	bool asPassword, 
	bool emptyNotAllowed
)
{
	InputDialog d(asPassword);
	d.setLabel(text);
	d.setTitle(title);
	d.setInput(input);
	d.enableNotEmptyValidator(emptyNotAllowed);
	d.exec();
	return d.result();
}



