﻿#pragma once
#include "AbstractReplyHandler.h"
#include "Model/Date.h"
#include "Model/Procedure/Procedure.h"
#include "TinyXML/tinyxml.h"
#include <vector>
#include "Model/Tooth/ToothUtils.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Procedure/MasterNZOK.h"

/*
	To use this handler make setPisActivities(const std::optional<Procedures>&)
	in template argument class

*/
template<typename T>
class DentalActivitiesHandler : public AbstractReplyHandler
{
	T* reciever;

public:
	DentalActivitiesHandler(T* reciever) : reciever(reciever) {};

	void getReply(const std::string& reply) override {

		if (reciever == nullptr) return;

		if (reply.empty()) {
			reciever->setPISActivities({});
			return;
		}

		TiXmlDocument doc;

		doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

		if (!doc.RootElement()) {
			reciever->setPISActivities({});
		}

		TiXmlHandle docHandle(&doc);

		auto table =
			docHandle
			.FirstChildElement()			  //envelope
			.Child(1)						  //body
			.FirstChildElement();			  //table

		std::vector<Procedure> result;

		//i is 1, since 0 is the table header (td)
		for (int i = 1; ; i++)
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

		reciever->setPISActivities(result);
	
	}

};
