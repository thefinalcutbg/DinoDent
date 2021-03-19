#include "ListInstance.h"
#include <QDebug>

ListInstance::ListInstance(AmbList ambList, Patient patient) :
	amb_list(ambList), 
	patient(patient),
	edited(0)
{
	selectedIndexes.reserve(32);
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

	if (isNew()) {
		tabName = "Нов амбулаторен лист";
	}
	else {
		tabName = "Амбулаторен лист №" + std::to_string(amb_list.number);
	}

	if (isEdited()) {
		tabName.append("*");
	}
	return tabName;
}

