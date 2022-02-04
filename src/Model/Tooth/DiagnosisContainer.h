#pragma once
#include <vector>
#include <string>


class DiagnosisContainer
{
	inline static bool s_init{ false };
	inline static std::vector<std::string> s_caries;
	inline static std::vector<std::string> s_pulpitis;
	inline static std::vector<std::string> s_fracture;
	inline static std::vector<std::string> s_lesion;
	inline static std::vector<std::string> s_root;

public:

	static void initialize();

	static const std::vector<std::string>& caries() { return s_caries; }
	static const std::vector<std::string>& pulpitis() { return s_pulpitis; }
	static const std::vector<std::string>& fracture() { return s_fracture; }
	static const std::vector<std::string>& lesion() { return s_lesion; }
	static const std::vector<std::string>& root() { return s_root; }

};

