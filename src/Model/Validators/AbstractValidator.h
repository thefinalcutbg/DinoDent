#pragma once
#include <string>
#include <array>

#include "Model/Date.h"

class Validator
{
protected:

	inline static const std::string noError{""};
	const std::string* _errorMsg{ nullptr };
public:
    virtual bool validateInput(const std::string&) { return true; };
    virtual bool validateInput(int, int) { return true; };
    virtual bool validateInput(const std::array<bool, 6>&) { return true; }
    virtual bool validateInput(const Date&) { return true; };
	
	const std::string& getErrorMessage() 
	{ 
		if (_errorMsg == nullptr) return noError;
		return *_errorMsg; 
	};
};
