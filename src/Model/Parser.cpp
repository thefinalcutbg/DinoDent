#include "Parser.h"
#include <JsonCpp/json.h>
#include "Model/Dental/ToothUtils.h"
#include "Model/Dental/ToothContainer.h"
#include "Model/Dental/PerioStatus.h"
#include "Model/Dental/Procedure.h"
#include "Model/TableStructs.h"
#include "Model/Financial/BusinessOperation.h"
#include "Model/Financial/Invoice.h"

std::string Parser::write(const PerioStatus& status)
{
	Json::Value json;
	
	json["PD"] = Json::Value(Json::arrayValue);
	for (auto& pd : status.pd)
		json["PD"].append(pd);

	json["CAL"] = Json::Value(Json::arrayValue);
	for (auto& cal : status.cal)
		json["CAL"].append(cal);

	json["BOP"] = Json::Value(Json::arrayValue);
	for (auto& bop : status.bop)
		json["BOP"].append(static_cast<int>(bop));
	
	json["AG"] = Json::Value(Json::arrayValue);
	for (auto& ag : status.ag)
		json["AG"].append(ag);
	
	json["Furc"] = Json::Value(Json::arrayValue);
	for (auto& furc : status.furcation)
		json["Furc"].append(furc);

	json["FMBS"] = Json::Value(Json::arrayValue);
	for (auto& FMBS : status.FMBS)
		json["FMBS"].append(static_cast<int>(FMBS));

	json["FMPS"] = Json::Value(Json::arrayValue);
	for (auto& FMPS : status.FMPS)
		json["FMPS"].append(static_cast<int>(FMPS));

	json["Disabled"] = Json::Value(Json::arrayValue);
	for (auto& d : status.disabled)
		json["Disabled"].append(static_cast<int>(d));

	json["Smoker"] = status.smoker;
	json["BoneLoss"] = status.boneLoss;
	json["Restore"] = status.completeRestorationNeeded;
	json["Sys"] = status.systemic;

	Json::FastWriter writer;

	return writer.write(json);
}


std::string Parser::write(const ToothContainer& status)
{
	Json::Value statusJson = Json::arrayValue;

	auto writeToothStatus = [](const Tooth& t) -> Json::Value
	{
		Json::Value toothJson;

		auto writeDentistMade = [](const DentistMade& dm) -> Json::Value
		{
			Json::Value result;
			result["LPK"] = dm.LPK;
			return result;
		};

		auto writeConstruction = [](const Construction& c) -> Json::Value
		{
			Json::Value result;
			result["LPK"] = c.LPK;
			result["Pos"] = static_cast<int>(c.position);
			return result;
		};


		toothJson["index"] = t.index;
		toothJson["temp"] = t.temporary.exists();

		if (t.healthy) toothJson["H"] = 1;
		if (t.pulpitis) toothJson["P"] = 1;
		if (t.lesion) toothJson["G"] = 1;
		if (t.calculus) toothJson["T"] = 1;
		if (t.fracture) toothJson["F"] = 1;
		if (t.root) toothJson["R"] = 1;
		if (t.impacted) toothJson["Re"] = 1;
		if (t.periodontitis) toothJson["Pa"] = 1;
		if (t.mobility) toothJson["M"] = static_cast<int>(t.mobility.degree);
		if (t.caries)
		{
			toothJson["C"] = Json::arrayValue;

			for (int i = 0; i < surfaceCount; i++)
			{
				if (!t.caries[i]) continue;
				Json::Value cariesJson;
				cariesJson["Surf"] = i;
				toothJson["C"].append(cariesJson);
			}
		}

		if (t.obturation)
		{
			toothJson["O"] = Json::arrayValue;

			for (int i = 0; i < surfaceCount; i++)
			{
				if (!t.obturation[i]) continue;
				Json::Value obturationJson;
				obturationJson["Surf"] = i;
				obturationJson["LPK"] = t.obturation[i].LPK;
				toothJson["O"].append(obturationJson);
			}
		}

		if (t.denture) toothJson["X"] = writeDentistMade(t.denture);
		if (t.endo) toothJson["Rc"] = writeDentistMade(t.endo);
		if (t.extraction) toothJson["E"] = writeDentistMade(t.extraction);
		if (t.implant) toothJson["I"] = writeDentistMade(t.implant);
		if (t.crown) toothJson["K"] = writeDentistMade(t.crown);
		if (t.post) toothJson["Rp"] = writeDentistMade(t.post);

		if (t.bridge) toothJson["B"] = writeConstruction(t.bridge);
		if (t.splint) toothJson["S"] = writeConstruction(t.splint);

		return toothJson;
	};

	for (auto& t : status)
	{
		if (t.noData()) continue;

		auto toothJson = writeToothStatus(t);
		if (t.dsn) {
			toothJson["D"] = writeToothStatus(t.dsn.tooth());
		}

		statusJson.append(toothJson);
	}

	Json::FastWriter writer;

	return writer.write(statusJson);

}


void Parser::parse(const std::string& jsonString, ToothContainer& status)
{
	Json::Value statusJson;
	Json::Reader reader;

	auto parseTooth = [](const Json::Value& t, Tooth& tooth) mutable
	{
		tooth.temporary.set(t["temp"].asBool());
		tooth.healthy.set(t.isMember("H"));
		tooth.pulpitis.set(t.isMember("P"));
		tooth.lesion.set(t.isMember("G"));
		tooth.calculus.set(t.isMember("T"));
		tooth.fracture.set(t.isMember("F"));
		tooth.root.set(t.isMember("R"));
		tooth.periodontitis.set(t.isMember("Pa"));
		tooth.impacted.set(t.isMember("Re"));

		tooth.mobility.set(t.isMember("M"));

		if (tooth.mobility) {
			tooth.mobility.degree = static_cast<Degree>(t["M"].asInt());
		}

		if (t.isMember("C"))
		{
			for (auto& c : t["C"])
			{
				tooth.caries.set(true, c["Surf"].asInt());
			}
		}

		if (t.isMember("O"))
		{
			for (auto& o : t["O"])
			{
				auto surface = o["Surf"].asInt();
				tooth.obturation.set(true, surface);
				tooth.obturation[surface].LPK = o["LPK"].asString();
			}
		}

		auto dentistMadeParser = [](const Json::Value& json, const char* numenclature)->DentistMade
		{
			if (!json.isMember(numenclature)) return {};

			DentistMade result;
			result.set(true);
			result.LPK = json[numenclature]["LPK"].asString();
			return result;
		};

		tooth.crown = dentistMadeParser(t, "K");
		tooth.implant = dentistMadeParser(t, "I");
		tooth.endo = dentistMadeParser(t, "Rc");
		tooth.extraction = dentistMadeParser(t, "E");
		tooth.post = dentistMadeParser(t, "Rp");
		tooth.denture = dentistMadeParser(t, "X");

		auto constructionParser = [](const Json::Value& json, const char* numenclature)->Construction
		{
			if (!json.isMember(numenclature)) return {};

			Construction result;
			result.set(true);
			result.LPK = json[numenclature]["LPK"].asString();
			result.position = static_cast<BridgePos>(json[numenclature]["Pos"].asInt());
			return result;
		};

		tooth.bridge = constructionParser(t, "B");
		tooth.splint = constructionParser(t, "S");
	};

	if(!reader.parse(jsonString, statusJson)) return;

	for (auto& toothJson : statusJson)
	{
		auto idx = toothJson["index"].asInt();

		auto& tooth = status[idx];

		parseTooth(toothJson, tooth);

		if (toothJson.isMember("D"))
		{
			tooth.dsn.set(true);
			parseTooth(toothJson["D"], tooth.dsn.tooth());
		}
	}

	

}
	

std::string Parser::write(const Procedure& procedure)
{
	Json::Value json;

	switch (procedure.code.type())
	{
	case ProcedureType::obturation:
	{
		auto& r = std::get<ProcedureObtData>(procedure.result);

		json["post"] = r.post;
	
		json["surfaces"] = Json::Value(Json::arrayValue);

		for (int i = 0; i < r.surfaces.size(); i++)
			if (r.surfaces[i]) json["surfaces"].append(i);

		break;
	}

	case ProcedureType::bridge:
	case ProcedureType::fibersplint:
	case ProcedureType::denture:
	//case ProcedureType::removebridgeOrSplint:
	{
		auto& r = std::get<ConstructionRange>(procedure.result);
		json["begin"] = r.tooth_begin;
		json["end"] = r.tooth_end;
		break;
	}
	case ProcedureType::anesthesia:
	{
		json["minutes"] = std::get<AnesthesiaMinutes>(procedure.result).minutes;
		break;
	}
	default:
		break;
	}

	Json::FastWriter writer;

	return writer.write(json);
}

std::string Parser::write(const std::optional<NhifContract>& contract)
{
	if (!contract.has_value())
		return std::string{};

	auto& c = contract.value();

	Json::Value json;

	json["name_short"] = c.name_short;
	json["contract_no"] = c.contract_no;
	json["date"] = c.date.to8601();
	json["bank"] = c.bank;
	json["iban"] = c.iban;
	json["bic"] = c.bic;
	json["nra"] = c.nra_pass;
	json["unfav"] = c.unfavourable;

	Json::FastWriter writer;

	return writer.write(json);
}

#include "Model/Financial/Invoice.h"

std::string Parser::write(const Invoice& inv)
{
	if (inv.nhifData.has_value())
		return inv.nhifData->monthNotifData;

	Json::Value json;

	auto mainDoc = inv.mainDocument();
	if (mainDoc) {
		json["mainDocumentNum"] = mainDoc->number;
		json["mainDocumentDate"] = mainDoc->date.to8601();
	}

	json["operations"] = Json::Value(Json::arrayValue);

	for (auto op : inv.businessOperations)
	{
		Json::Value operation;

		operation["code"] = op.activity_code;
		operation["name"] = op.activity_name;
		operation["quantity"] = op.quantity;
		operation["price"] = op.unit_price;

		json["operations"].append(operation);
	}

	json["taxEventDate"] = inv.aggragated_amounts.taxEventDate.to8601();
	json["paymentType"] = static_cast<int>(inv.aggragated_amounts.paymentType);

	Json::FastWriter writer;
	return writer.write(json);
}

std::string Parser::write(const Settings& settings)
{
	Json::Value json;

	json["pisCheck"] = settings.getPisHistoryAuto;
	json["nraCheck"] = settings.getNraStatusAuto;
	json["hisCheck"] = settings.getHisHistoryAuto;

	Json::FastWriter writer;
	return writer.write(json);
}

std::string Parser::write(const std::vector<Dosage>& dosage)
{
	Json::Value json = Json::arrayValue;

	for (auto& d : dosage)
	{
		Json::Value jsonDosage;

		jsonDosage["asNeeded"] = d.asNeeded;
		jsonDosage["route"] = d.route.getKey();
		jsonDosage["doseVal"] = d.doseQuantity.value;
		
		auto doseUnit = d.doseQuantity.getUnit();

		if (doseUnit.index() == 0) {
			jsonDosage["doseInt"] = std::get<int>(doseUnit);
		}
		else {
			jsonDosage["doseStr"] = std::get<std::string>(doseUnit);
		}

		jsonDosage["freq"] = d.frequency;
		jsonDosage["perIdx"] = d.period.getUnitIndex();
		jsonDosage["perVal"] = d.period.value;
		jsonDosage["boundsIdx"] = d.bounds.getUnitIndex();
		jsonDosage["boundsVal"] = d.bounds.value;

		Json::Value jsonWhen = Json::arrayValue;

		for (auto& tags : d.when.getTagIdx()) jsonWhen.append(tags);

		jsonDosage["when"] = jsonWhen;

		jsonDosage["offset"] = d.when.getOffset();

		jsonDosage["instr"] = d.additionalInstructions;


		json.append(jsonDosage);
	}

	return Json::FastWriter().write(json);

}

std::vector<Dosage> Parser::parseDosage(const std::string& str)
{
	std::vector<Dosage> result;

	Json::Value json = Json::arrayValue;

	Json::Reader().parse(str, json);

	for (const auto& jsonDosage : json)
	{
		result.emplace_back();
		auto& dosage = result.back();

		dosage.asNeeded = jsonDosage["asNeeded"].asBool();
		dosage.route.set(jsonDosage["route"].asInt());
		dosage.doseQuantity.value = jsonDosage["doseVal"].asDouble();
		
		if (jsonDosage.isMember("doseInt"))
		{
			dosage.doseQuantity.setUnit(jsonDosage["doseInt"].asInt());
		}
		else
		{
			dosage.doseQuantity.setUnit(jsonDosage["doseStr"].asString());
		}

		dosage.frequency = jsonDosage["freq"].asInt();


		dosage.period.setUnit(jsonDosage["perIdx"].asInt());
		dosage.period.value = jsonDosage["perVal"].asDouble();
		
		dosage.bounds.setUnit(jsonDosage["boundsIdx"].asInt());
		dosage.bounds.value = jsonDosage["boundsVal"].asDouble();

		for (auto& whenTags : jsonDosage["when"]) dosage.when.addTag(whenTags.asInt());

		dosage.when.setOffset(jsonDosage["offset"].asInt());
		dosage.additionalInstructions = jsonDosage["instr"].asString();

	}


	return result;
}


void Parser::parse(const std::string& jsonString, Procedure& procedure)
{
	Json::Value json;
	Json::Reader reader;

	bool parsingSuccessful = reader.parse(jsonString, json);

	if (!parsingSuccessful) {
		return;
	}

	//parsing additional data, where present:
	switch (procedure.code.type())
	{
	case ProcedureType::obturation:
	{
		procedure.result = ProcedureObtData
		{
			.surfaces = {false},

			.post = (json.isMember("post") && json["post"].asBool() == true)
		};

		const Json::Value& surfaces = json["surfaces"];

		for (int i = 0; i < surfaces.size(); i++)
			std::get<ProcedureObtData>(procedure.result).surfaces[surfaces[i].asInt()] = true;


		break;
	}



	case ProcedureType::bridge:
	case ProcedureType::fibersplint:
	case ProcedureType::denture:
	//case ProcedureType::removebridgeOrSplint:
		procedure.result = ConstructionRange{
			.tooth_begin = json["begin"].asInt(),
			.tooth_end = json["end"].asInt(),
		};

		break;
	case ProcedureType::anesthesia:
	{
		procedure.result = AnesthesiaMinutes{ json["minutes"].asInt() };
	}
	break;
	default:
		procedure.result = NoData{};
	}
}

void Parser::parse(const std::string& jsonString, DetailsSummary& summary)
{
	Json::Value procedure;
	Json::Reader reader;

	bool parsingSuccessful = reader.parse(jsonString, procedure);

	if (!parsingSuccessful) {
		return;
	}

	summary.procedureDiagnosis = procedure["diagnosis"].asString();

	if (!procedure["name"].isNull())
	{
		summary.procedureName = procedure["name"].asString();
	}

}

void Parser::parse(const std::string& jsonString, PerioStatus& status)
{
	Json::Value json;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonString, json);

	if (!parsingSuccessful) {
		return;
	}

	const Json::Value& pd = json["PD"];
	for (int i = 0; i < pd.size(); i++)
		status.pd[i] = pd[i].asInt();

	const Json::Value& cal = json["CAL"];
	for (int i = 0; i < cal.size(); i++)
		status.cal[i] = cal[i].asInt();

	const Json::Value& bop = json["BOP"];
	for (int i = 0; i < bop.size(); i++)
		status.bop[i] = bop[i].asBool();

	const Json::Value& ag = json["AG"];
	for (int i = 0; i < ag.size(); i++)
		status.ag[i] = ag[i].asInt();

	const Json::Value& furc = json["Furc"];
	for (int i = 0; i < furc.size(); i++)
		status.furcation[i] = furc[i].asInt();
	
	const Json::Value& fmps = json["FMPS"];
	for (int i = 0; i < fmps.size(); i++)
		status.FMPS[i] = fmps[i].asBool();

	const Json::Value& fmbs = json["FMBS"];
	for (int i = 0; i < fmbs.size(); i++)
		status.FMBS[i] = fmbs[i].asBool();

	const Json::Value& disabled = json["Disabled"];
	for (int i = 0; i < disabled.size(); i++)
		status.disabled[i] = disabled[i].asBool();

	status.smoker = json["Smoker"].asInt();
	status.boneLoss = json["BoneLoss"].asInt();
	status.completeRestorationNeeded = json["Restore"].asBool();
	status.systemic = json["Sys"].asBool();
}

void Parser::parse(const std::string& jsonString, Invoice& invoice)
{
	Json::Value json;

	Json::Reader reader;

	bool parsingSuccessful = reader.parse(jsonString, json);

	if (!parsingSuccessful) {
		throw std::invalid_argument("could not parse invoice data");
	}

	if (json.isMember("mainDocumentNum")) {
		invoice.setMainDocumentData(
			json["mainDocumentNum"].asInt(),
			Date(json["mainDocumentDate"].asString())
		);
	}

	for (const auto& operation : json["operations"])
	{

		double price = operation["price"].asDouble();
		int quantity = operation["quantity"].asInt();

		invoice.businessOperations.emplace_back(
				operation["code"].asString(),
				operation["name"].asString(),
				price, quantity
		);
	}

	invoice.aggragated_amounts.calculate(invoice.businessOperations);
	invoice.aggragated_amounts.paymentType = PaymentType::Cash;
	invoice.aggragated_amounts.taxEventDate = Date(json["taxEventDate"].asString());
	invoice.aggragated_amounts.paymentType = static_cast<PaymentType>(json["paymentType"].asInt());


}


std::string Parser::parseDiagnosis(const std::string& jsonProcedureString)
{
	Json::Value json;
	Json::Reader reader;

	bool parsingSuccessful = reader.parse(jsonProcedureString, json);

	if (!parsingSuccessful) {
		throw std::invalid_argument("could not parse procedure diagnosis");
	}

	return json["diagnosis"].asString();
}

std::optional<NhifContract> Parser::parseContract(const std::string& jsonString)
{
	if (jsonString.empty())
		return std::optional<NhifContract>{};

	Json::Value json;
	Json::Reader reader;

	bool parsingSuccessful = reader.parse(jsonString, json);

	NhifContract contract;
	contract.name_short = json["name_short"].asString();
	contract.date = Date{ json["date"].asString() };
	contract.contract_no = json["contract_no"].asString();
	contract.bank = json["bank"].asString();
	contract.iban = json["iban"].asString();
	contract.bic = json["bic"].asString();
	contract.nra_pass = json["nra"].asString();
	contract.unfavourable = json["unfav"].asBool();

	return contract;



}

Settings Parser::parseSettings(const std::string& settingsString)
{
	Json::Reader reader;
	Json::Value json;

	if (!reader.parse(settingsString, json)) {
		return {};
	}

	return Settings{
		.getHisHistoryAuto = json["hisCheck"].asBool(),
		.getPisHistoryAuto = json["pisCheck"].asBool(),
		.getNraStatusAuto = json["nraCheck"].asBool()
	};
}

