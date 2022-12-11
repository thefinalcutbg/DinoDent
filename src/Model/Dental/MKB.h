#pragma once
#include <string>
#include <vector>

class MKB
{
private:

	std::string m_code{};

public:

	MKB() {};
	MKB(const std::string& code) { setCode(code); }

	bool isValid() const {
		return m_code.size();
	}

	const std::string& code() const { return m_code; };
	const std::string& name() const;
	bool setCode(const std::string& code);

	static void initialize();
	static const std::string& getNameFromMKBCode(const std::string& mkbCode);
	static bool isInitialized();
	static const std::vector<MKB>& getDentalMKBList();
	static const std::vector<MKB>& getFullMKBList();
	
	
};