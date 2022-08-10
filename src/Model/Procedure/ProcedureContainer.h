#pragma once
#include "Procedure.h"
#include <vector>

class ToothContainer;

class ProcedureContainer
{
	std::vector<Procedure> m_proc;

public:
	void addProcedures(const std::vector<Procedure>& p);
	void addProcedure(const Procedure& p);
	bool hasNzokProcedure() const;
	const std::vector<Procedure>& list() const { return m_proc; }
	void refreshTeethTemporary(const ToothContainer& teeth);
	void removeProcedure(int idx);
	void replaceProcedure(const Procedure& p, int idx);
	const Procedure& at(int index) const;
	size_t size() const { return m_proc.size(); }

	auto begin() { return m_proc.begin(); }
	auto end() { return m_proc.end(); }
	auto begin() const { return m_proc.begin(); }
	auto end() const { return m_proc.end(); }
	bool empty() const { return m_proc.empty(); }
	const Procedure& operator [](int index) const { return m_proc[index]; }
	Procedure& operator [](int index) { return m_proc[index]; }

};