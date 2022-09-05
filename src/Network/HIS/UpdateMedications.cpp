#include "UpdateMedications.h"
#include "Database/Database.h"
#include <TinyXML/tinyxml.h>
#include <qdebug.h>
#include "Database/Database.h"

void UpdateMedications::update()
{
	GetNumenclature::sendRequest(9);
}

void UpdateMedications::parseReply(const std::string& reply)
{
	struct MediStruct { int key{ 0 }; std::string name{ 0 }; int form{ 0 }; };

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto table = docHandle
		.FirstChild()	//nhis::message
		.Child(1)		//nhis::contents
		.FirstChild();	//nhis::numenclature
	
	//second entry

	Db db;

	for ( int i = 1; ; i++)
	{
		auto entryHandle = table.Child(i);
		if (!entryHandle.ToElement()) break;

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

		db.execute(
			"INSERT INTO medication (key, name, form) VALUES ("
			+ key + ",'" + name + "'," + form + ") "
			"ON CONFLICT(key) DO UPDATE SET "
			"name='" + name + "',"
			"form=" + form
		);
	}



}