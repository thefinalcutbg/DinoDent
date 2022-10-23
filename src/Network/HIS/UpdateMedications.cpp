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

	db.execute("BEGIN TRANSACTION");

	db.execute("DELETE FROM numMed");


	for ( int i = 1; table.Child(i).ToElement(); i++)
	{
		
		db.newStatement("INSERT INTO numMed (rowid, name, form) VALUES (?,?,?)");
	
		auto entryHandle = table.Child(i);

		db.bind(1, entryHandle.Child(0).ToElement()->Attribute("value"));
		db.bind(2, entryHandle.Child(1).ToElement()->Attribute("value"));
		
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

		db.bind(3, form);

		db.execute();
	
	}

	if (!db.execute("END TRANSACTION"))
	{
		db.execute("ROLLBACK");
		db.execute("PRAGMA foreign_keys = ON");
		ModalDialogBuilder::showError(u8"Неуспешно обновяване на номенклатурата");
		return false;
	}
	
	db.execute("PRAGMA foreign_keys = ON");
	
	DbUpdateStatus::setAsUpdated(DynamicNum::Medication);

	Medication::initialize();

	return true;

}