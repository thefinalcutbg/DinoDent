#pragma once
#include <string>

struct AmbList;
struct Prescription;
struct Patient;
struct Invoice;

namespace DirTree {

	enum DeclarationType{ Denture, HIRBNo, Consent, GDPR };

	std::string get(const AmbList& amb, const Patient& patient);
	std::string get(const Prescription& prescr, const Patient& patient);
	std::string get(const Invoice& invoice);
	std::string get(DeclarationType declType, const Patient& patient);
}