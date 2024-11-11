#pragma once
#include <string>
#include <vector>

class ICD10
{
private:

	std::string m_code{};

public:

	ICD10() {};
	ICD10(const std::string& code);

	bool isValid() const {
		return m_code.size();
	}

	const std::string& code() const { return m_code; };
	const std::string& name() const;
	bool isDental() const;
	bool setCode(const std::string& code);
	bool operator == (const ICD10& other) const;
	bool operator != (const ICD10& other) const;

	static void initialize();
	static const std::string& getDescriptionFromICDCode(const std::string& mkbCode);
	static const std::string& getCodeFromName(const std::string& icdDescription);
	static bool isInitialized();
	static const std::vector<ICD10>& getDentalICDCodes();
	static const std::vector<ICD10>& getFullMKBList();
	
	
};