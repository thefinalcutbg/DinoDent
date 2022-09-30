#include "UpdateMedications.h"
#include "Database/Database.h"
#include <TinyXML/tinyxml.h>
#include <qdebug.h>
#include "Database/Database.h"
#include "Model/Prescription/Medication.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbUpdateStatus.h"

void UpdateMedications::update()
{
	GetNumenclature::sendRequest(9);
}

bool UpdateMedications::parseNumenclature(const std::string& reply)
{

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto table = docHandle
		.FirstChild()	//nhis::message
		.Child(1)		//nhis::contents
		.FirstChild();	//nhis::numenclature
	
	//second entry

	Db db;

	db.execute("PRAGMA foreign_keys = OFF");

	db.execute("DELETE FROM numMed");
	
	std::string query;
	query.reserve(10000);

	for ( int i = 1; table.Child(i).ToElement(); i++)
	{
		if (query.empty()) {
			query = "INSERT INTO numMed (rowid, name, form) VALUES ";
		}

		auto entryHandle = table.Child(i);

		std::string key;
		std::string name;
		std::string form;

		key = entryHandle.Child(0).ToElement()->Attribute("value");
		name = entryHandle.Child(1).ToElement()->Attribute("value");

		//iterating meta to find the form id:
		for (int y = 2;;y++)
		{
			auto metaHandle = entryHandle.Child(y);

			if (!metaHandle.ToElement()) break;
	
			if (metaHandle.FirstChild().ToElement()->FirstAttribute()->ValueStr() != "form id") continue;

			form = metaHandle.Child(1).ToElement()->FirstAttribute()->ValueStr();
			break;

		}

		query.append("(" + key + ",'" + name + "'," + form + "),");
		
		if (!(i%200)) {
			//removing the last comma;
			query.pop_back();

			db.execute(query);
			query.clear();
		}
		
	}

	if (query.size()) {
		query.pop_back();
		db.execute(query);
	}
	
	db.execute("PRAGMA foreign_keys = ON");
	
	DbUpdateStatus::setAsUpdated(DynamicNum::Medication);

	Medication::initialize();

	return true;
}