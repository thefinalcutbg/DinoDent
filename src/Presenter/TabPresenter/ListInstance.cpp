#include "ListInstance.h"
#include <QDebug>

ListInstance::ListInstance()
	: edited{ false }
{}

ListInstance::ListInstance(const ListInstance& old_obj) :
	edited{old_obj.edited},
	patient{old_obj.patient},
	amb_list{old_obj.amb_list},
	_scrollHeight{old_obj._scrollHeight},
	_scrollWidth{old_obj._scrollWidth}
{

	selectedTeeth.reserve(old_obj.selectedTeeth.size());

	for (const auto &tooth : old_obj.selectedTeeth)
	{
		selectedTeeth.push_back(&this->amb_list.teeth[tooth->index]);
	}
}

ListInstance::ListInstance(const AmbList& ambList, std::shared_ptr<Patient> patient) :
	amb_list(ambList),
	patient(patient),
	edited(0)
{
	selectedTeeth.reserve(32);
}

bool ListInstance::isNew()
{
	return amb_list.isNew();
}

bool ListInstance::isEdited()
{
	return edited;
}


std::string ListInstance::getTabName()
{
	std::string tabName;
	tabName.reserve(60);

	if (isEdited()) tabName+="*";

	tabName += isNew() ? u8"Нов амб.лист" :
	u8"Амб.лист №" + std::to_string(amb_list.number);

	tabName += " ";
	tabName += patient->FirstName;
	tabName += " ";

	tabName += patient->LastName;

	return tabName;
}

