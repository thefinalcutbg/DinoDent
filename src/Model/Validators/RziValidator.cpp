#include "RziValidaor.h"

const std::string error("Невалиден номер на практиката");

bool RziValidator::validateInput(const std::string& text)
{
	_errorMsg = &error;

	if (!m_notEmpty.validateInput(text)) {
		_errorMsg = &m_notEmpty.getErrorMessage();
		return false;
	}

	if (!m_digitsOnly.validateInput(text)) {
		return false;
	}

	if (!m_exactLenght.validateInput(text)) {
		return false;
	}

	int rhif = stoi(text.substr(0, 2));

	if (rhif >= 1 && rhif <= 28){
		return true;
	}

	if (rhif >= 31 && rhif <= 58) {
		return true;
	}

	return false;
}