#pragma once

#include "Network/PIS/GetFileService.h"
#include <stack>
#include <string>

class PisImportView;

class PisImportPresenter
{
	GetAmbHashes get_hash_serv;
	GetFileService get_file_serv;

	std::stack<std::string> m_hashes;

	PisImportView* view;

	void processHash();
	void importToDb(const std::string& file);
	void abort();

public:

	void setView(PisImportView* view);

	void importDataPressed();

};