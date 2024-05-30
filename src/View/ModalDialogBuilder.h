#pragma once
#include <string>
#include <vector>

#include "Model/Dental/ProcedureCode.h"

#include "Model/Financial/BusinessOperation.h"

#include "Model/MedicalNotice.h"
#include "Model/Allergy.h"

enum class SelectionPref {All, OnlyNZOK, OnlyPaid};
enum class MonthNotifLoad {FromPIS, FromFile, Rejected};

class ProcedureDialogPresenter;
class PatientDialogPresenter;
class ProcedureEditorPresenter;
class AllergiesDialogPresenter;
class BrowserPresenter;
class DetailedStatusPresenter;
class LoginPresenter;
class DoctorDialogPresenter;
class InvoicePresenter;

class MonthNotifPresenter;
class MedicationPresenter;
class DosagePresenter;
class PracticeManagerPresenter;
class ReferralPresenter;
class StatisticDialogPresenter;
class ProcedureHistoryPresenter;
class FiscalReceiptPresenter;

enum class DialogAnswer { Yes, No, Cancel };

enum class NotificationType {NoData, Xml, PisHash };

namespace ModalDialogBuilder
{
    void openDialog(ProcedureDialogPresenter& p);
    void openDialog(PatientDialogPresenter& p);
    void openDialog(ProcedureEditorPresenter& p);
    void openDialog(DetailedStatusPresenter& p);
    void openDialog(LoginPresenter& p);
    void openDialog(DoctorDialogPresenter& p);
    void openDialog(MedicationPresenter& p);
    void openDialog(DosagePresenter& p);
    void openDialog(PracticeManagerPresenter& p);
    void openDialog(ReferralPresenter& p);
	void openDialog(StatisticDialogPresenter& p);
	void openDialog(ProcedureHistoryPresenter& p);
	void openDialog(FiscalReceiptPresenter& p);

	void openBrowserDialog();

	DialogAnswer openSaveDialog(const std::string& text);
	std::optional<std::vector<Procedure>> selectProcedures(const std::vector<Procedure>& procedures, SelectionPref s = SelectionPref::All);
	//returns true if user wants to apply the procedures to the status
	std::optional<BusinessOperation> editBusinessOperation(const BusinessOperation& op);
	std::optional<BusinessOperation> addBusinessOperation();
	//use default constructed for new notice
	std::optional<MedicalNotice> openDialog(const MedicalNotice& notice);
	bool askDialog(const std::string& questionText);
	void showError(const std::string& error);
	void showMultilineDialog(const std::string& text, const std::string& title = "Debug");
	void showMessage(const std::string& message);
	void openSettingsDialog(int tabFocus = 0);
	int openButtonDialog(const std::vector<std::string>& buttonNames, const std::string& title, const std::string& description = std::string()); //returns -1 if canceled
	//std::optional<std::string> getFileNamePath(const std::string& filename);
	void openExplorer(const std::string& path);
	std::optional<std::string> getStringInput(const std::string& dialogName, const std::string& fieldName);
	//returns empty string if canceled
	std::string pinPromptDialog(const std::string& pem, const std::string& driver);
	std::optional<ProcedureCode> procedureCodeDialog();
	//returns empty string if canceled
	//std::string ksmpDialog(KsmpList& list, const std::string& preSelectCode = {});
	void pisDialog(MonthNotifPresenter* presenter);
	std::string inputDialog(const std::string& text, const std::string& title, const std::string& input = {}, bool asPassword = false, bool emptyNotAllowed = true);
	void saveFile(const std::string& data, const std::string& filename);
	std::optional<std::string> openFile();
	std::vector<std::string> openFiles();
	std::optional<std::string> getFilePath(const std::string& filename);
	void showDetailedError(const std::string& descr, const std::string& details);
	std::optional<Allergy> openAllergyDialog(const Allergy& a = {});
};

