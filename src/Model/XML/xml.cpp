#include "xml.h"
#include "Libraries/TinyXML/tinyxml.h"
#include "Model/User/UserManager.h"
#include <string_view>
#include "Database/DbXML.h"
#include <QDebug>

void XML::saveXMLfile(const Date& from, const Date& to, std::string path)
{
    TiXmlDocument doc("StomReport");
    
    TiXmlDeclaration* decl = new TiXmlDeclaration{ "1.0", "UTF-8" ,"" };
    doc.LinkEndChild(decl);

    TiXmlElement* report = new TiXmlElement("report");
    report->SetAttribute("xmlns", "http://localhost");
    report->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    report->SetAttribute("xsi:schemaLocation", "http://localhost ImportStomatologyReport.xsd"); //fix this

    auto& user = UserManager::currentUser();

    report->SetAttribute("practiceName", user.practice_name);
    report->SetAttribute("bulstat", user.bulstat);
    report->SetAttribute("RCZCode", user.rziCode);
    report->SetAttribute("contractNo", user.contract);
    report->SetAttribute("dentistName", user.doctor_name);
    report->SetAttribute("dentistSpec", user.specialty);
    report->SetAttribute("dentistPersonalCode", user.LPK);

    //getting the first two characters of user.rziCode:
    report->SetAttribute("RHIF", user.rziCode.substr(0, 2));

    report->SetAttribute("startFrom", from.toStringXML());
    report->SetAttribute("endTo", to.toStringXML());
    report->SetAttribute("dentalServiceType", user.dentalServiceType());
    

    TiXmlElement* dentalCareServices = new TiXmlElement("dentalCareServices");
    

    //this is where we serialize the ambLists:

    DbXML db;
    auto ambSheets = db.getAmbListXML(from, to, UserManager::currentUser().rziCode, UserManager::currentUser().LPK);

    for (auto& sheet : ambSheets)
    {
        TiXmlElement* dentalCareService = new TiXmlElement("dentalCareService");
        
            dentalCareService->SetAttribute("personType", sheet.personType);
            dentalCareService->SetAttribute("personIdentifier", sheet.personIdentifier);
            dentalCareService->SetAttribute("RHIFCode", sheet.RHIF);
            dentalCareService->SetAttribute("healthRegionCode", sheet.HealthRegion);
            dentalCareService->SetAttribute("personFirstName", sheet.personFirstName);

            if(!sheet.personMiddleName.empty())
                 dentalCareService->SetAttribute("personMiddleName", sheet.personMiddleName);

            dentalCareService->SetAttribute("personLastName", sheet.personLastName);
            dentalCareService->SetAttribute("spcificationType", sheet.specificationType);
            dentalCareService->SetAttribute("ambulatorySheetNo", sheet.ambulatorySheetNo);
            dentalCareService->SetAttribute("HIRBNo", sheet.HIRBNo); //throw if HIRBNo empty?
            dentalCareService->SetAttribute("unfavorableCondition", sheet.unfavourable_condition);
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

        dentalCareServices->LinkEndChild(dentalCareService);
    }
    
    report->LinkEndChild(dentalCareServices);

    doc.LinkEndChild(report);


    doc.SaveFile(path + "/test.xml");

}