#include "xml.h"
#include "Libraries/TinyXML/tinyxml.h"
#include "Model/User/UserManager.h"
#include "Database/DbXML.h"


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

void XML::saveXMLfile(int month, int year, std::string path)
{
    TiXmlDocument doc("StomReport");
    
    TiXmlDeclaration* decl = new TiXmlDeclaration{ "1.0", "UTF-8" ,"" };
    doc.LinkEndChild(decl);

    TiXmlElement* report = new TiXmlElement("report");
    report->SetAttribute("xmlns", "http://localhost");
    report->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    report->SetAttribute("xsi:schemaLocation", "http://localhost ImportStomatologyReport.xsd");

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

    Date from{ 1, month, year };
    Date to = from.getMaxDateOfMonth();

    report->SetAttribute("startFrom", from.toStringXML());
    report->SetAttribute("endTo", to.toStringXML());
    report->SetAttribute("dentalServiceType", user.dentalServiceType());
    

    TiXmlElement* dentalCareServices = new TiXmlElement("dentalCareServices");
    

    //this is where we serialize the ambLists:

    DbXML db;
    auto ambSheets = db.getAmbListXML(month, year, UserManager::currentUser().rziCode, UserManager::currentUser().LPK);

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
            dentalCareService->SetAttribute("specificationType", sheet.specificationType);
            dentalCareService->SetAttribute("ambulatorySheetNo", sheet.ambulatorySheetNo);
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
                    
                        service->SetAttribute("date", procedure.date.toStringXML());
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
                + UserManager::currentUser().rziCode + "_"
                + UserManager::currentUser().LPK + "_"
                + std::to_string(UserManager::currentUser().specialty) + "_"
                + from.toStringXMLName()
                +"_01.xml");
}