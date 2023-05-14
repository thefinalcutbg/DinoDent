#pragma once
#include <vector>
#include <string>

class Diagnosis
{
	
	int m_idx{ 0 };

	inline static std::vector<std::string> s_names;

public:
	static void initialize();

	Diagnosis() {};
	Diagnosis(const std::string& name);
	Diagnosis(int key, const std::string& description = {});

	int index() const { return m_idx; }

	bool isValid() const;

	const std::string& name() const { return s_names[m_idx]; }

	const std::string& getFullDiagnosis() const;

	std::string description;

	static const std::vector<std::string>& getNames() { return s_names; }
};

