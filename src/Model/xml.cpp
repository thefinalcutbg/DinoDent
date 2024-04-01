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



    report->SetAttribute("practiceName", practice.nhif_contract.value().name_short);
    report->SetAttribute("bulstat", practice.bulstat);
    report->SetAttribute("RCZCode", practice.rziCode);
    report->SetAttribute("contractNo", practice.nhif_contract.value().contract_no);
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
        dentalCareService->SetAttribute("personIdentifier", patient.type != Patient::EU ? patient.id : "0000000000");

        dentalCareService->SetAttribute("RHIFCode", patient.city.getRhif());
        dentalCareService->SetAttribute("healthRegionCode", patient.city.getHealthRegion());

        if (patient.type != Patient::EGN) {
            dentalCareService->SetAttribute("birthDate", patient.birth.to8601());
        }

        dentalCareService->SetAttribute("personFirstName", patient.FirstName);

        if (!patient.MiddleName.empty())
            dentalCareService->SetAttribute("personMiddleName", patient.MiddleName);

        dentalCareService->SetAttribute("personLastName", patient.LastName);

        //person type 4
        if (patient.type == Patient::EU)
        {
            bool is_ehic = patient.foreigner->isEHIC();

            dentalCareService->SetAttribute("countryCode", patient.foreigner->country.getCode());
            dentalCareService->SetAttribute("IICode", patient.foreigner->institution);
            dentalCareService->SetAttribute("documentType", is_ehic ? "ЕЗОК" : "Друг");
            dentalCareService->SetAttribute(is_ehic ? "validTo" : "validFrom", patient.foreigner->date_valid.to8601());
            
            if (is_ehic) {
                dentalCareService->SetAttribute("EHICNumber", patient.foreigner->ehic);
            }

            dentalCareService->SetAttribute("PICode", patient.id);
            
        }

        dentalCareService->SetAttribute("specificationType", list.nhifData.getSpecNumenclature(doctor.specialty));
        dentalCareService->SetAttribute("ambulatorySheetNo", 
            list.nrn.size() ? list.nrn :
            FreeFn::leadZeroes(list.number, 6)
        );
        dentalCareService->SetAttribute("HIRBNo", patient.HIRBNo); //throw if HIRBNo empty?
        dentalCareService->SetAttribute("unfavorableCondition", practice.isUnfavourable() && list.nhifData.isUnfavourable);
        dentalCareService->SetAttribute("substitute", 0);
        dentalCareService->SetAttribute("Sign", 1);

        //allergies
        TiXmlElement* allergies = new TiXmlElement("allergies");

        for (auto& a : patient.allergies)
        {
            TiXmlElement* allergy = new TiXmlElement("allergy");
            allergy->SetAttribute("allergyName", a.description);
            allergies->LinkEndChild(allergy);
        }
   
        dentalCareService->LinkEndChild(allergies);
    
        //past diseases
        TiXmlElement* pastDiseases = new TiXmlElement("pastDiseases");

        for (auto& p : patient.medStats.history)
        {
            TiXmlElement* pastDisease = new TiXmlElement("pastDisease");
            pastDisease->SetAttribute("name", p);
            pastDiseases->LinkEndChild(pastDisease);
        }

        dentalCareService->LinkEndChild(pastDiseases);

        //current diseases
        
        TiXmlElement* currentDiseases = new TiXmlElement("currentDiseases");

        for (auto& c : patient.medStats.condition)
        {
            TiXmlElement* currentDisease = new TiXmlElement("currentDisease");
            currentDisease->SetAttribute("name", c);
            currentDiseases->LinkEndChild(currentDisease);
        }

        dentalCareService->LinkEndChild(currentDiseases);
        
        //teeth

        TiXmlElement* teeth = new TiXmlElement("teeth");

        //parsing status

        for (auto& t : list.teeth)
        {
            auto status = t.getNhifStatus();
            if (status.empty()) continue;

            TiXmlElement* tooth = new TiXmlElement("tooth");
            tooth->SetAttribute("toothCode", ToothUtils::getNhifNumber(t.toothIndex()));

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

        //parsing supernumerals:

        for (auto& t : list.teeth)
        {
            if (!t[Dental::HasSupernumeral]) continue;

            auto status = t.getSupernumeral().getNhifStatus();
            if (status.empty()) continue;

            TiXmlElement* tooth = new TiXmlElement("tooth");
            tooth->SetAttribute("toothCode", ToothUtils::getNhifNumber(t.getSupernumeral().toothIndex()));

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
            service->SetAttribute("diagnosis", procedure.diagnosis.getFullDiagnosis());

            auto toothCode = procedure.tooth_idx.getNhifNumenclature();
            if (toothCode.empty()) toothCode = "99";

            service->SetAttribute("toothCode", toothCode);
            service->SetAttribute("activityCode", procedure.code.oldCode());

            services->LinkEndChild(service);

        }

        dentalCareService->LinkEndChild(services);
 
        auto medReferrals = new TiXmlElement("medicalReferrals");

        auto MDAReferrals = new TiXmlElement("MDAReferrals");

        auto prescSpecialists = new TiXmlElement("prescSpecialists");

        for (auto& r : list.referrals)
        {
            switch (r.type)
            {
                case(ReferralType::MDD4):
                {

                    if (r.nrn.empty()) continue;

                    auto mdaRef = new TiXmlElement("MDAReferral");
                        mdaRef->SetAttribute("pathwayNo", r.nrn);
                        mdaRef->SetAttribute("date", r.date.to8601());
                             auto codes = new TiXmlElement("codes");
                             auto code = new TiXmlElement("code");
                             code->SetAttribute("name", std::get<MDD4Data>(r.data).getCode());
                             codes->LinkEndChild(code);
                        mdaRef->LinkEndChild(codes);

                    MDAReferrals->LinkEndChild(mdaRef);

                    break;
                }
                case(ReferralType::Form3):
                {
                    if (r.nrn.empty()) continue;

                    auto medRef = new TiXmlElement("medicalReferral");
                     medRef->SetAttribute("pathwayNo", r.nrn);
                     medRef->SetAttribute("date", r.date.to8601());
                     medRef->SetAttribute("MRType", "1");
                     medRef->SetAttribute("specCode", R3Data::specialty);
                     medRef->SetAttribute("ICDCode", r.diagnosis.main.code());

                     medReferrals->LinkEndChild(medRef);

                     break;
                }
                case(ReferralType::Form3A):
                {
                    if (r.nrn.empty()) continue;

                    auto medRef = new TiXmlElement("medicalReferral");
                    medRef->SetAttribute("pathwayNo", r.nrn);
                    medRef->SetAttribute("date", r.date.to8601());
                    medRef->SetAttribute("MRType", "2");
                    medRef->SetAttribute("specCode", R3AData::nhifSpecialty);
                    medRef->SetAttribute("HSACode", R3AData::highlySpecializedActivity);
                    medRef->SetAttribute("ICDCode", r.diagnosis.main.code());

                    medReferrals->LinkEndChild(medRef);

                    break;
                }
                case(ReferralType::MH119):
                {
                    auto prescrSpec = new TiXmlElement("prescSpecialist");
                        
                        switch (std::get<MH119Data>(r.data).specCode)
                        {
                            case MH119Data::Pediatric:
                                prescrSpec->SetAttribute("SODPCode", 61); 
                                break;
                            case MH119Data::Surgery:
                                prescrSpec->SetAttribute("SODPCode", 62);
                                prescrSpec->SetAttribute("SODPCodeSecond", 68); 
                                break;
                        }

                        prescrSpec->SetAttribute("date", r.date.to8601());

                    prescSpecialists->LinkEndChild(prescrSpec);
                    break;
                }
            }


        }

        dentalCareService->LinkEndChild(medReferrals);

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

    addElementWithText(el_invoice, "fin_document_type_code", invoice.nhifDocumentTypeCode());
    addElementWithText(el_invoice, "fin_document_no", invoice.getInvoiceNumber());
    addElementWithText(el_invoice, "fin_document_month_no", FreeFn::leadZeroes(invoice.nhifData->fin_document_month_no, 10));
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
    addElementWithText(recipient, "recipient_bulstat", invoice.recipient.identifier);
    el_invoice->LinkEndChild(recipient);

    auto issuer = invoice.issuer();

    TiXmlElement* issuerXml = new TiXmlElement("Invoice_Issuer");

    addElementWithText(issuerXml, "issuer_type", std::to_string(issuer.type.index()));

    if (issuer.type.index() != 1)
    {
        auto& company = std::get<Company>(issuer.type);
        addElementWithText(issuerXml, "legal_form", company.legal_form);
        addElementWithText(issuerXml, "company_name", issuer.company_name);
    }
    else
    {
        auto& selfInsured = std::get<SelfInsured>(issuer.type);

        addElementWithText(issuerXml, "self_insured", "Y");
        addElementWithText(issuerXml, "self_insured_declaration", selfInsured.self_insured_declaration);

        addElementWithText(issuerXml, "company_name", issuer.company_name);
        TiXmlElement* person_info = new TiXmlElement("Person_Info");

        TiXmlElement* identifier = new TiXmlElement("Identifier");
        addElementWithText(identifier, "grao_no", selfInsured.person_info.identifier);
        person_info->LinkEndChild(identifier);

        addElementWithText(person_info, "first_name", selfInsured.person_info.first_name);
        addElementWithText(person_info, "second_name", selfInsured.person_info.second_name);
        addElementWithText(person_info, "last_name", selfInsured.person_info.last_name);

        issuerXml->LinkEndChild(person_info);

    }

    addElementWithText(issuerXml, "address_by_contract", issuer.address_by_contract);

    if (User::doctor().severalRHIF) {
        addElementWithText(issuerXml, "address_by_activity", issuer.address_by_activity);
    }

    auto vat = issuer.vat();

    addElementWithText(issuerXml, "registration_by_VAT", std::to_string(vat.has_value()));
    addElementWithText(issuerXml, "grounds_for_not_charging_VAT", issuer.grounds_for_not_charging_VAT);
    addElementWithText(issuerXml, "issuer_bulstat", issuer.bulstat);


    if (vat.has_value()) {
        addElementWithText(issuerXml, "issuer_bulstat_no_vat", vat.value());
    }

    if (User::hasNhifContract()) {
        auto& contract = *User::practice().nhif_contract;
        addElementWithText(issuerXml, "contract_no", contract.contract_no);
        addElementWithText(issuerXml, "contract_date", contract.date.to8601());
    }

    addElementWithText(issuerXml, "rhi_nhif_no", User::practice().rziCode);

    el_invoice->LinkEndChild(issuerXml);


    addElementWithText(el_invoice, "health_insurance_fund_type_code", invoice.nhifData->HIFTypeCode());
    addElementWithText(el_invoice, "activity_type_code", std::to_string(invoice.nhifData->activityTypeCode));
    addElementWithText(el_invoice, "date_from", invoice.nhifData->date_from.to8601());
    addElementWithText(el_invoice, "date_to", invoice.nhifData->date_to.to8601());

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
    addElementWithText(aggregatedAmounts, "total_amount", FreeFn::formatDouble(invoice.amount()));
    addElementWithText(aggregatedAmounts, "payment_amount", FreeFn::formatDouble(invoice.amount()));
    addElementWithText(aggregatedAmounts, "original", "Y");
    addElementWithText(aggregatedAmounts, "tax_event_date", invoice.taxEventDate.to8601());

    el_invoice->LinkEndChild(aggregatedAmounts);

    doc.LinkEndChild(el_invoice);

    TiXmlPrinter printer;
    printer.SetStreamPrinting();

    doc.Accept(&printer);

    return printer.Str();
}
