#include "GetNumenclature.h"
#include "View/ModalDialogBuilder.h"
#include "Model/FreeFunctions.h"
#include "Database/DbPatient.h"

GetNumenclature::GetNumenclature() : HisService("C001", "/v1/nomenclatures/all/get") {};

bool GetNumenclature::sendRequest(int num)
{
	if (num < 1 || num > 99) return false;

	std::string request =
		"<nhis:nomenclatureId value=\"CL" + FreeFn::leadZeroes(num, 3) + "\"/>";

	return HisService::sendRequestToHisNoAuth(request);
}
