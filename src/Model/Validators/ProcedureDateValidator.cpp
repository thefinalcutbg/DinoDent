#include "ProcedureDateValidator.h"

ProcedureDateValidator::ProcedureDateValidator (Date patientTurns18) :

	_dayBefore18 {patientTurns18.yesterday()}
{
	_validator.setMinDate(Date(patientTurns18.day, patientTurns18.month, patientTurns18.year-18));
    _validator.setMinErrorMsg("Дата на манипулацията не може да е преди раждането на пациента!");
	setProcedure(0, 0);
}


bool ProcedureDateValidator::validateInput(const Date& date)
{
	bool valid = _validator.validateInput(date);
	_errorMsg = &_validator.getErrorMessage();
	return valid;
}


#include "Model/Dental/NhifProcedures.h"

void ProcedureDateValidator::setProcedure(int oldCode, bool nhif)
{

	if (
		_dayBefore18 < _validator.getMax() &&
		_dayBefore18 > _validator.getMin() &&
		nhif &&
		NhifProcedures::isMinorOnly(oldCode))
	{
		_validator.setMaxDate(_dayBefore18);
		_validator.setMaxErrorMsg("Тази манипулация е възможна само за лица под 18 годишна възраст");
	}
	else
	{
		_validator.setMinErrorMsg("Дата на манипулацията не може да е преди раждането на пациента!");
		_validator.setMaxDate(Date{ 1,1,3000 });
	}
}
