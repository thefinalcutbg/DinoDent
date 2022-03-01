#include "xml.h"
#include "Libraries/TinyXML/tinyxml.h"
#include "Model/User/UserManager.h"
#include "Database/DbXML.h"
#include "Model/XML/InvoiceXML.h"
#include <unordered_set>
#include <cmath>
#include "Model/Procedure/MasterNZOK.h"
#include "Model/FreeFunctions.h"
/*

#include <QProcess>
#include <QDir>

void showFileInFolder(const QString& path) {
#ifdef _WIN32    //Code for Windows
    QProcess::startDetached("explorer.exe", { "/select,", QDir::toNativeSeparators(path) });
#elif defined(__APPLE__)    //Code for Mac
    QProcess::execute("/usr/bin/osascript", { "-e", "tell application \"Finder\" to reveal POSIX file \"" + path + "\"" });
    QProcess::execute("/usr/bin/osascript", { "-e", "tell application \"Finder\" to activate" });
#endif
}

*/

std::string leadZeroes(int num, int totalLength)
{
    std::string formated;
    formated.reserve(totalLength);

    std::string number(std::to_string(num));

    for (int i = 0; i < totalLength - number.length(); i++)
        formated += "0";

    formated += number;

    return formated;
}

std::string getErrors(const std::vector<AmbListXML> report)
{
    std::string errors;

    std::unordered_set<int> uniqueSheetNumbers;

    for (int i = 0; i < report.size(); i++)
    {
        if (report[i].HIRBNo.empty())
        {
            errors +=
                u8"Амбулаторен лист "
                + std::to_string(report[i].ambulatorySheetNo) +
                u8" няма данни за номер на здравноосигурителна книжка" + "\n";
        }

        if (uniqueSheetNumbers.count(report[i].ambulatorySheetNo))
        {
            errors +=
                u8"Дублирана номерация на амбулаторен лист "
                + std::to_string(report[i].ambulatorySheetNo) + "\n";

            uniqueSheetNumbers.insert(report[i].ambulatorySheetNo);

            continue;
        }

        uniqueSheetNumbers.insert(report[i].ambulatorySheetNo);

        if (i && report[i].services[0].date < report[i - 1].services[0].date) {

            errors +=
                u8"Несъответствие на датата на първите манипулации и поредните номера между амбулаторни листи "
                + std::to_string(report[i - 1].ambulatorySheetNo) +
                u8" и " + std::to_string(report[i].ambulatorySheetNo) + "\n";
        }

        if (i && report[i].ambulatorySheetNo != report[i - 1].ambulatorySheetNo + 1) {

            errors += u8"Нарушена поредност на номерата. Липсваща номерация между амбулаторни листи "
                    + std::to_string(report[i - 1].ambulatorySheetNo) +
                    u8" и " + std::to_string(report[i].ambulatorySheetNo) + "\n";
        }

    }

    return errors;
}


ReportResult XML::saveXMLreport(int month, int year, const std::string& path)
{
    

    auto& doctor = UserManager::currentUser().doctor;
    auto& practice = UserManager::currentUser().practice;

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

    Date from{ 1, month, year };
    Date to = from.getMaxDateOfMonth();

    report->SetAttribute("startFrom", from.toXMLString());
    report->SetAttribute("endTo", to.toXMLString());
    report->SetAttribute("dentalServiceType", doctor.dentalServiceType());
    

    TiXmlElement* dentalCareServices = new TiXmlElement("dentalCareServices");
    

    //this is where we serialize the ambLists:

    DbXML db;
    auto ambSheets = db.getAmbListXML(month, year, practice.rziCode, doctor.LPK);

    auto errors = getErrors(ambSheets);

    if (!errors.empty())
    {
        ReportResult{false, errors};
    }

    for (auto& sheet : ambSheets)
    {
        TiXmlElement* dentalCareService = new TiXmlElement("dentalCareService");
        
            dentalCareService->SetAttribute("personType", sheet.personType);
            dentalCareService->SetAttribute("personIdentifier", sheet.personIdentifier);
            dentalCareService->SetAttribute("RHIFCode", sheet.RHIF);
            dentalCareService->SetAttribute("healthRegionCode", sheet.HealthRegion);

            if (sheet.personType != 1){
                dentalCareService->SetAttribute("birthDate", sheet.birthDate.toXMLString());
            }
            
            dentalCareService->SetAttribute("personFirstName", sheet.personFirstName);

            if(!sheet.personMiddleName.empty())
                 dentalCareService->SetAttribute("personMiddleName", sheet.personMiddleName);

            dentalCareService->SetAttribute("personLastName", sheet.personLastName);
            dentalCareService->SetAttribute("specificationType", sheet.specificationType);
            dentalCareService->SetAttribute("ambulatorySheetNo", leadZeroes(sheet.ambulatorySheetNo, 6));
            dentalCareService->SetAttribute("HIRBNo", sheet.HIRBNo); //throw if HIRBNo empty?
            dentalCareService->SetAttribute("unfavorableCondition", 0);
            dentalCareService->SetAttribute("substitute", sheet.substitute);
            dentalCareService->SetAttribute("Sign", sheet.sign);
           
            {
                TiXmlElement* allergies = new TiXmlElement("allergies");
                        TiXmlElement* allergy = new TiXmlElement("allergy");
                            allergy->SetAttribute("allergyName", sheet.allergies);
                         allergies->LinkEndChild(allergy);
          
                 dentalCareService->LinkEndChild(allergies);
            }

            {
                TiXmlElement* pastDiseases = new TiXmlElement("pastDiseases");
  
                        TiXmlElement* pastDisease = new TiXmlElement("pastDisease");
                            pastDisease->SetAttribute("name", sheet.pastDiseases);
                        pastDiseases->LinkEndChild(pastDisease);

                dentalCareService->LinkEndChild(pastDiseases);
            }

            {
                TiXmlElement* currentDiseases = new TiXmlElement("currentDiseases");
            
                        TiXmlElement* currentDisease = new TiXmlElement("currentDisease");
                             currentDisease->SetAttribute("name", sheet.currentDiseases);
                        currentDiseases->LinkEndChild(currentDisease);

                dentalCareService->LinkEndChild(currentDiseases);
            }

            TiXmlElement* teeth = new TiXmlElement("teeth");

                for (auto& toothSimple : sheet.teeth)
                {
                    TiXmlElement* tooth = new TiXmlElement("tooth");
                    tooth->SetAttribute("toothCode", toothSimple.toothCode);

                        TiXmlElement* toothStatuses = new TiXmlElement("toothStatuses");

                            for (auto& status : toothSimple.toothStatus)
                            {
                                TiXmlElement* toothStatus = new TiXmlElement("toothStatus");
                                    toothStatus->SetAttribute("toothStatusCode", status);
                                 toothStatuses->LinkEndChild(toothStatus);
                            }

                            tooth->LinkEndChild(toothStatuses);

                    teeth->LinkEndChild(tooth);
                }

            dentalCareService->LinkEndChild(teeth);

            TiXmlElement* services = new TiXmlElement("services");

                for(auto& procedure : sheet.services)
                {
                    TiXmlElement* service = new TiXmlElement("service");
                    
                        service->SetAttribute("date", procedure.date.toXMLString());
                        service->SetAttribute("diagnosis", procedure.diagnosis);
                        service->SetAttribute("toothCode", procedure.toothCode);
                        service->SetAttribute("activityCode", procedure.activityCode);

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



    doc.SaveFile(path + "/STOM_" 
                + practice.rziCode + "_"
                + doctor.LPK + "_"
                + std::to_string(doctor.specialty) + "_"
                + from.toXMLReportFileName()
                +"_01.xml");


    double expectedPrice{0}; //calculating the expected price

    int pregled {0};
    int golqm {0};
    int malak {0};

    for(auto& list : ambSheets){
        for (auto& service : list.services) {

            expectedPrice += MasterNZOK::instance().getNZOKPrice
            (
                service.activityCode,
                service.date,
                UserManager::currentUser().doctor.specialty,
                list.birthDate.getAge(service.date) > 17,
                false
            );


        }
    }

    return ReportResult{true,  
                        u8"Отчетът е генериран успешно! Очаквана сума: " + 
                        formatDouble(expectedPrice) +
                        u8" лв."};
}


void XML::saveXMLinvoice(const Invoice& invoice, const std::string& path)
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

    addElementWithText(el_invoice, "fin_document_type_code", invoice.fin_document_type_code);
    addElementWithText(el_invoice, "fin_document_no", invoice.fin_document_no);
    addElementWithText(el_invoice, "fin_document_month_no", invoice.fin_document_month_no);
    addElementWithText(el_invoice, "fin_document_date", invoice.fin_document_date.toXMLString());

    if (invoice.mainDocument.has_value())
    {
        TiXmlElement* mainDocument = new TiXmlElement("Main_Fin_Doc");
            addElementWithText(mainDocument, "document_no", invoice.mainDocument.value().number);
            addElementWithText(mainDocument, "document_date", invoice.mainDocument.value().date.toXMLString());
        el_invoice->LinkEndChild(mainDocument);
    }

    TiXmlElement* recipient = new TiXmlElement("Invoice_Recipient");
        addElementWithText(recipient, "recipient_code", invoice.recipient.code);
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
                addElementWithText(issuer, "address_by_activity", invoice.issuer.address_by_activity);
                addElementWithText(issuer, "registration_by_VAT", std::to_string(invoice.issuer.registration_by_VAT.has_value()));
                addElementWithText(issuer, "grounds_for_not_charging_VAT", invoice.issuer.grounds_for_not_charging_VAT);
                addElementWithText(issuer, "issuer_bulstat", invoice.issuer.bulstat);
                if (invoice.issuer.registration_by_VAT.has_value()){
                    addElementWithText(issuer, "issuer_bulstat_no_vat", invoice.issuer.registration_by_VAT.value());
                }
                addElementWithText(issuer, "contract_no", invoice.issuer.contract_no);
                addElementWithText(issuer, "contract_date", invoice.issuer.contract_date.toXMLString());
                addElementWithText(issuer, "rhi_nhif_no", invoice.issuer.rhi_nhif_no);

    el_invoice->LinkEndChild(issuer);

    
    addElementWithText(el_invoice, "health_insurance_fund_type_code", invoice.health_insurance_fund_type_code);
    addElementWithText(el_invoice, "activity_type_code", std::to_string(invoice.activityTypeCode));
    addElementWithText(el_invoice, "date_from", invoice.date_from.toXMLString());
    addElementWithText(el_invoice, "date_to", invoice.date_to.toXMLString());

    for (auto& operation : invoice.businessOperations)
    {
        TiXmlElement* businessOperation = new TiXmlElement ("Business_operation");
        
            addElementWithText(businessOperation, "activity_code",   operation.activity_code);
            addElementWithText(businessOperation, "activity_name",   operation.activity_name);
            addElementWithText(businessOperation, "measure_code",    operation.measure_code);
            addElementWithText(businessOperation, "quantity",        std::to_string(operation.quantity));
            addElementWithText(businessOperation, "unit_price",      formatDouble(operation.unit_price));
            addElementWithText(businessOperation, "value_price",     formatDouble(operation.value_price));

        el_invoice->LinkEndChild(businessOperation);
    }

    TiXmlElement* aggregatedAmounts = new TiXmlElement("Aggregated_amounts");

        addElementWithText(aggregatedAmounts, "payment_type", invoice.aggragated_amounts.payment_type);
        addElementWithText(aggregatedAmounts, "total_amount", formatDouble(invoice.aggragated_amounts.total_amount));
        addElementWithText(aggregatedAmounts, "payment_amount", formatDouble(invoice.aggragated_amounts.payment_amount));
        addElementWithText(aggregatedAmounts, "original", invoice.aggragated_amounts.original);
        addElementWithText(aggregatedAmounts, "tax_event_date", invoice.aggragated_amounts.taxEventDate.toXMLString());

    el_invoice->LinkEndChild(aggregatedAmounts);

    doc.LinkEndChild(el_invoice);

    doc.SaveFile(path);



}
