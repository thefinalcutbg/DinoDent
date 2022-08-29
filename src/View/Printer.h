#pragma once
struct Patient;
struct AmbList;
struct Invoice;

namespace Print
{
	void ambList(const AmbList& amb, const Patient& patient);
	void invoice(const Invoice& inv);
	void ambList();
}

