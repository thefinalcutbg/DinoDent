#include "xml.h"
#include "Libraries/TinyXML/tinyxml.h"
#include "Model/User/UserManager.h"
#include <string_view>

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
    report->SetAttribute("RHIF", std::string_view(user.rziCode.c_str(), 2).data());

    report->SetAttribute("startFrom", from.toStringXML());
    report->SetAttribute("endTo", to.toStringXML());
    report->SetAttribute("dentalServiceType", user.dentalServiceType());
    doc.LinkEndChild(report);

    TiXmlElement* dentalCareServices = new TiXmlElement("dentalCareServices");
    report->LinkEndChild(dentalCareServices);

    //this is where we serialize the ambLists:

    TiXmlElement* dentalCareService = new TiXmlElement("dentalCareService");
    dentalCareServices->LinkEndChild(dentalCareService);


    doc.SaveFile(path + "/test.xml");

}