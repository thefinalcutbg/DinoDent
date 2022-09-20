#include "NhifProcedures.h"
#include <JsonCpp/json.h>
#include <tuple>

#include "Resources.h"

std::vector<ProcedureTemplate> _procedures;
std::unordered_map<int, int> code_durations;
std::unordered_map<int, int> _timeframes;
std::vector<NZOKUpdates> _updates;

std::unordered_set<int> minor_only;
std::unordered_set<int> temp_only;
std::unordered_set<int> perma_only;


void NhifProcedures::initialize()
{

	Json::Reader reader;
	Json::Value p;

	reader.parse(Resources::nzokDataJson(), p);

	//1.Getting all NZOK procedures

	const Json::Value& procedure = p["procedure"];

	_procedures.reserve(procedure.size());
	code_durations.reserve(procedure.size());

	for (int i = 0; i < procedure.size(); i++) 
	{
		ProcedureTemplate m;
		m.type = static_cast<ProcedureTemplateType>(procedure[i]["type"].asInt());
		m.code = procedure[i]["code"].asInt();
		m.name = procedure[i]["name"].asString();
		m.price = 0;
		m.nhif = true;
		m.ksmp = procedure[i]["ksmp"].asString();

		if (!procedure[i]["default_diag"].isNull())
			m.diagnosis = procedure[i]["default_diag"].asString();

		if (!procedure[i]["material"].isNull())
			m.material = procedure[i]["material"].asString();

		code_durations[m.code] = (procedure[i]["duration"].asInt());
		_procedures.push_back(m);
	}

	//2.Getting the constraints procedures

	const Json::Value& constraints = p["constraints"];

	for (auto& m_o : constraints["minor_only"]) minor_only.emplace(m_o.asInt());

	for (auto& t_o : constraints["temp_only"]) temp_only.emplace(t_o.asInt());

	for (auto& p_o : constraints["perma_only"]) perma_only.emplace(p_o.asInt());

	for (auto& timeframe : constraints["timeframe"])
	{
		for (auto& codes : timeframe["codes"])
			_timeframes[codes.asInt()] = timeframe["limit"].asInt();
	}

	//3.Getting the actual updates

	const Json::Value& updates = p["updates"];

	for (auto& u : updates)
	{
		NZOKUpdates c;		

		//3.1 Getting the date of the update

			c.date = Date{ u["date"].asString() };

		//3.2 Getting the maximum allowed packages

			const Json::Value& packages = u["packages"]; 

			c.packages.reserve(packages.size());

			for (auto& package_json : packages)
			{

					c.packages.emplace_back();
					auto& package = c.packages.back();

					const Json::Value& codes = package_json["codes"];
					for (auto& code : codes) package.codes.insert(code.asInt());
					package.limit_adult = package_json["adult"].asInt();
					package.limit_minor = package_json["minor"].asInt();

					if(!package_json["pregnant"].isNull())
					package.limit_pregnant = package_json["pregnant"].asInt();

					package.timeframe = package_json["timeframe"].asInt();
			
			}


		//3.3 Getting the price list
			const Json::Value& pList = u["price_list"]; 

		for (auto& priceMap : pList)
		{

					const Json::Value& procedures = priceMap["procedures"];
					const Json::Value& nzok_price = priceMap["nzok"];
					const Json::Value& patient_price = priceMap["patient"];

					PriceObj price_value;

					price_value.bigCode = priceMap["bigCode"].asInt();

					for (int i = 0; i < procedures.size(); i++)
					{
						int m_code = procedures[i].asInt();
						double nhif = nzok_price[i].asDouble();
						double patient = patient_price[i].asDouble();

						price_value.priceMap[m_code] = std::make_pair(patient, nhif);
					}

					const Json::Value& specialty = priceMap["specialty"];
					const Json::Value& adult = priceMap["adult"];
					const Json::Value& unfav = priceMap["specification"];

					for (auto& spec : specialty)
					{

						PriceKey key{ spec.asInt(), adult.asBool(), unfav.asInt() };
						c.prices[key] = price_value;
					}

		}

		_updates.push_back(c);

	}

}





std::vector<ProcedureTemplate> NhifProcedures::getM_Templates(Date ambDate, NhifSpecialty specialty, bool adult, NhifSpecification specification)
{
	int currentUpdateIdx = -1;

	for (int i = 0; i < _updates.size(); i ++)
	{
		if (ambDate < _updates[i].date) continue;

		currentUpdateIdx = i; break;
	}

	if (currentUpdateIdx == -1) return std::vector<ProcedureTemplate>{};

	auto& update = _updates[currentUpdateIdx];

	auto& m_map = update.prices[PriceKey{ static_cast<int>(specialty), adult, static_cast<int>(specification) }].priceMap;

	std::vector<ProcedureTemplate> product;

	product.reserve(m_map.size());

	for (auto& kv : m_map) 
	{
		for (auto& p : _procedures)
		{
			if (p.code != kv.first) continue;

			product.push_back(p);
			product.back().price = std::get<0>(kv.second);
		}

	}

	return product;
}

std::pair<patientPrice, nzokPrice> NhifProcedures::getPrices
(int code, Date date, bool adult, NhifSpecialty doctorSpecialty, NhifSpecification specification)
{
	int currentUpdateIdx = -1;

	for (int i = 0; i < _updates.size(); i++)
	{
		if (date < _updates[i].date) continue;

		currentUpdateIdx = i; break;
	}

	//or throw an exception maybe?!
	if (currentUpdateIdx == -1) return std::make_pair(0,0);

	return _updates[currentUpdateIdx].prices[PriceKey{ static_cast<int>(doctorSpecialty), adult, static_cast<int>(specification) }].
		priceMap[code];
}

std::vector<ProcedurePackage> NhifProcedures::getPackages(Date ambDate)
{
	for (auto& u : _updates) 
		if (ambDate > u.date) 
			return u.packages;

	return std::vector<ProcedurePackage>();
}


ProcedureTemplate NhifProcedures::getTemplateByCode(int code){ 

	for (auto& p : _procedures) if (p.code == code) return p;

	throw std::exception("No procedure with this code");
}

double NhifProcedures::getPatientPrice(int code, Date date, NhifSpecialty specialty, bool adult, NhifSpecification specification)
{ return std::get<0>(getPrices(code, date, adult, specialty, specification)); }

double NhifProcedures::getNZOKPrice(int code, Date date, NhifSpecialty specialty, bool adult, NhifSpecification specification)
{ return std::get<1>(getPrices(code, date, adult, specialty, specification)); }

int NhifProcedures::getDuration(int nzokCode) { return code_durations[nzokCode]; }

int NhifProcedures::getYearLimit(int nzokCode) { return _timeframes.count(nzokCode) ? _timeframes[nzokCode] : 0; }

bool NhifProcedures::isTempOnly(int code) { return temp_only.count(code); }

bool NhifProcedures::isPermaOnly(int code) { return perma_only.count(code); }

bool NhifProcedures::isMinorOnly(int code) { return minor_only.count(code); }
