#include "NhifProcedures.h"
#include <json/json.h>
#include <tuple>
#include "Resources.h"
#include "Model/UserStructs.h"

std::unordered_map<int, int> code_durations;
std::unordered_map<int, int> _timeframes;
std::vector<NRD> m_NRDlist;

std::unordered_set<int> minor_only;
std::unordered_set<int> temp_only;
std::unordered_set<int> perma_only;


void NhifProcedures::initialize()
{

	Json::Reader reader;
	Json::Value p;

	reader.parse(Resources::nhifDataJson(), p);

	//1.Getting all NZOK procedures

	const Json::Value& dur = p["durations"];

	code_durations.reserve(dur.size());

	for (int i = 0; i < dur["code"].size(); i++) 
	{
		code_durations[dur["code"][i].asInt()] = dur["minutes"][i].asInt();
	}

	//2.Getting the constraints procedures

	const Json::Value& constraints = p["constraints"];

	for (auto& m_o : constraints["minor_only"]) minor_only.emplace(m_o.asInt());

	for (auto& t_o : constraints["temp_only"]) temp_only.emplace(t_o.asInt());

	for (auto& p_o : constraints["perma_only"]) perma_only.emplace(p_o.asInt());

	for (auto& timeframe : constraints["timeframe"])
	{
		for (auto& codes : timeframe["codes"]) {

			_timeframes[codes.asInt()] = timeframe["limit"].asInt();
		}
	}

	//3.Getting the actual jsonNrd

	const Json::Value& jsonNrd = p["nrd"];

	for (auto& nrd : jsonNrd)
	{
		NRD contract;		

		//3.1 Getting the date of the effectiveNRD

			contract.date = Date{ nrd["date"].asString() };

		//3.2 Getting the maximum allowed packages

			const Json::Value& packages = nrd["packages"]; 

			contract.packages.reserve(packages.size());

			for (auto& package_json : packages)
			{

					contract.packages.emplace_back();
					auto& package = contract.packages.back();

					const Json::Value& codes = package_json["codes"];
					for (auto& code : codes) package.codes.insert(code.asInt());
					package.limit_adult = package_json["adult"].asInt();
					package.limit_minor = package_json["minor"].asInt();
					package.timeframe = package_json["timeframe"].asInt();
			
			}


		//3.3 Getting the price list
			const Json::Value& pList = nrd["price_list"]; 

		for (auto& priceMap : pList)
		{

					const Json::Value& jsonPorcedureCodes = priceMap["procedures"];
					const Json::Value& jsonNhifPrices = priceMap["nzok"];
					const Json::Value& jsonPatientPrices = priceMap["patient"];

					bool adult = priceMap["adult"].asBool();
					int specification = priceMap["specification"].asInt();
					int bigCode = priceMap["bigCode"].asInt();

					for (auto& spec : priceMap["specialty"]) {

						for (int i = 0; i < jsonPorcedureCodes.size(); i++)
						{
							int procedureCode = jsonPorcedureCodes[i].asInt();
							double nhifPrice = jsonNhifPrices[i].asDouble();
							double patientPrice = jsonPatientPrices[i].asDouble();

							auto key = PriceKey{
								.specialty = spec.asInt(),
								.adult = adult,
								.specification = specification,
								.procedure_code = procedureCode
							};

							auto value = PriceValue{
								.big_code = bigCode,
								.nhif_price = nhifPrice,
								.patient_price = patientPrice
							};

							contract.prices[key] = value;
						}
					}
		}

		m_NRDlist.push_back(contract);

	}

}

std::vector<ProcedureCode> NhifProcedures::getNhifProceduresLegacy(Date ambDate, NhifSpecialty specialty, bool adult, bool pregnancyAllowed, NhifSpecificationType specification)
{
	int currentUpdateIdx = -1;

    for (size_t i = 0; i < m_NRDlist.size(); i ++)
	{
		if (ambDate < m_NRDlist[i].date) continue;

		currentUpdateIdx = i; break;
	}

	if (currentUpdateIdx == -1) return {};

	auto& effectiveNRD = m_NRDlist[currentUpdateIdx];

	std::vector<ProcedureCode> result;

	result.reserve(10);

	auto key = PriceKey{
		.specialty = static_cast<int>(specialty),
		.adult = adult,
		.specification = static_cast<int>(specification),
		.procedure_code = 0
	};

	for (auto& [nhifCode, procedureCode] : ProcedureCode::procedureByNhifCodeLegacy()) {

		if (!pregnancyAllowed && nhifCode == 103) continue;

		key.procedure_code = nhifCode;

		if (effectiveNRD.prices.count(key)) {
			result.push_back(procedureCode);
		}
	}

	return result;
}

std::vector<ProcedureCode> NhifProcedures::getNhifProcedures(Date ambDate, NhifSpecialty specialty, bool adult, bool pregnancyAllowed, NhifSpecificationType specification)
{
	int currentUpdateIdx = -1;

	for (size_t i = 0; i < m_NRDlist.size(); i++)
	{
		if (ambDate < m_NRDlist[i].date) continue;

		currentUpdateIdx = i; break;
	}

	if (currentUpdateIdx == -1) return {};

	auto& effectiveNRD = m_NRDlist[currentUpdateIdx];

	std::vector<ProcedureCode> result;

	result.reserve(10);

	//FUCK NHIF AND THEIR NUMBERS
	static const std::vector<std::pair<int, std::string>> specMap[3] = {
		//NHIF PARTIAL
		{
			{101, "97017-00"},
			{101, "97017-01"},
			{301, "97546-00"}, //!!
			{301, "97546-01"}, //!!
			{332, "97423-00"}, //!!
			{333, "97423-01"}, //!!
			{508, "97311-09"},
			{509, "97311-10"},

			{102, "97016-00"},
			{520, "97300-00"},
			{510, "97301-00"},
			{544, "97020-00"},
			{832, "97710-00"},
			{833, "97710-01"},
			{834, "D-09-004"},
			{835, "D-09-005"}
		},

		//NHIF FULL
		{
			{101, "97017-00"},
			{101, "97017-01"},

			{301, "97546-03"}, //!!
			{301, "97546-02"}, //!!
			{332, "97423-02"}, //!!
			{333, "97423-03"}, //!!

			{508, "97311-09"},
			{509, "97311-10"},

			{102, "97016-00"},
			{520, "97300-00"},
			{510, "97301-00"},
			{544, "97020-00"},
			{832, "97710-00"},
			{833, "97710-01"},
			{834, "D-09-004"},
			{835, "D-09-005"}
		},

		//NHIF ANESTHESIA
		{
			
			{101, "97017-00"},
			{101, "97017-01"},
			{301, "97546-00"}, //!!
			{301, "97546-01"}, //!!
			{508, "97311-11"}, //!!
			{509, "97311-12"}, //!!
			{332, "97423-00"}, //!!
			{333, "97423-01"}, //!!
			{102, "97016-00"},
			{520, "97300-01"},
			{510, "97301-01"},
			{544, "97020-00"},
			{901, "D-01-007"},
			{902, "D-01-008"},
			{903, "D-01-009"}, //!!
		},
	};

	auto key = PriceKey{
		.specialty = static_cast<int>(specialty),
		.adult = adult,
		.specification = static_cast<int>(specification),
		.procedure_code = 0
	};
	
	for (auto& [nhifCode, procedureCode] : ProcedureCode::procedureByNhifCodeLegacy()) {

		key.procedure_code = nhifCode;

		if (effectiveNRD.prices.count(key)) {

			for (auto& [code, achi] : specMap[key.specification])
			{
				if (achi == "97017-01" && pregnancyAllowed == false) {
					continue;
				}

				if (nhifCode == code) result.emplace_back(achi);
			}
		}
	}

	return result;

}

std::pair<patientPrice, nzokPrice> NhifProcedures::getPrices
(int code, Date date, bool adult, NhifSpecialty doctorSpecialty, NhifSpecificationType specification)
{
	int currentUpdateIdx = -1;

	for (int i = 0; i < m_NRDlist.size(); i++)
	{
		if (date < m_NRDlist[i].date) continue;

		currentUpdateIdx = i; break;
	}

	//or throw an exception maybe?!
	if (currentUpdateIdx == -1) return std::make_pair(0,0);

	auto key = PriceKey{
		.specialty = static_cast<int>(doctorSpecialty),
		.adult = adult,
		.specification = static_cast<int>(specification),
		.procedure_code = code
	};

	auto& value = m_NRDlist[currentUpdateIdx].prices[key];

	return std::make_pair(value.patient_price, value.nhif_price);
}

std::vector<ProcedurePackage> NhifProcedures::getPackages(Date ambDate)
{
	for (auto& u : m_NRDlist) 
		if (ambDate > u.date) 
			return u.packages;

	return std::vector<ProcedurePackage>();
}

double NhifProcedures::getPatientPrice(int code, Date date, NhifSpecialty specialty, bool adult, NhifSpecificationType specification)
{ return std::get<0>(getPrices(code, date, adult, specialty, specification)); }

double NhifProcedures::getNhifPrice(int code, Date date, NhifSpecialty specialty, bool adult, NhifSpecificationType specification)
{ return std::get<1>(getPrices(code, date, adult, specialty, specification)); }

PriceValue NhifProcedures::getPriceValue(const PriceKey & key, const Date& date)
{
	int currentUpdateIdx = -1;

	for (int i = 0; i < m_NRDlist.size(); i++)
	{
		if (date < m_NRDlist[i].date) continue;

		currentUpdateIdx = i; break;
	}

	if (currentUpdateIdx == -1) return PriceValue{ 0,0,0 };

	if (!m_NRDlist[currentUpdateIdx].prices.count(key)) return PriceValue{ 0 ,0 ,0 };

	return m_NRDlist[currentUpdateIdx].prices.at(key);
}

int NhifProcedures::getDuration(int nzokCode) { return code_durations[nzokCode]; }

int NhifProcedures::getYearLimit(int nzokCode) { return _timeframes.count(nzokCode) ? _timeframes[nzokCode] : 0; }

bool NhifProcedures::isTempOnly(int code) { return temp_only.count(code); }

bool NhifProcedures::isPermaOnly(int code) { return perma_only.count(code); }

bool NhifProcedures::isMinorOnly(int code) { return minor_only.count(code); }
