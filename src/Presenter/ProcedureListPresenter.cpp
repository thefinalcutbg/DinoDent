#include "ProcedureListPresenter.h"
#include "Model/Dental/ProcedureCode.h"
#include "Database/DbDoctor.h"
#include "Database/DbProcedure.h"
#include "Model/User.h"
#include "Model/Dental/NhifProcedures.h" 
#include <unordered_map>

ProcedureListPresenter::ProcedureListPresenter()
{

	//lazy init
	if (m_elements.empty()) {

		int blockNumber = -1;

		auto& allProcedures = ProcedureCode::getProcedures();

		m_elements.reserve(allProcedures.size());

		for (size_t i = 0; i < allProcedures.size(); i++)
		{

			auto& code = allProcedures[i];

			//setting nhif procedures separately
			if (code.nhifCode()) continue;

			m_elements.push_back(
				ProcedureListElement{
						.code = code.code(),
						.nhif = false,
						.favourite = false
				});


			auto currentBlockNumber = code.achiBlock();

			if (blockNumber != currentBlockNumber) {

				blockNumber = currentBlockNumber;

				auto newBlock = Block{
					.name = code.achiBlockName(),
					.from = m_elements.size()-1
				};

				m_blocks.push_back(newBlock);
			}

			m_blocks.back().to = m_elements.size();

		}

		m_blocks.back().to = m_elements.size();
	}
	
	auto favourites = DbDoctor::getFavouriteProcedures(User::doctor().LPK);

	auto prices = DbProcedure::getCodeValues();

	if (favourites.empty()) return;

	has_favourites = true;

	for (auto& e : m_elements) {

		if (favourites.count(e.code.code())) {
			e.favourite = true;
		}

		if (prices.count(e.code.code())) {
			e.price = prices.at(e.code.code());
		}
	}

}

std::vector<std::string> ProcedureListPresenter::getSectionList() const
{
	std::vector<std::string> result{
		"Всички",
		"Избрани"
	};

	for (auto& block : m_blocks) {
		result.push_back(block.name);
	}

	return result;
}

std::vector<ProcedureListElement> ProcedureListPresenter::getList(int index) const
{
	std::vector<ProcedureListElement> result;

	if (!index) {

		result.reserve(m_nhif_list.size() + m_elements.size());

		result.insert(result.end(), m_nhif_list.begin(), m_nhif_list.end());
		result.insert(result.end(), m_elements.begin(), m_elements.end());

		return result;
	}

	if (index == 1) {
		
		for (auto& code : m_nhif_list) {
			result.push_back(code);
		}

		for (auto& e : m_elements) {

			if (e.favourite) {
				result.push_back(e);
			}
		}

		return result;
	}


	index -= 2;

	result.reserve(m_blocks[index].to - m_blocks[index].from);

	for (int i = m_blocks[index].from; i < m_blocks[index].to; i++)
	{
		result.push_back(m_elements[i]);
	}

	return result;

}

void ProcedureListPresenter::favClicked(int section, const std::string& code)
{
	fav_update_required = true;

	for (auto& e : m_elements) {

		if (code == e.code.code()) {

			e.favourite = !e.favourite;

			return;

		}

	}	
}

void ProcedureListPresenter::setNhifProcedures(const std::vector<std::pair<ProcedureCode, double>>& nhifProcedures)
{
	m_nhif_list.clear();

	for (auto& [nhifCode, price] : nhifProcedures) {

		m_nhif_list.push_back(

			ProcedureListElement{
				.code = nhifCode,
				.nhif = true,
				.favourite = false,
				.price = price
			}
		);

	}
}

bool ProcedureListPresenter::hasFavourites() const
{
	return has_favourites;
}

void ProcedureListPresenter::setCodePrice(const std::string& code, double price)
{
	for (auto& e : m_elements) {
		if (e.code.code() == code) {
			
			e.price = price;
			
			DbProcedure::setPrice(code, price);

			return;
		}
	}
}

ProcedureListPresenter::~ProcedureListPresenter()
{
	if (!fav_update_required) return;

	std::vector<std::string> fav_list;

	for (auto& e : m_elements) {
		if (e.favourite) {
			fav_list.push_back(e.code.code());
		}
	}

	DbDoctor::updateFavouriteProcedures(fav_list, User::doctor().LPK);
}
