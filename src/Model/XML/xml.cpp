#include "xml.h"
#include <TinyXML/tinyxml.h>

#include <unordered_set>
#include <cmath>

#include "Model/Procedure/MasterNZOK.h"
#include "Model/FreeFunctions.h"
#include "Model/CityCode.h"
#include "Model/Tooth/ToothUtils.h"
#include "Model/User/User.h"
#include "Model/Financial/Invoice.h"

std::string getSpec(bool fullCoverage)
{
    constexpr const char* specType[4]
    {
        "PRIMARY NORM", //код специалнсот 60 или 64 и цената се покрива частично/изцяло от НЗОК
        "PRIMARY SPEC", //код специалнсот 60 или 64 и цената се покрива ИЗЦЯЛО от НЗОК
        "SPEC NORM",    //код специалнсот 61, 62 или 68 и цената се покрива частично/изцяло от НЗОК
        "SPEC DOMOVE"   //код специалнсот 61, 62 или 68 и цената се покрива ИЗЦЯЛО от НЗОК
        //"SPEC_PS      //за обща анестезия (такъв няма feature все още)
    };

    auto& specialty = User::doctor().specialty;

    //specialty check:
    {
        constexpr int specialtyCheck[5]{ 60, 61, 62, 64, 68 };

        for (auto spec : specialtyCheck) {
            if (specialty == spec) {
                goto specTypeValid;
            }
        }

        throw std::exception(u8"Невалиден код специалност на доктора");
    }

specTypeValid:

    bool primaryDentalAid = specialty == 60 || specialty == 64;

    if (!fullCoverage && primaryDentalAid)
        return specType[0];

    if (fullCoverage && primaryDentalAid)
        return specType[1];

    if (!fullCoverage && !primaryDentalAid)
        return specType[2];

    if (fullCoverage && primaryDentalAid)
        return specType[3];

    throw std::exception(u8"НЕВАЛИДНА СПЕЦИФИКАЦИЯ");
}


std::string XML::getReport(const std::vector<AmbList>& lists, const std::unordered_map<long long, Patient>& patients)
{

    auto& doctor = User::doctor();
    auto& practice = User::practice();

    TiXmlDocument doc("StomReport");

    TiXmlDeclaration* decl = new TiXmlDeclaration{ "1.0", "UTF-8" ,"" };
    doc.LinkEndChild(decl);

    TiXmlElement* report = new TiXmlElement("report");
    report->SetAttribute("xmlns", "http://localhost");
    report->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    report->SetAttribute("xsi:schemaLocation", "http://localhost ImportStomatologyReport.xsd");



    report->SetAttribute("practiceName", practice.nzok_contract.value().name_short);
    report->SetAttribute("bulstat", practice.bulstat);
    report->SetAttribute("RCZCode", practice.rziCode);
    report->SetAttribute("contractNo", practice.nzok_contract.value().contract_no);
    report->SetAttribute("dentistName", doctor.getFullName(false));
    report->SetAttribute("dentistSpec", doctor.specialty);
    report->SetAttribute("dentistPersonalCode", doctor.LPK);

    //getting the first two characters of user.rziCode:
    report->SetAttribute("RHIF", practice.rziCode.substr(0, 2));

    Date from{ 1, lists[0].getDate().month, lists[0].getDate().year };
    Date to = from.getMaxDateOfMonth();

    report->SetAttribute("startFrom", from.toXMLString());
    report->SetAttribute("endTo", to.toXMLString());
    report->SetAttribute("dentalServiceType", doctor.dentalServiceType());


    TiXmlElement* dentalCareServices = new TiXmlElement("dentalCareServices");


    //this is where we serialize the ambLists:


    for (auto& list : lists)
    {
        
        auto& patient = patients.at(list.patient_rowid);

        TiXmlElement* dentalCareService = new TiXmlElement("dentalCareService");

        dentalCareService->SetAttribute("personType", patient.type);
        dentalCareService->SetAttribute("personIdentifier", patient.id);

        auto [rhif, healthRegion] = CityCode::getCodes(patient.city);

        dentalCareService->SetAttribute("RHIFCode", rhif);
        dentalCareService->SetAttribute("healthRegionCode", healthRegion);

        if (patient.type != 1) {
            dentalCareService->SetAttribute("birthDate", patient.birth.toXMLString());
        }

        dentalCareService->SetAttribute("personFirstName", patient.FirstName);

        if (!patient.MiddleName.empty())
            dentalCareService->SetAttribute("personMiddleName", patient.MiddleName);

        dentalCareService->SetAttribute("personLastName", patient.LastName);
        dentalCareService->SetAttribute("specificationType", getSpec(list.full_coverage));
        dentalCareService->SetAttribute("ambulatorySheetNo", leadZeroes(list.number, 6));
        dentalCareService->SetAttribute("HIRBNo", patient.HIRBNo); //throw if HIRBNo empty?
        dentalCareService->SetAttribute("unfavorableCondition", 0);
        dentalCareService->SetAttribute("substitute", 0);
        dentalCareService->SetAttribute("Sign", 1);


        auto getAllergiesAndStuff = [](const std::string& str) {
            if (!str.size())
                return std::string(u8"Не съобщава");
            return str;
        };

        {
            TiXmlElement* allergies = new TiXmlElement("allergies");
            TiXmlElement* allergy = new TiXmlElement("allergy");
            allergy->SetAttribute("allergyName", getAllergiesAndStuff(patient.allergies));
            allergies->LinkEndChild(allergy);

            dentalCareService->LinkEndChild(allergies);
        }

        {
            TiXmlElement* pastDiseases = new TiXmlElement("pastDiseases");

            TiXmlElement* pastDisease = new TiXmlElement("pastDisease");
            pastDisease->SetAttribute("name", getAllergiesAndStuff(patient.pastDiseases));
            pastDiseases->LinkEndChild(pastDisease);

            dentalCareService->LinkEndChild(pastDiseases);
        }

        {
            TiXmlElement* currentDiseases = new TiXmlElement("currentDiseases");

            TiXmlElement* currentDisease = new TiXmlElement("currentDisease");
            currentDisease->SetAttribute("name", getAllergiesAndStuff(patient.currentDiseases));
            currentDiseases->LinkEndChild(currentDisease);

            dentalCareService->LinkEndChild(currentDiseases);
        }

        TiXmlElement* teeth = new TiXmlElement("teeth");

        //parsing status

        for (auto& t : list.teeth)
        {
            auto status = t.getSimpleStatuses();
            if (status.empty()) continue;

            TiXmlElement* tooth = new TiXmlElement("tooth");
            tooth->SetAttribute("toothCode", ToothUtils::getNomenclature(t));

            TiXmlElement* toothStatuses = new TiXmlElement("toothStatuses");

            for (auto& s : status)
            {
                TiXmlElement* toothStatus = new TiXmlElement("toothStatus");
                toothStatus->SetAttribute("toothStatusCode", s);
                toothStatuses->LinkEndChild(toothStatus);
            }

            tooth->LinkEndChild(toothStatuses);

            teeth->LinkEndChild(tooth);
        }

        //parsing procedures

        dentalCareService->LinkEndChild(teeth);

        TiXmlElement* services = new TiXmlElement("services");

        for (auto& procedure : list.procedures)
        {
            TiXmlElement* service = new TiXmlElement("service");

            service->SetAttribute("date", procedure.date.toXMLString());
            service->SetAttribute("diagnosis", procedure.diagnosis);
            service->SetAttribute("toothCode", ToothUtils::getToothNumber(procedure.tooth, procedure.temp));
            service->SetAttribute("activityCode", procedure.code);

            services->LinkEndChild(service);

        }

        dentalCareService->LinkEndChild(services);

        //no such features yet
        dentalCareService->LinkEndChild(new TiXmlElement("medicalReferrals"));
        dentalCareService->LinkEndChild(new TiXmlElement("MDAReferrals"));
        dentalCareService->LinkEndChild(new TiXmlElement("prescSpecialists"));

        dentalCareServices->LinkEndChild(dentalCareService);
    }

    report->LinkEndChild(dentalCareServices);

    doc.LinkEndChild(report);

    TiXmlPrinter printer;
    printer.SetStreamPrinting();

    doc.Accept(&printer);

    return printer.Str();

}

std::string XML::getInvoice(const Invoice& invoice)
{
    TiXmlDocument doc("Invoice");

    TiXmlDeclaration* decl = new TiXmlDeclaration{ "1.0", "UTF-8" ,"" };
    doc.LinkEndChild(decl);

    TiXmlElement* el_invoice = new TiXmlElement("ELECTRONIC_INVOICE");
    el_invoice->SetAttribute("xmlns", "http://pis.technologica.com/electronic_invoice.xsd");


    auto addElementWithText = [](TiXmlElement* parent, const char* name, const std::string& value)
    {
        TiXmlElement* element = new TiXmlElement(name);
        TiXmlText* text = new TiXmlText(value);

        element->LinkEndChild(text);

        parent->LinkEndChild(element);
    };

    addElementWithText(el_invoice, "fin_document_type_code", invoice.nzokData->fin_document_type_code);
    addElementWithText(el_invoice, "fin_document_no", invoice.getInvoiceNumber());
    addElementWithText(el_invoice, "fin_document_month_no", leadZeroes(invoice.nzokData->fin_document_month_no, 10));
    addElementWithText(el_invoice, "fin_document_date", invoice.date.toXMLString());

    auto mainDoc = invoice.mainDocument();

    if (mainDoc)
    {
        TiXmlElement* mainDocument = new TiXmlElement("Main_Fin_Doc");
        addElementWithText(mainDocument, "document_no", leadZeroes(mainDoc->number, 10));
        addElementWithText(mainDocument, "document_date", mainDoc->date.toXMLString());
        el_invoice->LinkEndChild(mainDocument);
    }

    TiXmlElement* recipient = new TiXmlElement("Invoice_Recipient");
    addElementWithText(recipient, "recipient_code", User::practice().rziCode.substr(0, 2));
    addElementWithText(recipient, "recipient_name", invoice.recipient.name);
    addElementWithText(recipient, "recipient_address", invoice.recipient.address);
    addElementWithText(recipient, "recipient_bulstat", invoice.recipient.bulstat);
    el_invoice->LinkEndChild(recipient);

    TiXmlElement* issuer = new TiXmlElement("Invoice_Issuer");

    addElementWithText(issuer, "issuer_type", std::to_string(invoice.issuer.type.index()));

    if (invoice.issuer.type.index() == 1)
    {
        auto& selfInsured = std::get<SelfInsured>(invoice.issuer.type);

        addElementWithText(issuer, "self_insured", "Y");
        addElementWithText(issuer, "self_insured_declaration", selfInsured.self_insured_declaration);

        TiXmlElement* person_info = new TiXmlElement("Person_Info");

        TiXmlElement* identifier = new TiXmlElement("Identifier");
        addElementWithText(identifier, "grao_no", selfInsured.person_info.identifier);
        person_info->LinkEndChild(identifier);

        addElementWithText(person_info, "first_name", selfInsured.person_info.first_name);
        addElementWithText(person_info, "second_name", selfInsured.person_info.second_name);
        addElementWithText(person_info, "last_name", selfInsured.person_info.last_name);

        issuer->LinkEndChild(person_info);

    }
    else
    {
        auto& company = std::get<Company>(invoice.issuer.type);
        addElementWithText(issuer, "legal_form", company.legal_form);
    }

    addElementWithText(issuer, "company_name", invoice.issuer.company_name);
    addElementWithText(issuer, "address_by_contract", invoice.issuer.address_by_contract);

    if (User::doctor().severalRHIF) {
        addElementWithText(issuer, "address_by_activity", invoice.issuer.address_by_activity);
    }

    addElementWithText(issuer, "registration_by_VAT", std::to_string(invoice.issuer.registration_by_VAT.has_value()));
    addElementWithText(issuer, "grounds_for_not_charging_VAT", invoice.issuer.grounds_for_not_charging_VAT);
    addElementWithText(issuer, "issuer_bulstat", invoice.issuer.bulstat);
    if (invoice.issuer.registration_by_VAT.has_value()) {
        addElementWithText(issuer, "issuer_bulstat_no_vat", invoice.issuer.registration_by_VAT.value());
    }
    addElementWithText(issuer, "contract_no", invoice.nzokData->contract_no);
    addElementWithText(issuer, "contract_date", invoice.nzokData->contract_date.toXMLString());
    addElementWithText(issuer, "rhi_nhif_no", invoice.nzokData->rhi_nhif_no);

    el_invoice->LinkEndChild(issuer);


    addElementWithText(el_invoice, "health_insurance_fund_type_code", invoice.nzokData->health_insurance_fund_type_code);
    addElementWithText(el_invoice, "activity_type_code", std::to_string(invoice.nzokData->activityTypeCode));
    addElementWithText(el_invoice, "date_from", invoice.nzokData->date_from.toXMLString());
    addElementWithText(el_invoice, "date_to", invoice.nzokData->date_to.toXMLString());

    for (auto& operation : invoice.businessOperations)
    {
        TiXmlElement* businessOperation = new TiXmlElement("Business_operation");

        addElementWithText(businessOperation, "activity_code", operation.activity_code);
        addElementWithText(businessOperation, "activity_name", operation.activity_name);
        addElementWithText(businessOperation, "measure_code", "BR");
        addElementWithText(businessOperation, "quantity", std::to_string(operation.quantity));
        addElementWithText(businessOperation, "unit_price", formatDouble(operation.unit_price));
        addElementWithText(businessOperation, "value_price", formatDouble(operation.value_price));

        el_invoice->LinkEndChild(businessOperation);
    }

    TiXmlElement* aggregatedAmounts = new TiXmlElement("Aggregated_amounts");

    addElementWithText(aggregatedAmounts, "payment_type", "B");
    addElementWithText(aggregatedAmounts, "total_amount", formatDouble(invoice.aggragated_amounts.total_amount));
    addElementWithText(aggregatedAmounts, "payment_amount", formatDouble(invoice.aggragated_amounts.payment_amount));
    addElementWithText(aggregatedAmounts, "original", "Y");
    addElementWithText(aggregatedAmounts, "tax_event_date", invoice.aggragated_amounts.taxEventDate.toXMLString());

    el_invoice->LinkEndChild(aggregatedAmounts);

    doc.LinkEndChild(el_invoice);

    TiXmlPrinter printer;
    printer.SetStreamPrinting();

    doc.Accept(&printer);

    return printer.Str();
}
