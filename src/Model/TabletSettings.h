#pragma once
#include <string>
#include <vector>

struct TabletSettings {

	enum DirType {
		PRACTICE, DOCTOR, PATIENTLF, PATIENTFL, YEARMONTH, DOCTYPE
	};

	int model{ 0 };
	std::string signer_filepath;
	std::string pdfDir{};
	std::vector<DirType> subdirStructure;
};
