#pragma once
#include <string>

struct Patient;
struct AmbList;
struct Invoice;
class Referral;
struct FiscalReport;
class NhifSpecReport;
struct Prescription;

namespace Print
{
	enum DeclaratorType { Insured, Custody, Empty };

	void ambList(const AmbList& amb, const Patient& patient, const std::string& pdfFilename = {});;
	void invoice(const Invoice& inv, const std::string& pdfFilename = {});
	void consent(const Patient& patient, const std::string& pdfFilename = {});
	void consent();
	void gdpr(const Patient& patient, const std::string& pdfFilename = {});
	void ambList();
	void printDentureDeclaration(const Patient& patient, DeclaratorType type, const AmbList* amblist = nullptr, const std::string& pdfFilename = {});
	void printHirbNoDeclaration(const Patient& patient, DeclaratorType type, const std::string& pdfFilename = {});
	void referral(const Referral& ref, const Patient& patient, const std::string& ambSheetNumber);
	void saveFsicalReportToPDF(const FiscalReport& report);
	void saveNhifSpecReport(const NhifSpecReport& report);
	void prescription(const Prescription& prescr, const Patient& patient, const std::string& filename = {});
	
}

