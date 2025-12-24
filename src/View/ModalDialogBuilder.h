#pragma once
#include <string>
#include <vector>

#include "Model/Dental/ProcedureCode.h"
#include "Model/Dental/Procedure.h"
#include "Model/Financial/BusinessOperation.h"
#include "Model/Dental/HisSnapshot.h"
#include "Model/MedicalNotice.h"
#include "Model/Allergy.h"

class QPixmap;
class ProcedureDialogPresenter;
struct PatientDialogPresenter;
class ProcedureEditorPresenter;
class DetailedStatusPresenter;

class InvoicePresenter;

class MonthNotifPresenter;
class MedicationPresenter;
class PracticeManagerPresenter;
class ReferralPresenter;
class StatisticDialogPresenter;
class ProcedureHistoryPresenter;

enum class DialogAnswer { Yes, No, Cancel };

enum class NotificationType {NoData, Xml, PisHash };

namespace ModalDialogBuilder
{
	void openBrowserDialog();

	DialogAnswer YesNoCancelDailog(const std::string& text);
    std::string inputDialog(const std::string& label, const std::string& title, const std::string& input = {}, bool asPassword = false, bool emptyNotAllowed = true);
	void saveFile(const std::string& data, const std::string& filename);
	bool askDialog(const std::string& questionText, bool defaultAnswer = true);
	void showError(const std::string& error);
	void showMessage(const std::string& message);
	int openButtonDialog(const std::vector<std::string>& buttonNames, const std::string& title, const std::string& description = std::string()); //returns -1 if canceled
	void openExplorer(const std::string& path);
    std::optional<std::string> getStringInput(const std::string& dialogName, const std::string& fieldName, const std::string& input);
    void showMultilineDialog(const std::string& text, const std::string& title = "Debug");
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

	void openSettingsDialog(int settingsTab = 0);
	void pisDialog(MonthNotifPresenter* presenter);
	void displayPixmap(const QPixmap& px);
	ICD10 icdDialog(const ICD10& icd = {});
};

