#pragma once
#include "Procedure.h"
#include <vector>
#include <utility>

class ToothContainer;

class ProcedureContainer
{
	std::vector<Procedure> m_proc;
	std::vector<Procedure> m_removed;

public:
	void addProcedures(const std::vector<Procedure>& p);
	void addProcedure(const Procedure& p);
	bool hasNhifProcedure() const;

	bool hasDentureManifactureProcedure() const;
	std::string nhifDentureStr() const;
	std::string nhifDentureDate() const;

	bool hasDentalExam() const;
	const std::vector<Procedure>& list() const { return m_proc; }
	void refreshTeethTemporary(const ToothContainer& teeth);
	void removeProcedure(int idx);
	void replaceProcedure(const Procedure& p, int idx);
	//returns true if move is successful
	bool moveProcedure(int from, int to);
	const Procedure& at(int index) const;
	size_t size() const { return m_proc.size(); }
	bool hasPregnancy() const;
	auto begin() { return m_proc.begin(); }
	auto end() { return m_proc.end(); }
	auto begin() const { return m_proc.begin(); }
	auto end() const { return m_proc.end(); }
	bool empty() const { return m_proc.empty(); }
	const Procedure& operator [](int index) const { return m_proc[index]; }
	Procedure& operator [](int index) { return m_proc[index]; }
	const std::vector<Procedure>& removedProcedures() const { return m_removed; }
	void clearRemovedProcedures() { m_removed.clear(); }
	void setRemovedProcedures(const std::vector<Procedure>& removed) { m_removed = removed; }
};