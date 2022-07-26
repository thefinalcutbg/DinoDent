#include "PISServ.h"
#include "View/ModalDialogBuilder.h"
#include "PKCS11.h"
#include "Network.h"
#include <TinyXML/tinyxml.h>
#include "Model/FreeFunctions.h"
#include "XmlSigner.h"
#include "Base64Convert.h"

const char* personTypeArr[5]
{
	"",
	"egn",
	"lnch",
	"",
	"",
};


std::string PISQuery::dentalActivities(const std::string& id, int personType)
{

	std::string tag = personTypeArr[personType];

	return
		"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
				   "xmlns:ns3=\"http://pis.technologica.com/ws/\">"
		"<ns3:user>"
		"<ns3:" + tag + ">" + id + "</ns3:" + tag + ">"
		"</ns3:user>"
		"<ns3:from_clause>INYEAR_DENTAL_ACTS</ns3:from_clause>"

		"<ns3:orderby_clause>"
			"<ns1:ocolumn sort=\"DESC\">ACTIVITY_DATE</ns1:ocolumn>"
		"</ns3:orderby_clause>"

		"</ns3:query>"
		;
}

std::string PISQuery::NotifList(const std::string& rziCode)
{
	return 
		"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
				   "xmlns:ns3=\"http://pis.technologica.com/ws/\">"
			"<ns3:user><ns3:msp>" + rziCode + "</ns3:msp></ns3:user>"
			"<ns3:select_clause>"
					"<ns1:scolumn>MONTHNOTIF_ID_HASH</ns1:scolumn>" //needed to get the actual data
					"<ns1:scolumn>ACC_DATE</ns1:scolumn>"			//the notif date
					"<ns1:scolumn>FILE_TYPE_CODE</ns1:scolumn>"		//notification type
					"<ns1:scolumn>MSP_CODE</ns1:scolumn>"			//RZI code
					"<ns1:scolumn>UIN_NUMBER</ns1:scolumn>"			//LPK code
					"<ns1:scolumn>CLAIM_ID_HASH</ns1:scolumn>"		//has it been claimed?
			"</ns3:select_clause>"
			"<ns3:from_clause>MONTH_NOTIF_INFO</ns3:from_clause>"

			"<ns3:orderby_clause>"
                "<ns1:ocolumn sort=\"DESC\">ACC_DATE</ns1:ocolumn>"
           "</ns3:orderby_clause>"

		"</ns3:query>"
		;
}

std::string PISQuery::getNotificationData(const std::string& rziCode, const std::string& notifHash)
{
	return 
		"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
				   "xmlns:ns3=\"http://pis.technologica.com/ws/\">"

			"<ns3:user><ns3:msp>" + rziCode + "</ns3:msp></ns3:user>"

			"<ns3:select_clause>"
					"<ns1:scolumn>MONTHNOTIF_FILE_BYTES</ns1:scolumn>" //the actual data
			"</ns3:select_clause>"

			"<ns3:from_clause>MONTH_NOTIF_INFO</ns3:from_clause>"

			"<ns3:where_clause>"
					"<ns1:filter>"
						"<ns1:xfy oper=\"=\">"
							"<ns1:fcolumn>MONTHNOTIF_ID_HASH</ns1:fcolumn>"
							"<ns1:fvalue>" + notifHash + "</ns1:fvalue>"
						"</ns1:xfy>"
					"</ns1:filter>"
			"</ns3:where_clause>"

		"</ns3:query>"
		;
}



std::string PISQuery::sendInvoice(const std::string& data, const std::string& rziCode, FinancialDocType purpose)
{
	const char* purposeArr[4]{
		"FDOC_INV",
		"FDOC_DTNOTE",
		"FDOC_CTNOTE",
		""
	};

	return
	"<ns1:userFile xmlns:ns1=\"http://pis.technologica.com/ws/\" "
				  "xmlns:ns2=\"http://pis.technologica.com/files/\">"

		"<ns1:user><ns1:msp>"+ rziCode + "</ns1:msp></ns1:user>"

			"<ns1:file>"
				"<ns2:base64>" + Base64Convert::encode(data.data(), data.size()) + "</ns2:base64>"
				"<ns2:type>xml</ns2:type>"
				"<ns2:encoding>utf-8</ns2:encoding>"
			"</ns1:file>"
			"<ns1:purpose>" + purposeArr[static_cast<int>(purpose)] + "</ns1:purpose>"

	"</ns1:userFile>"
	;
}

std::string PISQuery::sendAmbReport(const std::string& data, const std::string& doctorEgn)
{
	return
		"<ns1:userFile xmlns:ns1=\"http://pis.technologica.com/ws/\" "
		"xmlns:ns2=\"http://pis.technologica.com/files/\">"

		"<ns1:user><ns1:egn>" + doctorEgn + "</ns1:egn></ns1:user>"

		"<ns1:file>"
		"<ns2:base64>" + Base64Convert::encode(data.data(), data.size()) + "</ns2:base64>"
		"<ns2:type>xml</ns2:type>"
		"<ns2:encoding>utf-8</ns2:encoding>"
		"</ns1:file>"
		"<ns1:purpose>AMB_DENT</ns1:purpose>"

		"</ns1:userFile>"
		;
}

std::string PISQuery::activeHIRBNo(const std::string& id, int personType)
{
	std::string tag = personTypeArr[personType];

	return
		"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
				   "xmlns:ns3=\"http://pis.technologica.com/ws/\">"
		"<ns3:user>"
		"<ns3:" + tag + ">" + id + "</ns3:" + tag + ">"
		"</ns3:user>"
		"<ns3:from_clause>ACTIVE_HB</ns3:from_clause>"
		"</ns3:query>"
		;

}


bool PIS::sendRequest(const std::string& query, AbstractReplyHandler& handler, SOAPAction header)
{

/*
Since both xmlSec and qt network manager adopt the private key and release it on their own
we have to create two PKCS11 instances - one for the signing and one for the SSL connection
*/

	PKCS11 signer;

	if (!signer.hsmLoaded())
	{
		ModalDialogBuilder::showMessage(u8"Не е открит КЕП");
		//Network::clearAccessCache();
		return false;
	}

	if (signer.loginRequired()) {

		Network::clearAccessCache();
		auto pin = ModalDialogBuilder::pinPromptDialog(signer.pem_x509cert());

		if (pin.empty()) {
			return false;
		}

	
		if (!signer.login(pin))
		{
			ModalDialogBuilder::showError(u8"Грешна парола или блокирана карта");
			return false;
		};
	}

	//creating another instance for the SSL certificate
	PKCS11 clientSsl;

	auto signedRequest = XmlSigner::signPisQuery(
		query,
		signer.takePrivateKey(),
		signer.pem_x509cert()
	);

	std::string soapActionHeader;

	switch (header)
	{
	case SOAPAction::View: 
		soapActionHeader = "\"http://pis.technologica.com/view\""; 
		break;

	case SOAPAction::Files:
		soapActionHeader = "\"http://pis.technologica.com/files/\"";
		break;

	}


	Network::sendRequestToPis(
		signedRequest,
		clientSsl,
		&handler,
		soapActionHeader.c_str()
	);

	//XmlSigner::cleanup();

	return true;
	
}

#include "Model/Patient.h"
#include "Model/User/User.h"
#include <QTime>

bool PIS::insuranceRequest(AbstractReplyHandler& handler, const Patient& p, const Date& date)
{
	auto& practice = User::practice();

	if (!practice.nzok_contract.has_value() ||
		practice.nzok_contract->nra_pass.empty()
	) 
	{
		ModalDialogBuilder::showMessage(u8"Не е въведена парола за достъп към НАП");
		return false;
	}

	auto time =
		date.toXMLString() +
		"T" +
		QTime::currentTime().toString("hh:mm:ss").toStdString();
		//+ ".0Z";

	std::string query{

		"<?xml version=\"1.0\" encoding=\"UTF-8\"?> "
			"<PIS_Request xmlns=\"request.ws.nhif\" "
						 "ReqDate=\"" +  time + "\" " +
						 "RespType = \"0\" "
						 "RCZNo=\"" + practice.rziCode + "\" "
						 "RCZEik = \"" + practice.bulstat + "\" "
						 "RCZPassword=\"" + practice.nzok_contract->nra_pass + "\">"
			 "<Patient>"
			  "<PIN>" + p.id + "</PIN>"
			  "<PINType>"+ std::to_string(p.type-1) + "</PINType>"
			  "<ChkDt>" + date.toXMLString() + "T00:00:00" + "</ChkDt>"
			"</Patient>"
			"</PIS_Request>"

	};

	Network::sendRequestToNra(query, &handler);

	return true;

}
