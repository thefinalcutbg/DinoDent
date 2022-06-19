#include "DentalActivitiesHandler.h"
#include "TinyXML/tinyxml.h"
#include <vector>
#include "Model/Tooth/ToothUtils.h"
#include "Presenter/ListPresenter/ListPresenter.h"
#include "Model/Procedure/MasterNZOK.h"
DentalActivitiesHandler::DentalActivitiesHandler(ListPresenter* p) :
	p_presenter(p)
{
}


void DentalActivitiesHandler::getReply(const std::string& reply)
{

	if (p_presenter == nullptr) return;

	if (reply.empty()) {
		ModalDialogBuilder::showError(u8"Неуспешна връзка със сървъра");
		p_presenter->setPISActivities({});
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

	std::vector<Procedure> result;

	//i is 1, since 0 is the table header (td)
	for(int i = 1; ; i++)
	{
		auto row = table.Child(i); //table row

		if (!row.ToElement()) {
			break;
		}
		
		auto [tooth, temp] = ToothUtils::getArrayIdxAndTemp(std::stoi(row.Child(5).ToElement()->GetText()));

		result.emplace_back(
			MasterNZOK::instance().getTemplateByCode(std::stoi(row.Child(2).ToElement()->GetText())),
			Date::getDateFromXmlFormat(row.Child(0).ToElement()->GetText()),
			row.Child(4).ToElement()->GetText(), //diagnosis
			tooth, 
			temp
		);

	}

	p_presenter->setPISActivities(result);

}
