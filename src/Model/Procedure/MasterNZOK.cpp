#include "MasterNZOK.h"
#include "C:/Dev/TorqueMVP/jsoncpp/jsoncpp-src-0.5.0/include/json/json.h"
#include <fstream>
#include <tuple>

MasterNZOK MasterNZOK::_instance;

MasterNZOK::MasterNZOK()
{
}

void MasterNZOK::loadData()
{
	std::ifstream ifs("nzok.json");
	Json::Reader reader;
	Json::Value m;

	reader.parse(ifs, m);


	const Json::Value& procedure = m["manipulation"];

	_procedures.reserve(procedure.size());
	code_durations.reserve(procedure.size());

	for (int i = 0; i < procedure.size(); i++)
	{
		ProcedureTemplate m;
		m.type = static_cast<ProcedureType>(procedure[i]["type"].asInt());
		m.code = procedure[i]["code"].asInt();
		m.name = procedure[i]["name"].asString();
		m.price = -1;
		m.nzok = true;

		if (!procedure[i]["default_diag"].isNull())
			m.diagnosis = procedure[i]["default_diag"].asString();

		if (!procedure[i]["material"].isNull())
			m.material = procedure[i]["material"].asString();

		code_durations[m.code] = (procedure[i]["duration"].asInt());
		_procedures[m.code] = m;
	}

	const Json::Value& constraints = m["constraints"];

	for (auto& m_o : constraints["minor_only"]) minor_only.emplace(m_o.asInt());

	for (auto& t_o : constraints["temp_only"]) temp_only.emplace(t_o.asInt());

	for (auto& p_o : constraints["perma_only"]) perma_only.emplace(p_o.asInt());

	const Json::Value& updates = m["updates"];

	for (auto& u : updates)
	{
		CurrentPrices c;							//the current update
		c.date = Date{ u["date"].asString() };

		const Json::Value& pList = u["price_list"]; //current update price_list

		for (auto& priceMap : pList)
		{

			const Json::Value& manipulations = priceMap["manipulations"];
			const Json::Value& nzok_price = priceMap["nzok"];
			const Json::Value& patient_price = priceMap["patient"];

			PriceObj price_value;

			price_value.bigCode = priceMap["bigCode"].asInt();

			for (int i = 0; i < manipulations.size(); i++)
			{
				int m_code = manipulations[i].asInt();
				double nzok = nzok_price[i].asDouble();
				double patient = patient_price[i].asDouble();

				price_value.priceMap[m_code] = std::make_pair(patient, nzok);
			}

			const Json::Value& specialty = priceMap["specialty"];
			const Json::Value& adult = priceMap["adult"];
			const Json::Value& unfav = priceMap["unfav"];

			for (auto& spec : specialty)
			{

				PriceKey key{ spec.asInt(), adult.asBool(), unfav.asBool() };
				c.prices[key] = price_value;
			}

		}

		updatesVec.push_back(c);

	}
}

MasterNZOK& MasterNZOK::instance()
{
	return _instance;
}

void MasterNZOK::loadUpdates()
{
	_instance.loadData();
}



std::vector<ProcedureTemplate> MasterNZOK::getM_Templates(Date ambDate, int specialty, bool adult, bool unfav)
{
	int currentUpdateIdx = -1;

	for (int i = 0; i < updatesVec.size(); i ++)
	{
		if (ambDate < updatesVec[i].date) continue;

		currentUpdateIdx = i; break;
	}

	if (currentUpdateIdx == -1) return std::vector<ProcedureTemplate>{};

	auto& update = updatesVec[currentUpdateIdx];

	auto& m_map = update.prices[PriceKey{ specialty, adult, unfav }].priceMap;

	std::vector<ProcedureTemplate> product;

	product.reserve(m_map.size());

	for (auto& kv : m_map)
	{
		product.push_back(_procedures[kv.first]);
		product.back().price = std::get<0>(kv.second);
	}

	return product;
}

std::pair<patientPrice, nzokPrice> MasterNZOK::getPrices
	(
		int code, 
		Date ambDate, 
		int specialty, 
		bool adult,
		bool unfav
	)
{
	int currentUpdateIdx = -1;

	for (int i = 0; i < updatesVec.size(); i++)
	{
		if (ambDate < updatesVec[i].date) continue;

		currentUpdateIdx = i; break;
	}

	if (currentUpdateIdx == -1) throw "No update entity";

	return updatesVec[currentUpdateIdx].prices[PriceKey{ specialty, adult, unfav }].
		priceMap[code];
}

ProcedureTemplate MasterNZOK::getTemplateByCode(int code)
{
	return _procedures[code];
}

double MasterNZOK::getPatientPrice(int code, Date date, int specialty, bool adult, bool unfav)
{
	return std::get<0>(getPrices(code, date, specialty, adult, unfav));
}

double MasterNZOK::getNZOKPrice(int code, Date date, int specialty, bool adult, bool unfav)
{
	return std::get<1>(getPrices(code, date, specialty, adult, unfav));
}

int MasterNZOK::getDuration(int nzokCode)
{
	return code_durations[nzokCode];
}

bool MasterNZOK::isTempOnly(int code)
{
	return temp_only.count(code);
}

bool MasterNZOK::isPermaOnly(int code)
{
	return perma_only.count(code);
}

bool MasterNZOK::isMinorOnly(int code)
{
	return minor_only.count(code);
}
