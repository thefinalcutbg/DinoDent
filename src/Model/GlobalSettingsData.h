#pragma once
#include <vector>
#include <string>

enum DirType {
	PRACTICE, DOCTOR, PATIENTLF, PATIENTFL, YEARMONTH, DOCTYPE
};

struct GlobalSettingsData {
	std::vector<std::string> pkcs11_list;
	bool dev_branch{ false };
	bool show_requests{ false };
	bool show_replies{ false };
	int signer_model{ 0 };
	std::string signer_filepath;

	std::string pdfDir{};
	std::vector<DirType> subdirStructure;
};
