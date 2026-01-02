#pragma once
#include <string>
#include "Model/Patient.h"

struct Patient;
struct AmbList;
struct Invoice;
class Referral;
struct FiscalReport;
class NhifSpecReport;
class ProcedureTemplateModel;
struct Prescription;
struct TreatmentPlan;

namespace Print
{
	//if the filepath is provided, the file is saved as PDF

	enum DeclaratorType { Insured, Custody, Empty };

	bool ambList(
		const AmbList& amb, 
		const Patient& patient, 
		bool printNhif,
		const std::vector<Procedure>& selectedProcedures,
		bool printReferrals,
		const std::string& pdfFilename = {}
	);
	bool consent(const Patient& p, const std::string& pdfFilename = {});
	bool invoice(const Invoice& inv, const std::string& pdfFilename = {});
	bool prescription(const Prescription& prescr, const Patient& patient, const std::string& filename = {});
    bool treatmentPlan(const TreatmentPlan& plan, const Patient& p, const std::string& pdfFilename = {});
	//declarations:
	bool printDentureDeclaration(const Patient* patient, const AmbList* amblist = nullptr, const std::string& pdfFilename = {});
	bool printHirbNoDeclaration(const Patient* patient, const std::string& pdfFilename = {});
	bool gdpr(const Patient& patient, const std::string& pdfFilename = {});
    bool priceList(ProcedureTemplateModel& m);

	void referral(const Referral& ref, const Patient& patient, const std::string& ambSheetNumber);

	//empty stuff
	void consent();
	void gdpr();
	void ambList();


	void saveFsicalReportToPDF(const FiscalReport& report);
	void saveNhifSpecReport(const NhifSpecReport& report);
		
}

