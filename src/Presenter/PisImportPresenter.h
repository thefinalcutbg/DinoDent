#pragma once

#include "Network/PIS/GetFileService.h"
#include "Network/HIS/EDental.h"
#include <stack>
#include <string>

class PisImportView;

class PisImportPresenter
{
	GetAmbHashes get_hash_serv;
	GetFileService get_file_serv;
	EDental::Fetch his_fetch_serv;

	std::stack<int> m_years;
	std::stack<std::string> m_hashes;

	PisImportView* view;

	void getNextYearHashes();
	void processHash();
	void importToDb(const std::string& file);
	void abort();


public:

	void setView(PisImportView* view);

	void importData(int fromYear, int toYear);

	void loadXml();

};