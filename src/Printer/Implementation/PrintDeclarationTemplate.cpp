#include "PrintPrv.h"
#include "Model/DeclarationTemplate.h"
#include "View/Widgets/PrintPreviewDialog.h"

void generateReport(LimeReport::ReportEngine& report, const DeclarationTemplate& d, const Patient& p) {

	report.loadFromFile(":/reports/report_declTemplate.lrxml");	

	QString parts[] = { 
		d.header.c_str(), 
		d.subheader.c_str(), 
		d.body.c_str(), 
		d.footer.c_str() 
	};

	auto formatTag = [](const std::string& tag) -> QString {
		return "<b>" + QString::fromStdString(tag) + "</b>";
	};

	for(auto& part : parts)
	{
		part.replace(DeclarationTemplate::nametag, formatTag(p.fullName()));
		part.replace(DeclarationTemplate::idtag, formatTag(p.id));
		part.replace(DeclarationTemplate::addresstag, formatTag(p.getFullAddress()));
		part.replace(DeclarationTemplate::phonetag, formatTag(p.phone));
		part.replace(DeclarationTemplate::emailtag, formatTag(p.email));
		part.replace(DeclarationTemplate::birthtag, formatTag(p.birth.toBgStandard(true)));
		part.replace(DeclarationTemplate::currentdatetag, formatTag(Date::currentDate().toBgStandard(true)));
		part.replace(DeclarationTemplate::doctorNametag, formatTag(User::doctor().getFullName(true)));
		part.replace(DeclarationTemplate::dentistLPKtag, formatTag(User::doctor().LPK));
		part.replace(DeclarationTemplate::dentistPhonetag, formatTag(User::doctor().phone));
		part.replace(DeclarationTemplate::dentistSpecialtytag, formatTag(User::doctor().hisSpecialty.getName()));
	}

	//replacing the new lines in the body with <br>:

	parts[2].replace("\n", "<br>");

	report.dataManager()->setReportVariable("header", parts[0]);
	report.dataManager()->setReportVariable("subheader", parts[1]);
	report.dataManager()->setReportVariable("body", parts[2]);
	report.dataManager()->setReportVariable("footer", parts[3]);

}

bool Print::printDeclarationTemplate(const DeclarationTemplate& d, const Patient& p, const std::string& pdfFilename)
{
	QApplication::setOverrideCursor(Qt::BusyCursor);

	LimeReport::ReportEngine report;

	generateReport(report, d, p);

	QApplication::restoreOverrideCursor();

	return PrintPrv::printLogic(report, pdfFilename);
}

void Print::previewDeclarationTemplate(const DeclarationTemplate& decl) {

	LimeReport::ReportEngine report;

	auto patient = Patient{
		.id = "1234567890",
		.birth = Date(1, 1, 1990),
		.FirstName = "Иван",
		.MiddleName = "Иванов",
		.LastName = "Иванов",
		.city = Ekatte(68134),
		.address = "ул. Примерна 1",
		.phone = "0888123456",
		.email = "ivan.ivanov@example.com",
	};

	generateReport(report, decl, patient);


   PrintPreviewDialog d(report);

   if(d.exec() == QDialog::Accepted) report.printReport();

}