#include "ProcedureDateValidator.h"

ProcedureDateValidator::ProcedureDateValidator (Date ambDate, Date patientTurns18) :

	_ambDate{ ambDate },
	_dayBefore18 {patientTurns18.yesterday()}
{
	_validator.setMinDate(_ambDate);
	_validator.setMinErrorMsg("Датата на манипулацията не може да е по-малка от тази на амбулаторният лист");
}


bool ProcedureDateValidator::validateInput(const Date& date)
{
	bool valid = _validator.validateInput(date);
	_errorMsg = &_validator.getErrorMessage();
	return valid;
}

Date ProcedureDateValidator::getValidDate()
{
	return _validator.getMin();
}

#include "Model/Procedure/MasterNZOK.h"

void ProcedureDateValidator::setProcedure(int code, bool nzok)
{
	if (!nzok || !MasterNZOK::instance().isMinorOnly(code))
	{
		_validator.setMaxDate(Date{_ambDate.getMaxDayOfMonth(), _ambDate.month, _ambDate.year});
		_validator.setMaxErrorMsg("Датата на манипулацията не съвпада с месеца на амбулаторният лист");

		return;
	}
	
	if (_dayBefore18 < _validator.getMax() && _dayBefore18 > _validator.getMin())
	{
		_validator.setMaxDate(_dayBefore18);
		_validator.setMaxErrorMsg("Тази манипулация е възможна само за лица под 18 годишна възраст");
	}
}
