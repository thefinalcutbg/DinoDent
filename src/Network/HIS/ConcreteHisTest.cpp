#include "ConcreteHisTest.h"
#include "View/ModalDialogBuilder.h"
#include "Model/FreeFunctions.h"
#include "Database/DbPatient.h"
void ConcreteHisTest::parseReply(const std::string& reply)
{
	ModalDialogBuilder::showMultilineDialog(reply);
}

bool ConcreteHisTest::sendRequest(int num)
{
	if (num < 1 || num > 99) return false;

	std::string request =
		subject(DbPatient::get("8903261129", 1)) + requester() +
		"<nhis:nomenclatureId value=\"CL" + leadZeroes(num, 3) + "\"/>";

	return HisService::sendRequestToHis(request);
}
