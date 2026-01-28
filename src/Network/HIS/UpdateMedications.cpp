#include "UpdateMedications.h"

#include <TinyXML/tinyxml.h>

#include <json/json.h>
#include "Model/Prescription/Medication.h"
#include "View/ModalDialogBuilder.h"
#include "GlobalSettings.h"

void UpdateMedications::update()
{
	GetNumenclature::sendRequest(9);
}

bool UpdateMedications::parseNumenclature(const std::string& reply)
{
	if (reply.empty()) return false;

	auto error = getErrors(reply);

	if (error.size()) {
		ModalDialogBuilder::showError(error);
		return false;
    }

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto table = docHandle
		.FirstChild()	//nhis::message
		.Child(1)		//nhis::contents
		.FirstChild();	//nhis::numenclature

    Json::Value num;

	for ( int i = 1; table.Child(i).ToElement(); i++)
	{

		auto entryHandle = table.Child(i);

        std::string id = entryHandle.Child(0).ToElement()->Attribute("value");
        std::string descr = entryHandle.Child(1).ToElement()->Attribute("value");

		std::string form{ "0" };

		//iterating meta to find the form id:
		for (int y = 2;;y++)
		{
			auto metaHandle = entryHandle.Child(y);

			if (!metaHandle.ToElement()) break;
	
			if (metaHandle.FirstChild().ToElement()->FirstAttribute()->ValueStr() != "form id") continue;

			form = metaHandle.Child(1).ToElement()->FirstAttribute()->ValueStr();
			break;
		}

        Json::Value entity;
        entity["id"] = std::stoi(id);
        entity["descr"] = descr;
        entity["form"] = std::stoi(form);

        num["meds"].append(entity);
	}

    num["last_updated"] = Date::currentDate().to8601();

    GlobalSettings::writeNumMedJson(Json::FastWriter().write(num));

    Medication::initialize();

	return true;
}
