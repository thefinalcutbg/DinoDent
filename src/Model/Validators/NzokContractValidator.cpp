#include "NzokContractValidator.h"

const std::string error{ u8"Невалиден номер на договор с НЗОК" };

bool NzokContractValidator::validateInput(const std::string& text)
{
	_errorMsg = &error;

	if (!m_notEmptyValidator.validateInput(text)) {
		_errorMsg = &m_notEmptyValidator.getErrorMessage();
		return false;
	}

	if (!m_exactValidator.validateInput(text)){
		return false;
	}

	if (!m_digitsOnlyValidator.validateInput(text)) {
		_errorMsg = &m_digitsOnlyValidator.getErrorMessage();
		return false;
	}


	int rhif = stoi(text.substr(0, 2));

	if (rhif < 1 || rhif > 28) {
		return false;
	}

	return true;
}
