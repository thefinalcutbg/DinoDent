#include "ListInstance.h"
#include <QDebug>

ListInstance::ListInstance()
	: edited{ false }
{}

ListInstance::ListInstance(AmbList ambList, std::shared_ptr<Patient> patient) :
	amb_list(ambList), 
	patient(patient),
	edited(0)
{
	selectedTeeth.reserve(32);
}

bool ListInstance::isNew()
{
	return amb_list.id.empty();
}

bool ListInstance::isEdited()
{
	return edited;
}

std::string ListInstance::getTabName()
{
	std::string tabName;
	tabName.reserve(30);

	if (isEdited()) {
		tabName.append("*");
	}

	if (isNew()) {
		tabName.append("Нов амб.лист");
	}
	else {
		tabName.append("Амб.лист №" + std::to_string(amb_list.number));
	}

	tabName.append(" -" + patient->FirstName + " " + patient->LastName);

	return tabName;
}

