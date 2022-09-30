#include "UpdateMkb.h"
#include "View/ModalDialogBuilder.h"
#include "TinyXML/tinyxml.h"
#include "Database/Database.h"
#include "Database/DbUpdateStatus.h"
#include "Model/Dental/MKB.h"

void UpdateMkb::update()
{
    GetNumenclature::sendRequest(11);
}

bool UpdateMkb::parseNumenclature(const std::string& reply)
{

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto table = docHandle
		.FirstChild()	//nhis::message
		.Child(1)		//nhis::contents
		.FirstChild();	//nhis::numenclature


	Db db;

	db.execute("PRAGMA foreign_keys = OFF");

	db.execute("DELETE FROM mkb");

	std::string query;
	query.reserve(10000);


	//second entry

	for (int i = 1; table.Child(i).ToElement(); i++)
	{
		if (query.empty()) {
			query = "INSERT INTO mkb (code, description) VALUES ";
		}

		auto entryHandle = table.Child(i);

		std::string key;
		std::string description;

		key = entryHandle.Child(0).ToElement()->Attribute("value");
		description = entryHandle.Child(1).ToElement()->Attribute("value");

		query.append("('" + key + "','" + description + "'),");

		if (!(i % 200)) {
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

	DbUpdateStatus::setAsUpdated(DynamicNum::MKB);

	MKB::initialize();


	return true;
}