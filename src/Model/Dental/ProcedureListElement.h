#pragma once
#include "ProcedureCode.h"
#include <set>

struct ProcedureListElement
{
	ProcedureCode code;

	bool nhif = false;
	bool favourite = false;
};

class ProcedureList {

	struct Block {

		std::string name;
		size_t from;
		size_t to;
	};

	static inline std::vector<Block> m_blocks;
	static inline std::vector<ProcedureListElement> m_elements;

	std::vector<ProcedureListElement> m_nhif_list;

	bool fav_update_required = false;
	
public:
	ProcedureList();

	std::vector<std::string> getSectionList() const;
	std::vector<ProcedureListElement> getList(int index) const;
	void favClicked(int section, const std::string& code);
	void setNhifProcedures(const std::vector<ProcedureCode>& nhifProcedures);

	~ProcedureList();
};