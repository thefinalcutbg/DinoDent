#pragma once
#include <string>
#include "Model/Patient.h"

struct Patient;
struct AmbList;
struct Invoice;
class Referral;
struct FiscalReport;
class NhifSpecReport;
struct Prescription;

namespace Print
{
	//if the filepath is provided, the file is saved as PDF

	enum DeclaratorType { Insured, Custody, Empty };

	bool ambList(const AmbList& amb, const Patient& patient, const std::string& pdfFilename = {});;
	bool consent(const Patient& p, const std::string& pdfFilename = {});
	bool invoice(const Invoice& inv, const std::string& pdfFilename = {});
	bool prescription(const Prescription& prescr, const Patient& patient, const std::string& filename = {});
	
	//declarations:
	bool printDentureDeclaration(const Patient* patient, const AmbList* amblist = nullptr, const std::string& pdfFilename = {});
	bool printHirbNoDeclaration(const Patient* patient, const std::string& pdfFilename = {});
	bool gdpr(const Patient& patient, const std::string& pdfFilename = {});

	void referral(const Referral& ref, const Patient& patient, const std::string& ambSheetNumber);

	//empty stuff
	void consent();
	void gdpr();
	void ambList();


	void saveFsicalReportToPDF(const FiscalReport& report);
	void saveNhifSpecReport(const NhifSpecReport& report);
		
}

