#pragma once
#include <unordered_set>
#include <string>
#include <vector>
#include "View/ReportDialog/ReportDialogResult.h"
#include "Model/Procedure/ProcedureTemplate.h"
#include "Model/AmbList.h"
#include "Model/Financial/BusinessOperation.h"
#include "Model/Financial/MonthNotifRow.h"

enum class SelectionPref {All, OnlyNZOK, OnlyPaid};
enum class MonthNotifLoad {FromPIS, FromFile, Rejected};

class ProcedureDialogPresenter;
class PatientDialogPresenter;
class ProcedureEditorPresenter;
class AllergiesDialogPresenter;
class ListSelectorPresenter;
class DetailedStatusPresenter;
class LoginPresenter;
class DoctorDialogPresenter;
class InvoicePresenter;
class AddPracticePresenter;
class MonthNotifPresenter;
enum class DialogAnswer { Yes, No, Cancel };

namespace ModalDialogBuilder
{
	void openDialog(ProcedureDialogPresenter* p);
	void openDialog(PatientDialogPresenter* p);
	void openDialog(ProcedureEditorPresenter* p);
	void openDialog(AllergiesDialogPresenter* p);
	void openDialog(ListSelectorPresenter* p);
	void openDialog(DetailedStatusPresenter* p);
	void openDialog(LoginPresenter* p);
	void openDialog(std::optional<ReportDialogResult>& result);
	void openDialog(AddPracticePresenter* p);
	void openDialog(DoctorDialogPresenter* p);
	int monthNotifPicker(const std::vector<MonthNotifRow>& rows);
	//returns 0 if canceled
	long long saveAsDocNumber(long long newNum, std::unordered_set<int> existingNumbers, const std::string& docName, int numberCharLength);
	std::optional<Date> saveAsDate(const Date& date, const std::string& docName);
	std::optional<ProcedureTemplate> openProcedureTemplateDialog(const ProcedureTemplate* pTemp = nullptr, int code = 0);
	DialogAnswer openSaveDialog(const std::string& text);
	std::optional<std::string> getMonthNotifFromFile();
	std::optional<Procedures> selectProcedures(const Procedures& procedures, SelectionPref s = SelectionPref::All);
	//returns true if user wants to apply the procedures to the status
	bool pisHistoryDialog(const Procedures& procedures);
	std::optional<BusinessOperation> editBusinessOperation(const BusinessOperation& op);
	std::optional<BusinessOperation> addBusinessOperation(const std::vector<ProcedureTemplate>& priceList);
	bool askDialog(const std::string& questionText);
	void showError(const std::string& error);
	void showErrorList(const std::string& errors);
	void showMessage(const std::string& message);
	void openSettingsDialog();
	std::optional<std::string> getFileNamePath(const std::string& filename);
	void openExplorer(const std::string& path);
	MonthNotifLoad monthNotifLoadDialog();
	std::optional<std::string> getStringInput(const std::string& dialogName, const std::string& fieldName);
	//returns empty string if canceled
	std::string pinPromptDialog(const std::string& pem);
};

