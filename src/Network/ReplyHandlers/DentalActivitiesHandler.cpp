#include "DentalActivitiesHandler.h"
#include "TinyXML/tinyxml.h"
#include <vector>
#include "Model/Tooth/ToothUtils.h"
#include "Presenter/ListPresenter/ListPresenter.h"
DentalActivitiesHandler::DentalActivitiesHandler(ListPresenter* p) :
	p_presenter(p)
{
}


void DentalActivitiesHandler::getReply(const std::string& reply)
{

	if (p_presenter == nullptr) return;

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto table =
		docHandle
		.FirstChildElement()			  //envelope
		.Child(1)						  //body
		.FirstChildElement();			  //table
		
	std::vector<SimpleProcedure> result;

	//i is 1, since 0 is the table header (td)
	for(int i = 1; ; i++)
	{
		auto row = table.Child(i); //table row

		if (!row.ToElement()) {
			break;
		}


		SimpleProcedure p;

		p.date = Date::getDateFromXmlFormat(row.Child(0).ToElement()->GetText());
		p.code = row.Child(2).ToElement()->GetText();
		p.diagnosis = row.Child(4).ToElement()->GetText();
		p.tooth = std::stoi(row.Child(5).ToElement()->GetText());

		auto pair = ToothUtils::getArrayIdxAndTemp(p.tooth);
		p.idx = pair.first;
		p.temp = pair.second;

		result.push_back(p);
	}

	p_presenter->showPISActivities(result);

}
