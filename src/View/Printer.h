#pragma once
#include <string>

struct Patient;
struct AmbList;
struct Invoice;
class Referral;
struct FiscalReport;
class NhifSpecReport;

namespace Print
{
	enum DeclaratorType { Insured, Custody, Empty };

	void ambList(const AmbList& amb, const Patient& patient);
	void invoice(const Invoice& inv);
	void consent(const Patient& patient);
	void gdpr(const Patient& patient);
	void ambList();
	void printDentureDeclaration(const Patient& patient, DeclaratorType type);
	void printHirbNoDeclaration(const Patient& patient, DeclaratorType type);
	void referral(const Referral& ref, const Patient& patient, const std::string& ambSheetNumber);
	void saveFsicalReportToPDF(const FiscalReport& report);
	void saveNhifSpecReport(const NhifSpecReport& report);
	
}

