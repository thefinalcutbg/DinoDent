#include "Parser.h"
#include <JsonCpp/json.h>
#include "Model/Dental/ToothUtils.h"
#include "Model/Dental/ToothContainer.h"
#include "Model/Dental/PerioStatus.h"
#include "Model/Dental/Procedure.h"
#include "Model/TableStructs.h"
#include "Model/Financial/BusinessOperation.h"

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
	Json::Value json;

	auto writeStatus = [](int index, const Status& status)
	{
		Json::Value parameters;
		parameters["idx"] = index;
		return parameters;
	};

	auto writeDentistMade = [](int index, const DentistMade& procedure)
	{
		Json::Value parameters;
		parameters["LPK"] = procedure.LPK;
		parameters["idx"] = index;
		return parameters;
	};

	for (int i = 0; i < 32; i++)
	{
		const Tooth& tooth = status[i];

		if (tooth.healthy)
		{
			if (!json.isMember("Healthy"))
			{
				json["Healthy"] = Json::Value(Json::arrayValue);
			}

			json["Healthy"].append(writeStatus(i, tooth.healthy));
		}

		if (tooth.temporary.exists())
		{
			if (!json.isMember("Temporary"))
			{
				json["Temporary"] = Json::Value(Json::arrayValue);
			}

			json["Temporary"].append(writeStatus(i, tooth.temporary));
		}

		if (tooth.hyperdontic.exists())
		{
			if (!json.isMember("Hyperdontic"))
			{
				json["Hyperdontic"] = Json::Value(Json::arrayValue);
			}

			json["Hyperdontic"].append(writeStatus(i, tooth.hyperdontic));
		}


		if (tooth.impacted.exists())
		{
			if (!json.isMember("Impacted"))
			{
				json["Impacted"] = Json::Value(Json::arrayValue);
			}

			json["Impacted"].append(writeStatus(i, tooth.impacted));
		}

		if (tooth.mobility.exists())
		{
			if (!json.isMember("Mobility"))
			{
				json["Mobility"] = Json::Value(Json::arrayValue);
			}
			Json::Value parameters = writeStatus(i, tooth.mobility);
			parameters["degree"] = static_cast<int>(tooth.mobility.degree);
			json["Mobility"].append(parameters);
		}

		if (tooth.periodontitis.exists())
		{
			if (!json.isMember("Periodontitis"))
			{
				json["Periodontitis"] = Json::Value(Json::arrayValue);
			}

			json["Periodontitis"].append(writeStatus(i, tooth.periodontitis));
		}

		if (tooth.calculus.exists())
		{
			if (!json.isMember("Calculus"))
			{
				json["Calculus"] = Json::Value(Json::arrayValue);
			}

			json["Calculus"].append(writeStatus(i, tooth.denture));
		}

		if (tooth.obturation.exists())
		{
			if (!json.isMember("Obturation"))
			{
				json["Obturation"] = Json::Value(Json::arrayValue);
			}
			for (int y = 0; y < 6; y++)
			{
				if (tooth.obturation.exists(y)) {

					auto parameters = writeDentistMade(i, tooth.obturation[y]);
					parameters["Surface"] = y;
					json["Obturation"].append(parameters);
				}
			}
		}


		if (tooth.caries.exists())
		{
			if (!json.isMember("Caries"))
			{
				json["Caries"] = Json::Value(Json::arrayValue);
			}
			for (int y = 0; y < 6; y++)
			{
				if (tooth.caries.exists(y)) {

					auto parameters = writeStatus(i, tooth.caries[y]);
					parameters["Surface"] = y;
					json["Caries"].append(parameters);
				}

			}
		}

		if (tooth.pulpitis.exists())
		{
			if (!json.isMember("Pulpitis"))
			{
				json["Pulpitis"] = Json::Value(Json::arrayValue);
			}
			json["Pulpitis"].append(writeStatus(i, tooth.pulpitis));
		}

		if (tooth.lesion.exists())
		{
			if (!json.isMember("Lesion"))
			{
				json["Lesion"] = Json::Value(Json::arrayValue);
			}

			json["Lesion"].append(writeStatus(i, tooth.lesion));
		}

		if (tooth.endo.exists())
		{
			if (!json.isMember("EndoTreatment"))
			{
				json["EndoTreatment"] = Json::Value(Json::arrayValue);
			}

			json["EndoTreatment"].append(writeDentistMade(i, tooth.endo));
		}

		if (tooth.post.exists())
		{
			if (!json.isMember("Post"))
			{
				json["Post"] = Json::Value(Json::arrayValue);
			}

			json["Post"].append(writeDentistMade(i, tooth.post));
		}


		if (tooth.extraction.exists())
		{

			if (!json.isMember("Extraction"))
			{
				json["Extraction"] = Json::Value(Json::arrayValue);
			}

			json["Extraction"].append(writeDentistMade(i, tooth.extraction));
		}

		if (tooth.root.exists())
		{
			if (!json.isMember("Root"))
			{
				json["Root"] = Json::Value(Json::arrayValue);
			}

			json["Root"].append(writeStatus(i, tooth.root));
		}


		if (tooth.fracture.exists())
		{
			if (!json.isMember("Fracture"))
			{
				json["Fracture"] = Json::Value(Json::arrayValue);
			}

			json["Fracture"].append(writeStatus(i, tooth.fracture));
		}

		if (tooth.crown.exists())
		{
			if (!json.isMember("Crown"))
			{
				json["Crown"] = Json::Value(Json::arrayValue);
			}
			
			json["Crown"].append(writeDentistMade(i, tooth.crown));
		}

		if (tooth.implant.exists())
		{
			if (!json.isMember("Implant"))
			{
				json["Implant"] = Json::Value(Json::arrayValue);
			}

			json["Implant"].append(writeDentistMade(i, tooth.implant));
		}

		if (tooth.bridge.exists())
		{
			if (!json.isMember("Bridge"))
			{
				json["Bridge"] = Json::Value(Json::arrayValue);
			}
			auto param = writeDentistMade(i, tooth.bridge);
			param["pos"] = static_cast<int>(tooth.bridge.position);
			json["Bridge"].append(param);
		}

		if (tooth.splint.exists())
		{
			if (!json.isMember("Splint"))
			{
				json["Splint"] = Json::Value(Json::arrayValue);
			}

			auto param = writeDentistMade(i, tooth.splint);
			param["pos"] = static_cast<int>(tooth.splint.position);
			json["Splint"].append(param);
		}

		if (tooth.denture.exists())
		{
			if (!json.isMember("Denture"))
			{
				json["Denture"] = Json::Value(Json::arrayValue);
			}

			json["Denture"].append(writeDentistMade(i, tooth.denture));
		}
	}

	Json::FastWriter writer;

	return writer.write(json);
}

std::string Parser::write(const Procedure& procedure)
{
	Json::Value json;

	switch (procedure.code.type())
	{
	case ProcedureType::obturation:
	{
		auto& r = std::get<ProcedureObtData>(procedure.result);

		if (r.post)
		{
			json["post"] = r.post;
		}
	
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
	default:
		break;
	}

	Json::FastWriter writer;

	return writer.write(json);
}

std::string Parser::write(const std::optional<NzokContract>& contract)
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

std::string Parser::write(const std::vector<ProcedureCode>& priceList)
{
	Json::Value priceJson = Json::Value(Json::arrayValue);

	for (auto& p : priceList)
	{
		Json::Value pTemplate;

		pTemplate["code"] = p.code();

		priceJson.append(pTemplate);
	}

	Json::FastWriter writer;
	return writer.write(priceJson);
}

#include "Model/Financial/Invoice.h"

std::string Parser::write(const Invoice& inv)
{
	if (inv.nzokData.has_value())
		return inv.nzokData->monthNotifData;

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

			.post = json.isMember("post")
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

void Parser::parse(const std::string& jsonString, ToothContainer& status)
{
	Json::Value json;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonString, json);

	if (!parsingSuccessful) {
		return;
	}

	const Json::Value& healthy = json["Healthy"];

	for (int i = 0; i < healthy.size(); i++)
	{
		Tooth& tooth = status[healthy[i]["idx"].asInt()];
		tooth.healthy.set(true);
	}


	const Json::Value& temporary = json["Temporary"];

	for (int i = 0; i < temporary.size(); i++)
	{
		Tooth& tooth = status[temporary[i]["idx"].asInt()];
		tooth.temporary.set(true);
	}

	const Json::Value& hyperdontic = json["Hyperdontic"];

	for (int i = 0; i < hyperdontic.size(); i++)
	{
		Tooth& tooth = status[hyperdontic[i]["idx"].asInt()];
		tooth.hyperdontic.set(true);
	}

	const Json::Value& impacted = json["Impacted"];

	for (int i = 0; i < impacted.size(); i++)
	{
		Tooth& tooth = status[impacted[i]["idx"].asInt()];
		tooth.impacted.set(true);
	}

	const Json::Value& mobility = json["Mobility"];

	for (int i = 0; i < mobility.size(); i++)
	{
		Tooth& tooth = status[mobility[i]["idx"].asInt()];
		tooth.mobility.set(true);
		tooth.mobility.degree = static_cast<Degree>(mobility[i]["degree"].asInt());

	}

	const Json::Value& periodontitis = json["Periodontitis"];

	for (int i = 0; i < periodontitis.size(); i++)
	{
		Tooth& tooth = status[periodontitis[i]["idx"].asInt()];
		tooth.periodontitis.set(true);
	}

	const Json::Value& calculus = json["Calculus"];

	for (int i = 0; i < calculus.size(); i++)
	{
		Tooth& tooth = status[calculus[i]["idx"].asInt()];
		tooth.calculus.set(true);
	}


	const Json::Value& obturation = json["Obturation"];

	for (int i = 0; i < obturation.size(); i++)
	{
		Tooth& tooth = status[obturation[i]["idx"].asInt()];
		int surface = obturation[i]["Surface"].asInt();
		tooth.obturation.set(true, surface);
		tooth.obturation[surface].LPK = obturation[i]["LPK"].asString();

	}

	const Json::Value& car = json["Caries"];

	for (int i = 0; i < car.size(); i++)
	{
		Tooth& tooth = status[car[i]["idx"].asInt()];
		int surface = car[i]["Surface"].asInt();
		tooth.caries.set(true, surface);
	}

	const Json::Value& pulpitis = json["Pulpitis"];

	for (int i = 0; i < pulpitis.size(); i++)
	{
		Tooth& tooth = status[pulpitis[i]["idx"].asInt()];
		tooth.pulpitis.set(true);

	}

	const Json::Value& lesion = json["Lesion"];

	for (int i = 0; i < lesion.size(); i++)
	{
		Tooth& tooth = status[lesion[i]["idx"].asInt()];
		tooth.lesion.set(true);
	}

	const Json::Value& fracture = json["Fracture"];

	for (int i = 0; i < fracture.size(); i++)
	{
		Tooth& tooth = status[fracture[i]["idx"].asInt()];
		tooth.fracture.set(true);
	}

	const Json::Value& endo = json["EndoTreatment"];

	for (int i = 0; i < endo.size(); i++)
	{
		Tooth& tooth = status[endo[i]["idx"].asInt()];
		tooth.endo.set(true);
		tooth.endo.LPK = endo[i]["LPK"].asString();
	}

	const Json::Value& post = json["Post"];

	for (int i = 0; i < post.size(); i++)
	{
		Tooth& tooth = status[post[i]["idx"].asInt()];
		tooth.post.set(true);
		tooth.post.LPK = post[i]["LPK"].asString();
	}

	const Json::Value& extraction = json["Extraction"];

	for (int i = 0; i < extraction.size(); i++)
	{
		Tooth& tooth = status[extraction[i]["idx"].asInt()];
		tooth.extraction.set(true);
		tooth.extraction.LPK = extraction[i]["LPK"].asString();
	}

	const Json::Value& root = json["Root"];

	for (int i = 0; i < root.size(); i++)
	{

		Tooth& tooth = status[root[i]["idx"].asInt()];
		tooth.root.set(true);
	}

	const Json::Value& implant = json["Implant"];

	for (int i = 0; i < implant.size(); i++)
	{
		Tooth& tooth = status[implant[i]["idx"].asInt()];
		tooth.implant.set(true);
		tooth.implant.LPK = implant[i]["LPK"].asString();
	}

	const Json::Value& crown = json["Crown"];

	for (int i = 0; i < crown.size(); i++)
	{
		Tooth& tooth = status[crown[i]["idx"].asInt()];
		tooth.crown.set(true);
		tooth.crown.LPK = crown[i]["LPK"].asString();
	}

	const Json::Value& bridge = json["Bridge"];

	for (int i = 0; i < bridge.size(); i++)
	{
		Tooth& tooth = status[bridge[i]["idx"].asInt()];
		tooth.bridge.set(true);
		tooth.bridge.LPK = bridge[i]["LPK"].asString();
		tooth.bridge.position = static_cast<BridgePos>(bridge[i]["pos"].asInt());
	}

	const Json::Value& splint = json["Splint"];

	for (int i = 0; i < splint.size(); i++)
	{
		Tooth& tooth = status[splint[i]["idx"].asInt()];
		tooth.splint.position = static_cast<BridgePos>(splint[i]["pos"].asInt());
		tooth.splint.LPK = splint[i]["LPK"].asString();
		tooth.splint.set(true);
	}

	const Json::Value& denture = json["Denture"];

	for (int i = 0; i < denture.size(); i++)
	{
		Tooth& tooth = status[denture[i]["idx"].asInt()];
		tooth.denture.LPK = denture[i]["LPK"].asString();
		tooth.denture.set(true);
	}

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


std::vector<ProcedureCode> Parser::getPriceList(const std::string& priceList)
{
	
	Json::Reader reader;
	Json::Value procedureTemplate = Json::arrayValue;

	reader.parse(priceList, procedureTemplate);

	std::vector<ProcedureCode> procedureTemplateList;
	procedureTemplateList.reserve(procedureTemplate.size());

	for (int i = 0; i < procedureTemplate.size(); i++)
	{
		procedureTemplateList.emplace_back(procedureTemplate[i]["code"].asString());
	}

	return procedureTemplateList;
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

std::optional<NzokContract> Parser::parseContract(const std::string& jsonString)
{
	if (jsonString.empty())
		return std::optional<NzokContract>{};

	Json::Value json;
	Json::Reader reader;

	bool parsingSuccessful = reader.parse(jsonString, json);

	NzokContract contract;
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

	Settings s;
	s.getPisHistoryAuto = json["pisCheck"].asBool();
	s.getNraStatusAuto = json["nraCheck"].asBool();

	return s;
}

