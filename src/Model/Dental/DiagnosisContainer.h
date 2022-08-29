#pragma once
#include <vector>
#include <string>

namespace DiagnosisContainer
{
	void initialize();
	enum DiagnosisType { Caries, Pulpitis, Fracture, Lesion, Root };
	std::vector<std::string>& diagnosis(DiagnosisType type);
};

