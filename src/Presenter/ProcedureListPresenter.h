#pragma once
#include "Model/Dental/ProcedureCode.h"
#include "Model/Dental/ProcedureListElement.h"
#include <set>

class ProcedureListPresenter {

	struct Block {

		std::string name;
		size_t from;
		size_t to;
	};

	static inline std::vector<Block> m_blocks;
	static inline std::vector<ProcedureListElement> m_elements;

	std::vector<ProcedureListElement> m_nhif_list;

	bool fav_update_required = false;

	bool has_favourites = false;
	
public:
	ProcedureListPresenter();

	std::vector<std::string> getSectionList() const;
	std::vector<ProcedureListElement> getList(int index) const;
	void favClicked(int section, const std::string& code);
	void setNhifProcedures(const std::vector<std::pair<ProcedureCode, double>>& nhifProcedures);
	bool hasFavourites() const;
	void setCodePrice(const std::string& code, double price);
	~ProcedureListPresenter();
};