#pragma once
#include "AbstractReplyHandler.h"
#include "View/ModalDialogBuilder.h"
#include <QDebug>
#include <TinyXML/tinyxml.h>
#include "Model/InsuranceStatus.h"

//the template class has to implement void setInsuranceStatus(std::optional<InsuranceStatus>)
template<class T>
class NraReplyHandler : public AbstractReplyHandler
{
	T* reciever;

public:
	NraReplyHandler(T* reciever) : reciever(reciever) {};

	void getReply(const std::string& reply) override
	{
		if (reciever == nullptr) return;
		TiXmlDocument doc;

		doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

		if (!doc.RootElement()) {
			reciever->setInsuranceStatus({});
			ModalDialogBuilder::showError(u8"Неуспешна връзка с НАП");
			return;
		}

		TiXmlHandle statusHandle(&doc);

		auto patient =
			statusHandle
			.FirstChildElement()			  //response
			.FirstChildElement();			  //patient OR error


		if (patient.FirstChild().ToElement() == nullptr) {
			reciever->setInsuranceStatus({});
			return;
		}

		if (patient.ToElement()->ValueStr() == "ns0:Error") {
			ModalDialogBuilder::showError(
				patient.Child(1).ToElement()->GetText()
			);
			reciever->setInsuranceStatus({});
			return;
		}

		int insuredCode = std::stoi(patient.Child(0).ToElement()->GetText());

		switch(insuredCode)
		{
		case 1:  reciever->setInsuranceStatus(InsuranceStatus{ Insured::Yes }); break;

		case 2: 
			{
				InsuranceStatus result{ Insured::No };
				
				for (int i = 3; ; i++) {

					auto period = patient.Child(i).ToElement();

					if (!period) break;

					int year{0}, month{0};
					period->Attribute("Mnt", &month);
					period->Attribute("Yr", &year);

					if (!year || !month) break;

					result.yearsMonths[year].push_back(month);
				}

				reciever->setInsuranceStatus({result});
				break;
			}
			default:  reciever->setInsuranceStatus({}); break;
		}

	}

};

