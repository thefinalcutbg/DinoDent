#include "xml.h"
#include <TinyXML/tinyxml.h>

#include <unordered_set>
#include <cmath>

#include "Model/Dental/NhifProcedures.h"
#include "Model/FreeFunctions.h"
#include "Model/Dental/ToothUtils.h"
#include "Model/User.h"
#include "Model/Financial/Invoice.h"



std::string XML::getReport(const std::vector<AmbList>& lists, const std::unordered_map<long long, Patient>& patients)
{

    auto& doctor = User::doctor();
    auto& practice = User::practice();

    TiXmlDocument doc("StomReport");

    TiXmlDeclaration* decl = new TiXmlDeclaration{ "1.0", "UTF-8" ,"" };
    doc.LinkEndChild(decl);

    TiXmlElement* report = new TiXmlElement("report");
    report->SetAttribute("xmlns", "http://nhif.bg/xsd/dental");
    report->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");;



    report->SetAttribute("practiceName", practice.nzok_contract.value().name_short);
    report->SetAttribute("bulstat", practice.bulstat);
    report->SetAttribute("RCZCode", practice.rziCode);
    report->SetAttribute("contractNo", practice.nzok_contract.value().contract_no);
    report->SetAttribute("dentistName", doctor.getFullName(false));
    report->SetAttribute("dentistSpec", doctor.specialtyAsInt());
    report->SetAttribute("dentistPersonalCode", doctor.LPK);

    //getting the first two characters of user.rziCode:
    report->SetAttribute("RHIF", practice.rziCode.substr(0, 2));

    Date from{ 1, lists[0].getDate().month, lists[0].getDate().year };
    Date to = from.getMaxDateOfMonth();

    report->SetAttribute("startFrom", from.to8601());
    report->SetAttribute("endTo", to.to8601());
    report->SetAttribute("dentalServiceType", doctor.specialty == NhifSpecialty::General ? 0 : 1);


    TiXmlElement* dentalCareServices = new TiXmlElement("dentalCareServices");


    //this is where we serialize the ambLists:


    for (auto& list : lists)
    {
        
        auto& patient = patients.at(list.patient_rowid);

        TiXmlElement* dentalCareService = new TiXmlElement("dentalCareService");

        dentalCareService->SetAttribute("personType", patient.type);
        dentalCareService->SetAttribute("personIdentifier", patient.id);

        dentalCareService->SetAttribute("RHIFCode", patient.city.getRhif());
        dentalCareService->SetAttribute("healthRegionCode", patient.city.getHealthRegion());

        if (patient.type != 1) {
            dentalCareService->SetAttribute("birthDate", patient.birth.to8601());
        }

        dentalCareService->SetAttribute("personFirstName", patient.FirstName);

        if (!patient.MiddleName.empty())
            dentalCareService->SetAttribute("personMiddleName", patient.MiddleName);

        dentalCareService->SetAttribute("personLastName", patient.LastName);
        dentalCareService->SetAttribute("specificationType", list.nhifData.getSpecString(doctor.specialty));
        dentalCareService->SetAttribute("ambulatorySheetNo", 
            list.nrn.size() ? list.nrn :
            FreeFn::leadZeroes(list.number, 6)
        );
        dentalCareService->SetAttribute("HIRBNo", patient.HIRBNo); //throw if HIRBNo empty?
        dentalCareService->SetAttribute("unfavorableCondition", practice.isUnfavourable() && list.nhifData.isUnfavourable);
        dentalCareService->SetAttribute("substitute", 0);
        dentalCareService->SetAttribute("Sign", 1);


        auto getAllergiesAndStuff = [](const std::string& str) {
            if (!str.size())
                return std::string("Не съобщава");
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

            service->SetAttribute("date", procedure.date.to8601());
            service->SetAttribute("diagnosis", procedure.diagnosis);
            service->SetAttribute("toothCode", ToothUtils::getNhifNumber(procedure.tooth, procedure.temp, procedure.hyperdontic));
            service->SetAttribute("activityCode", procedure.code.nhifCode());

            services->LinkEndChild(service);

        }

        dentalCareService->LinkEndChild(services);

        //no such features yet
        dentalCareService->LinkEndChild(new TiXmlElement("medicalReferrals"));

        auto MDAReferrals = new TiXmlElement("MDAReferrals");

        auto prescSpecialists = new TiXmlElement("prescSpecialists");

        for (auto& r : list.referrals)
        {
            switch (r.type)
            {
                case(ReferralType::MDD4):
                {
                    auto mdaRef = new TiXmlElement("MDAReferral");
                        mdaRef->SetAttribute("pathwayNo", FreeFn::leadZeroes(r.number, 6));
                        mdaRef->SetAttribute("date", r.date.to8601());
                             auto codes = new TiXmlElement("codes");
                             auto code = new TiXmlElement("code");
                             code->SetAttribute("name", std::get<MDD4Data>(r.data).getCode());
                             codes->LinkEndChild(code);
                        mdaRef->LinkEndChild(codes);
                    MDAReferrals->LinkEndChild(mdaRef);
                    break;
                }
                case(ReferralType::MH119):
                {
                    auto prescrSpec = new TiXmlElement("prescSpecialist");

                        prescrSpec->SetAttribute("SODPCode", std::get<MH119Data>(r.data).getSpecCode());
                        prescrSpec->SetAttribute("date", r.date.to8601());

                    prescSpecialists->LinkEndChild(prescrSpec);
                    break;
                }
            }


        }

        dentalCareService->LinkEndChild(MDAReferrals);
        
        dentalCareService->LinkEndChild(prescSpecialists);

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
    addElementWithText(el_invoice, "fin_document_month_no", FreeFn::leadZeroes(invoice.nzokData->fin_document_month_no, 10));
    addElementWithText(el_invoice, "fin_document_date", invoice.date.to8601());

    auto mainDoc = invoice.mainDocument();

    if (mainDoc)
    {
        TiXmlElement* mainDocument = new TiXmlElement("Main_Fin_Doc");
        addElementWithText(mainDocument, "document_no", FreeFn::leadZeroes(mainDoc->number, 10));
        addElementWithText(mainDocument, "document_date", mainDoc->date.to8601());
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

    if (invoice.issuer.type.index() != 1)
    {
        auto& company = std::get<Company>(invoice.issuer.type);
        addElementWithText(issuer, "legal_form", company.legal_form);
        addElementWithText(issuer, "company_name", invoice.issuer.company_name);
    }
    else
    {
        auto& selfInsured = std::get<SelfInsured>(invoice.issuer.type);

        addElementWithText(issuer, "self_insured", "Y");
        addElementWithText(issuer, "self_insured_declaration", selfInsured.self_insured_declaration);

        addElementWithText(issuer, "company_name", invoice.issuer.company_name);
        TiXmlElement* person_info = new TiXmlElement("Person_Info");

        TiXmlElement* identifier = new TiXmlElement("Identifier");
        addElementWithText(identifier, "grao_no", selfInsured.person_info.identifier);
        person_info->LinkEndChild(identifier);

        addElementWithText(person_info, "first_name", selfInsured.person_info.first_name);
        addElementWithText(person_info, "second_name", selfInsured.person_info.second_name);
        addElementWithText(person_info, "last_name", selfInsured.person_info.last_name);

        issuer->LinkEndChild(person_info);

    }

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
    addElementWithText(issuer, "contract_date", invoice.nzokData->contract_date.to8601());
    addElementWithText(issuer, "rhi_nhif_no", invoice.nzokData->rhi_nhif_no);

    el_invoice->LinkEndChild(issuer);


    addElementWithText(el_invoice, "health_insurance_fund_type_code", invoice.nzokData->health_insurance_fund_type_code);
    addElementWithText(el_invoice, "activity_type_code", std::to_string(invoice.nzokData->activityTypeCode));
    addElementWithText(el_invoice, "date_from", invoice.nzokData->date_from.to8601());
    addElementWithText(el_invoice, "date_to", invoice.nzokData->date_to.to8601());

    for (auto& operation : invoice.businessOperations)
    {
        TiXmlElement* businessOperation = new TiXmlElement("Business_operation");

        addElementWithText(businessOperation, "activity_code", operation.activity_code);
        addElementWithText(businessOperation, "activity_name", operation.activity_name);
        addElementWithText(businessOperation, "measure_code", "BR");
        addElementWithText(businessOperation, "quantity", std::to_string(operation.quantity));
        addElementWithText(businessOperation, "unit_price", FreeFn::formatDouble(operation.unit_price));
        addElementWithText(businessOperation, "value_price", FreeFn::formatDouble(operation.value_price));

        el_invoice->LinkEndChild(businessOperation);
    }

    TiXmlElement* aggregatedAmounts = new TiXmlElement("Aggregated_amounts");

    addElementWithText(aggregatedAmounts, "payment_type", "B");
    addElementWithText(aggregatedAmounts, "total_amount", FreeFn::formatDouble(invoice.aggragated_amounts.total_amount));
    addElementWithText(aggregatedAmounts, "payment_amount", FreeFn::formatDouble(invoice.aggragated_amounts.payment_amount));
    addElementWithText(aggregatedAmounts, "original", "Y");
    addElementWithText(aggregatedAmounts, "tax_event_date", invoice.aggragated_amounts.taxEventDate.to8601());

    el_invoice->LinkEndChild(aggregatedAmounts);

    doc.LinkEndChild(el_invoice);

    TiXmlPrinter printer;
    printer.SetStreamPrinting();

    doc.Accept(&printer);

    return printer.Str();
}
