#include "InvoiceXML.h"

#include "Libraries/JsonCpp/json.h"
#include <array>
#include <fstream>
#include <stdexcept>

constexpr int recipientCount = 28;

std::array<Recipient, recipientCount> recipients;
bool recipients_initialized{ false };

void initializeRecipients()
{
	std::ifstream ifs("data/recipients.json");
	Json::Value recipientsJson = Json::arrayValue;

	Json::Reader reader;
	bool parsingSuccessful = reader.parse(ifs, recipientsJson);

	if (recipientsJson.size() != recipients.size() || !parsingSuccessful) {
		throw std::invalid_argument("Parsing failed");
	}

	for (int i = 0; i < recipientCount; i ++)
	{
		recipients[i].code = recipientsJson[i]["code"].asString();
		recipients[i].name = recipientsJson[i]["name"].asString();
		recipients[i].address = recipientsJson[i]["address"].asString();
		recipients[i].bulstat = recipientsJson[i]["bulstat"].asString();
	}
}

const std::string_view insuranceFunds[46]
{
"NONE",
"NZOK",
"MZ",
"NZOK",
"MZ",
"NZOK",
"NZOK",
"NZOK",
"NZOK",
"MZ",
"MZ",
"MZ",
"NZOK",
"NZOK",
"MZ",
"MZ",
"NZOK",
"NZOK",
"MZ",
"NZOK",
"NZOK",
"MZ",
"ASP",
"ASP",
"NZOK",
"NZOK",
"MZ",
"MZ",
"MZ",
"MZ",
"MZ",
"NZOK",
"NZOK",
"NZOK",
"MZ",
"NZOK",
"ASP",
"MZ",
"NZOK",
"MZ",
"NZOK",
"MZ",
"NZOK",
"MZ",
"NZOK",
"MZ"
};



const Recipient& getRecipient(const std::string& RHIF)
{
	if (!recipients_initialized)
	{
		initializeRecipients();
		recipients_initialized = true;
	}


	int idx = std::stoi(RHIF);
	if(idx > 28 || idx < 1)
		throw std::invalid_argument("Invalid RHIF");

	return recipients[idx];
}

std::string getInsuranceFund(int activityTypeCode)
{
	if(activityTypeCode < 1 || activityTypeCode > 45)
		throw std::invalid_argument("Invalid activityCode");

	return insuranceFunds[activityTypeCode].data();
}
