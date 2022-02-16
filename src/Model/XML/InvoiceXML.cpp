#include "InvoiceXML.h"

#include <array>
#include <fstream>
#include "Model/User/User.h"
#include <stdexcept>

constexpr int recipientCount = 28;

std::array<Recipient, recipientCount> recipients
{{
    {
        "01",
        u8"РЗОК Благоевград",
        u8"гр. Благоевград, пл. “Георги Измирлиев” 9",
        "1218582201431"
    },
    {
        "02",
        u8"РЗОК Бургас",
        u8"гр. Бургас, Парк “Езеро”",
        "1218582201446"
    },
    {
        "03",
        u8"РЗОК Варна",
        u8"гр. Варна, бул. “Цар Освободител” 76-Г",
        "1218582201450"
    },
    {
        "04",
        u8"РЗОК Велико Търново",
        u8"гр. Велико Търново, ул. “Ивайло” 2",
        "1218582201465"
    },
    {
        "05",
        u8"РЗОК Видин",
        u8"гр. Видин, бул. “Панония” 2",
        "1218582201471"
    },
    {
        "06",
        u8"РЗОК Враца",
        u8"гр. Враца, ул. “Иваница Данчов” 2",
        "1218582201484"
    },
    {
        "07",
        u8"РЗОК Габрово",
        u8"гр. Габрово, ул.“Отец Паисий” 25",
        "1218582201499"
    },
    {
        "08",
        u8"РЗОК Добрич",
        u8"гр. Добрич, ул. “Независимост” 5, ет. 4",
        "1218582201656"
    },
    {
        "09",
        u8"РЗОК Кърджали",
        u8"гр. Кърджали, бул. “Тракия” 19",
        "1218582200015"
    },
    {
        "10",
        u8"РЗОК Кюстендил",
        u8"гр. Кюстендил, ул. “Демокрация” 44, вх. Б",
        "1218582201515"
    },
    {
        "11",
        u8"РЗОК Ловеч",
        u8"гр. Ловеч, ул. “Кубрат” 7",
        "1218582201524"
    },
    {
        "12",
        u8"РЗОК Монтана",
        u8"гр. Монтана, бул. “Трети март” 41",
        "1218582201534"
    },
    {
        "13",
        u8"РЗОК Пазарджик",
        u8"гр. Пазарджик, ул. “11 август” 2",
        "1218582201549"
    },
    {
        "14",
        u8"РЗОК Перник",
        u8"гр. Перник, пл. “Кракра Пернишки” 2, ет. 6",
        "1218582201553"
    },
    {
        "15",
        u8"РЗОК Плевен",
        u8"гр. Плевен, ул. “Княз Александър Батенберг I” 7",
        "1218582201568"
    },
    {
        "16",
        u8"РЗОК Пловдив",
        u8"гр. Пловдив, ул. “Христо Чернопеев” 14",
        "1218582201572"
    },
    {
        "17",
        u8"РЗОК Разград",
        u8"гр. Разград, бул. “България” 36",
        "1218582200023"
    },
    {
        "18",
        u8"РЗОК Русе",
        u8"гр. Русе, ул. “Райко Даскалов” 16",
        "1218582201587"
    },
    {
        "19",
        u8"РЗОК Силистра",
        u8"гр. Силистра, ул. “Партений Павлович” 3",
        "1218582201618"
    },
    {
        "20",
        u8"РЗОК Сливен",
        u8"гр. Сливен, ул. „Самуил” 1, НТС, ет. 4 - ет. 8",
        "1218582201603"
    },
    {
        "21",
        u8"РЗОК Смолян",
        u8"гр. Смолян, бул. “България” 7",
        "1218582201591"
    },
    {
        "22",
        u8"СЗОК",
        u8"гр. София, ул. “Енос” 10, вх. Б",
        "1218582201412"
    },
    {
        "23",
        u8"РЗОК София - област",
        u8"гр. София, ул. “Енос” 10, вх. Б",
        "1218582201427"
    },
    {
        "24",
        u8"РЗОК Стара Загора",
        u8"гр. Стара Загора, ул. “Цар Иван Шишман” 54А",
        "1218582201622"
    },
    {
        "25",
        u8"РЗОК Търговище",
        u8"гр. Търговище, ул. “Трайко Китанчев” 37",
        "1218582201637"
    },
    {
        "26",
        u8"РЗОК Хасково",
        u8"гр. Хасково, пл. “Градска болница” 1",
        "1218582201675"
    },
    {
        "27",
        u8"РЗОК Шумен",
        u8"гр. Шумен, ул. “Цар Освободител” 102",
        "1218582201641"
    },
    {
        "28",
        u8"РЗОК Ямбол",
        u8"гр. Ямбол, ул. “Д-р П. Брънеков” 1",
        "1218582201660"
    }
}};



constexpr const char* insuranceFunds[46]
{
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


const Recipient& getRecipient(const std::string& practiceRZI)
{

	int idx = std::stoi(practiceRZI.substr(0, 2));
	if(idx > 28 || idx < 1)
		throw std::invalid_argument("Invalid RHIF");

	return recipients[idx-1];
}



std::string getInsuranceFund(int activityTypeCode)
{

	if(activityTypeCode < 1 || activityTypeCode > 45)
		throw std::invalid_argument("Invalid activityCode");

	return insuranceFunds[activityTypeCode-1];
}

#include "Libraries/TinyXML/tinyxml.h"

IssuerType getIssuerType(int legalEntity, const Doctor& doctor)
{

	const char* legalForms[5]{
		u8"ET",u8"ЕООД",u8"ООД",u8"ЕАД",u8"АД"
	};

	switch (legalEntity)
	{
	case 0:
		return SelfInsured{
            u8"Декларация по чл.43,ал.5 от ЗДДФЛ, че съм самоосигуряващо се лице по смисъла на КСО",
            PersonInfo{
                doctor.egn,
                doctor.fname,
                doctor.mname,
                doctor.lname
            }
        };
	default:
		return Company {legalForms[legalEntity - 1]} ;
	}
}

Issuer::Issuer(const User& user) :
	type							    { getIssuerType(user.practice.legal_entity, user.doctor) },
	company_name						{ user.practice.name },
	address_by_contract					{ user.practice.firm_address },
    address_by_activity                 { user.practice.practice_address },
	registration_by_VAT					{ user.practice.vat },
	grounds_for_not_charging_VAT		{ registration_by_VAT ? u8"Чл. 39 от ЗДДС" : u8"Чл.113,ал.9 от ЗДДС"},
	bulstat						        { user.practice.bulstat },
	contract_no							{ user.practice.nzok_contract.value().contract_no },
	contract_date						{ user.practice.nzok_contract.value().date },
	rhi_nhif_no							{ user.practice.rziCode.substr(0, 2) }
{
}


Invoice::Invoice(const TiXmlDocument& monthNotif, const User& user)
	:
	fin_document_type_code			{monthNotif.RootElement()->FirstChildElement("inv_type_code")->GetText() },
	fin_document_month_no			{monthNotif.RootElement()->FirstChildElement("monthly_notification_num")->GetText() },
	
    mainDocument{           
        fin_document_type_code == "INVOICE"
        ?
        std::optional<MainDocument>{} 
        :
        MainDocument{
        monthNotif.RootElement()->FirstChildElement("from_inv_num")->GetText(),
        Date::getDateFromXmlFormat(monthNotif.RootElement()->FirstChildElement("from_inv_date")->GetText())
        }
    },

	activityTypeCode				{std::stoi(monthNotif.RootElement()->FirstChildElement("nhif_type_code")->GetText())},
	health_insurance_fund_type_code	{getInsuranceFund(activityTypeCode) },
	date_from						{Date::getDateFromXmlFormat(monthNotif.RootElement()->FirstChildElement("date_from")->GetText())},
	date_to							{Date::getDateFromXmlFormat(monthNotif.RootElement()->FirstChildElement("date_to")->GetText())},
	recipient						{getRecipient(user.practice.rziCode)},
	issuer							{user}
{

	for (
		const TiXmlElement* business_operation = monthNotif.RootElement()->FirstChildElement("Monthly_Notification_Details");
		business_operation != NULL;
		business_operation = business_operation->NextSiblingElement("Monthly_Notification_Details")
		)
	{
		businessOperations.emplace_back(

			BusinessOperation{
				business_operation->FirstChildElement("activity_code")->GetText(),
				business_operation->FirstChildElement("activity_name")->GetText(),
				std::stof(business_operation->FirstChildElement("quantity")->GetText()),
				std::stoi(business_operation->FirstChildElement("unit_price")->GetText()),
				std::stof(business_operation->FirstChildElement("value_price")->GetText()),
			}
		);
	}

    for (auto& operation : businessOperations)
    {
        aggragated_amounts.payment_amount += operation.value_price;
    }

    aggragated_amounts.total_amount = aggragated_amounts.payment_amount;
    aggragated_amounts.taxEventDate = date_from; //not sure about this!

}



