#pragma once
#include <string>
#include <unordered_map> 
#include <unordered_set>

class Specialty
{
	static inline std::unordered_map<int, std::string> s_specMap;
	static inline std::unordered_map<std::string, int> s_specNames;
	static inline std::unordered_set<int> s_isDoctor;
	static inline std::string s_spec_dummy;

	int m_idx{ -1 };

public:

	Specialty() {};

	Specialty(int index) {
		if (!s_specMap.count(index)) return;
		m_idx = index;
	};

	Specialty(const std::string& name);

	static void initialize();
	int getIdx() const { return m_idx; }
	bool isValid() const { return m_idx != -1; };
	const std::string& getName() const;
	static const std::unordered_map<std::string, int>& names() { return s_specNames; };
	static bool isDoctor(int idx) { return s_isDoctor.count(idx); }

};