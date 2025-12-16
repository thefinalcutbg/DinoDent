#include "BulkRequester.h"
#include "Model/User.h"

void BulkRequester::nextStep()
{
	if (steps.empty() && m_callback) {
		m_callback(m_result);
		return;
	}

	auto step = std::move(steps.back());
	steps.pop_back();
	step();
}

void BulkRequester::sendRequest(const Patient& patient, const std::vector<RequestType>& requestTypes)
{
	if (steps.size()) {
		return;
	}

	p = patient;

	for (auto& reqType : requestTypes) {
		switch (reqType) {

		case RequestType::NhifProcedures:

			if(!User::hasNhifContract()) break;

			steps.push_back([this]() {
				if (!dentalActService.sendRequest(
					p,
					false,
					[this](const std::optional<std::vector<Procedure>>& result) {
						m_result.pisDentalActivities = result;
						nextStep();
					}
				)) nextStep();
				});
			break;

		case RequestType::HISProcedures:
			steps.push_back([this]() {
				if (eDentalGetStatusAndProceduresService.sendRequest(
					p, false, [this](const std::optional<std::vector<Procedure>>& procedures, const std::vector<HisSnapshot>& snapshots)
					{
						m_result.hisDentalRecords = procedures;
						nextStep();
					}
				)) nextStep();
				});
			break;

		case RequestType::NraStatus:

			if (!User::hasNhifContract() ||  User::practice().hasNraAccess()) break;

			steps.push_back([this]() {
				if (!nraStatusServ.sendRequest(
					p,
					[this](const std::optional<InsuranceStatus>& result) {
						m_result.nraStatus = result;
						nextStep();
					}
				)) nextStep();
				});
			break;

		case RequestType::HirbNo:

			if (!User::hasNhifContract()) break;

			steps.push_back([this]() {
				if(!hirbnoService.sendRequest(
					p,
					[this](const std::string& hirbNo) {
						m_result.hirbNo = hirbNo;
						nextStep();
					}
				)) nextStep();
			});
			break;
		

		case RequestType::NhifMedicalConditions:

			if (!User::hasNhifContract()) break;

			steps.push_back([this]() {
				
				if(!nhifDiagnosisServ.sendRequest(
					p,
					[this](const std::vector<ICD10>& current, const std::vector<ICD10>& past) {

						for (auto& status : current) {
							m_result.currentDiseases.push_back(status);
						}
						for (auto& status : past) {
							m_result.pastDiseases.push_back(status);
						}

						nextStep();
					}
					)) nextStep();
				});
			break;

		case RequestType::Allergies:
			steps.push_back([this]() {
				if(!eAllergyFetchService.sendRequest(
					p, User::practice().rziCode,
					[this](const std::vector<Allergy>& result) {
						m_result.allergies = result;
						nextStep();
					}
				)) nextStep();
				});
			break;



		case RequestType::HISMedicalConditions:
			steps.push_back([this]() {
				if (!eClinicalConditionFetchService.sendRequest(
					p, User::practice().rziCode,
					[this](const std::vector<MedicalStatus>& activeConditions, const std::vector<MedicalStatus>& pastConditions) {
						for (auto& status : activeConditions) {
							m_result.currentDiseases.push_back(status);
						}
						for (auto& status : pastConditions) {
							m_result.pastDiseases.push_back(status);
						}

						nextStep();
					}
				)) nextStep();
				});
			break;

		case RequestType::Hospitalizations:
			steps.push_back([this]() {
				if(!eHospitalizationFetchService.sendRequest(
					p, User::practice().rziCode,
					[this](const std::vector<Hospitalization>& hospitalizations) {
						m_result.hospitalizations = hospitalizations;
						nextStep();
					}
				)) nextStep();
				});
			break;

		};
	}

	std::reverse(steps.begin(), steps.end());

	nextStep();
}
