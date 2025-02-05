#include "PrintPrv.h"


bool Print::consent(const Patient& patient, const std::string& pdfFilename)
{
    std::string dialogTitle = "Информирано съгласие";

    //asking for declarator
    int declaratorType = ModalDialogBuilder::openButtonDialog(
        {
        "Пациент",
        "Родител/Настойник/Попечител",
        "Лица по чл.87(5),(6),чл.162(3) от ЗЗ"
        },
        dialogTitle,
        "Изберете декларатор:"
    );

    //different declarator
    if (declaratorType == -1) return false;

    Patient declarator = patient;

    if (declaratorType > 0) {

        PatientDialogPresenter p("Въведете декларатор");
        auto declaratorResult = p.open();

        if (!declaratorResult) return false;

        declarator = *declaratorResult;
    }


    int choice = ModalDialogBuilder::openButtonDialog(
        {
        "Дава информирано съгласие",
        "Отказва да бъде информиран",
        "Отказва лечение"
        },
        dialogTitle,
        "Избор на декларатора:"
    );

    if (choice == -1) return false;

    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    auto& doctor = User::doctor();
    auto& practice = User::practice();

    report.loadFromFile(":/reports/report_consent.lrxml");


    report.dataManager()->setReportVariable("practice_name", practice.name.c_str());
    report.dataManager()->setReportVariable("practice_address", practice.practice_address.getString().c_str());
    report.dataManager()->setReportVariable("doctor", QString::fromStdString(doctor.getFullName()));

    report.dataManager()->setReportVariable("gender", declarator.sex == Patient::Female ? "Долуподписаната" : "Долуподписаният");


    const char* idType[5] = { "","ЕГН","ЛНЧ","ССН","ID" };
    report.dataManager()->setReportVariable("type", idType[declarator.type]);
    report.dataManager()->setReportVariable("name", QString::fromStdString(declarator.fullName()));
    report.dataManager()->setReportVariable("id", QString::fromStdString(declarator.id));
    report.dataManager()->setReportVariable("phone", QString::fromStdString(declarator.phone));

    report.dataManager()->setReportVariable("circle" + QString::number(declaratorType), QString{ QChar{ 0x25EF } });

    if (declaratorType != 0) //patient case 
    {
        report.dataManager()->setReportVariable("patientName" + QString::number(declaratorType), QString::fromStdString(patient.fullName()));
    }

    const char* declaratorTypeStr[3] = {
        "<b>пациент</b>",
        "<b>родител/настойник/попечител</b>",
        "<b>лице по чл. 87, ал. 5, дирекцията по чл. 87, ал. 6, както и лицата по чл. 162, ал. 3 от Закон за здравето</b>"
    };

    report.dataManager()->setReportVariable("declaratorType", declaratorTypeStr[declaratorType]);

    Date date = Date::currentDate();

    report.dataManager()->setReportVariable("name" + QString::number(choice), QString::fromStdString(declarator.fullName()));
    report.dataManager()->setReportVariable("date" + QString::number(choice), QString::fromStdString(date.toBgStandard()));

    QApplication::restoreOverrideCursor();

    return PrintPrv::printLogic(report, pdfFilename);
}

void Print::consent()
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    auto& doctor = User::doctor();
    auto& practice = User::practice();

    report.loadFromFile(":/reports/report_consent.lrxml");

    report.dataManager()->setReportVariable("placeholder", "........................................");
    report.dataManager()->setReportVariable("practice_name", QString::fromStdString(practice.name));
    report.dataManager()->setReportVariable("practice_address", QString::fromStdString(practice.practice_address.getString()));
    report.dataManager()->setReportVariable("doctor", QString::fromStdString(doctor.getFullName()));
    report.dataManager()->setReportVariable("declarator", "Долуподписаният/та:");
    report.dataManager()->setReportVariable("type", "ЕГН/ID");
    report.dataManager()->setReportVariable("declaratorType", "пациент / родител, настойник, попечител / лице по чл. 87, ал. 5, дирекцията по чл. 87, ал. 6, както и лицата по чл. 162, ал. 3 от Закон за здравето");

    QApplication::restoreOverrideCursor();

    report.printReport();
}