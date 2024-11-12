#pragma once
#include <string>
#include <vector>

#include "Model/Dental/ProcedureCode.h"
#include "Model/Dental/Procedure.h"
#include "Model/Financial/BusinessOperation.h"
#include "Model/Dental/HisSnapshot.h"
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
	std::string inputDialog(const std::string& text, const std::string& title, const std::string& input = {}, bool asPassword = false, bool emptyNotAllowed = true);
	void saveFile(const std::string& data, const std::string& filename);
	bool askDialog(const std::string& questionText);
	void showError(const std::string& error);
	void showMessage(const std::string& message);
	int openButtonDialog(const std::vector<std::string>& buttonNames, const std::string& title, const std::string& description = std::string()); //returns -1 if canceled
	void openExplorer(const std::string& path);
	std::optional<std::string> getStringInput(const std::string& dialogName, const std::string& fieldName);
	std::optional<std::string> showMultilineDialog(const std::string& text, const std::string& title = "Debug", bool enableEdit = false);
	std::string pinPromptDialog(const std::string& pem, const std::string& driver); //returns empty string if canceled

	std::optional<std::string> openFile();
	std::vector<std::string> openFiles();
	std::optional<std::string> getFilePath(const std::string& filename);
	void showDetailedError(const std::string& descr, const std::string& details);


	std::optional<Allergy> openAllergyDialog(const Allergy& a = {});
	std::optional<std::vector<Procedure>> selectProcedures(const std::vector<Procedure>& procedures, FinancingSource source);
	std::optional<BusinessOperation> editBusinessOperation(const BusinessOperation& op);
	std::optional<BusinessOperation> addBusinessOperation();
	std::optional<MedicalNotice> openDialog(const MedicalNotice& notice); //use default constructed for new notice
	void showSnapshots(const std::vector<HisSnapshot>& snapshots);

	void openSettingsDialog(int tabFocus = 0);
	void pisDialog(MonthNotifPresenter* presenter);
	
	ICD10 icdDialog(const ICD10& icd = {});
};

