#include "NraStatusService.h"
#include "View/ModalDialogBuilder.h"
#include "Network/NetworkManager.h"
#include "Model/Patient.h"

void NraStatusService::parseReply(const std::string& reply)
{
	
		TiXmlDocument doc;

		if (reply.empty()) return;

		doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

		if (!doc.RootElement()) return;

		TiXmlHandle statusHandle(&doc);

		auto patient =
			statusHandle
			.FirstChildElement()			  //response
			.FirstChildElement();			  //patient OR error


		if (patient.FirstChild().ToElement() == nullptr) {
			m_callback({});
			return;
		}

		if (patient.ToElement()->ValueStr() == "ns0:Error") {
			ModalDialogBuilder::showError(
				patient.Child(1).ToElement()->GetText()
			);
			m_callback({});
			return;
		}

		int insuredCode = std::stoi(patient.Child(0).ToElement()->GetText());

		switch(insuredCode)
		{
		case 1:  m_callback(InsuranceStatus{ Insured::Yes }); break;

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

				m_callback({result});
				break;
			}
		default:  m_callback(InsuranceStatus{Insured::NoData}); break;
		}

}

#include <QDateTime>

bool NraStatusService::sendRequest(const Patient& p, std::function<void(const std::optional<InsuranceStatus>&)> callback, bool showDialogs, const Date& date )
{
	m_callback = callback;

	this->show_dialogs = showDialogs;

	auto& practice = User::practice();

	if (!practice.hasNraAccess())
	{
		if (showDialogs) {
			ModalDialogBuilder::showMessage("Не е въведена парола за достъп към НАП");
		}
		return false;
	}

	auto time =
		date.to8601() +
		"T" +
		QTime::currentTime().toString("hh:mm:ss").toStdString();
	//+ ".0Z";

	std::string query{

		"<?xml version=\"1.0\" encoding=\"UTF-8\"?> "
			"<PIS_Request xmlns=\"request.ws.nhif\" "
						 "ReqDate=\"" + time + "\" " +
						 "RespType = \"0\" "
						 "RCZNo=\"" + practice.rziCode + "\" "
						 "RCZEik = \"" + practice.bulstat + "\" "
						 "RCZPassword=\"" + practice.nhif_contract->nra_pass + "\">"
			 "<Patient>"
			  "<PIN>" + p.id + "</PIN>"
			  "<PINType>" + std::to_string(p.type - 1) + "</PINType>"
			  "<ChkDt>" + date.to8601() + "T00:00:00" + "</ChkDt>"
			"</Patient>"
			"</PIS_Request>"

	};

	awaiting_reply = true;

	NetworkManager::sendRequestToNra(query, this);

	return true;
}
