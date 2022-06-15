#include "NotifListHandler.h"
#include "Presenter/MonthNotifLoader.h"
#include "TinyXML/tinyxml.h"
#include "View/ModalDialogBuilder.h"

NotifListHandler::NotifListHandler(MonthNotifLoader* loader) :
	loader{ loader }
{
}

FinancialDocType getNotifType(const std::string& type) {

	const char* typeStr[3]{
		"C_FDOC_INV",
		"C_FDOC_DTNOTE",
		"C_FDOC_CTNOTE"
	};

	for (int i = 0; i < 3; i++) {

		if (type == typeStr[i])
			return static_cast<FinancialDocType>(i);
	}

	return FinancialDocType::Unknown;
	
}


void NotifListHandler::getReply(const std::string& reply)
{
	if (loader == nullptr) return;

	if (reply.empty()) {
		ModalDialogBuilder::showError(u8"Неуспешна връзка със сървъра");
		loader->setNotifRows({});
		return;
	}



	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto table =
		docHandle
		.FirstChildElement()			  //envelope
		.Child(1)						  //body
		.FirstChildElement();			  //table

	std::vector<MonthNotifRow> result;

	//i is 1, since 0 is the table header (td)
	for (int i = 1; ; i++)
	{
		auto row = table.Child(i); //table row

		if (row.ToElement() == nullptr) {
			break;
		}


		result.emplace_back(
			MonthNotifRow{
				row.Child(0).ToElement()->GetText(),								//id hash
				Date::getDateFromXmlFormat(row.Child(1).ToElement()->GetText()),	//date
				getNotifType(getTextNullCheck(row.Child(2).ToElement())),						//type
				getTextNullCheck(row.Child(3).ToElement()),								//rzi
				getTextNullCheck(row.Child(4).ToElement()),								//uin
				(row.Child(5).ToElement()->GetText() != nullptr)					//claimed
			}
		);

	}

	loader->setNotifRows(result);

}
