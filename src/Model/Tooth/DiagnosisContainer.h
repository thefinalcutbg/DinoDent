#pragma once
#include <vector>
#include <string>

typedef std::vector<std::string> DiagnosisList;

class DiagnosisContainer
{
	inline static bool s_init{ false };
	inline static DiagnosisList s_caries;
	inline static DiagnosisList s_pulpitis;
	inline static DiagnosisList s_fracture;
	inline static DiagnosisList s_lesion;
	inline static DiagnosisList s_root;

public:

	static void initialize();

	static const DiagnosisList& caries() { return s_caries; }
	static const DiagnosisList& pulpitis() { return s_pulpitis; }
	static const DiagnosisList& fracture() { return s_fracture; }
	static const DiagnosisList& lesion() { return s_lesion; }
	static const DiagnosisList& root() { return s_root; }

};

