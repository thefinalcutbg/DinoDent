#pragma once
#include "CommonValidators.h"

class NameValidator : public Validator
{
	inline static const std::string invalidName{ "Невалидно име" };
	inline static const std::string mustBeInCyrillic{ "Името трябва да бъде изписано на кирилица" };

	NotEmptyValidator not_empty;
	CyrillicValidator cyrillic_only;
public:
	bool validateInput(const std::string& text) override;
};