#pragma once
struct Patient;
struct AmbList;
struct Invoice;
class Referral;

namespace Print
{
	enum DentureDeclaration { Insured, Custody, Empty };

	void ambList(const AmbList& amb, const Patient& patient);
	void invoice(const Invoice& inv);
	void consent(const Patient& patient);
	void gdpr(const Patient& patient);
	void ambList();
	void printDentureDeclaration(const Patient& patient, DentureDeclaration type);
	void referral(const Referral& ref, const Patient& patient, int ambSheetNumber);
	
}

