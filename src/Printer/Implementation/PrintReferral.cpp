#include "PrintPrv.h"


void Print::referral(const Referral& ref, const Patient& patient, const std::string& ambSheetNumber)
{

    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    auto refCommonFill = [&] {
        report.dataManager()->setReportVariable("ambNum", ambSheetNumber.c_str());
        report.dataManager()->setReportVariable("refNum", ref.getNumber().c_str());
        report.dataManager()->setReportVariable("diagnosis_main", ref.diagnosis.main.code().c_str());
        report.dataManager()->setReportVariable("diagnosis_additional", ref.diagnosis.additional.code().c_str());
        report.dataManager()->setReportVariable("reason", FreeFn::leadZeroes(ref.reason.getIndex() + 1, 2).c_str());
        report.dataManager()->setReportVariable("date", ref.date.toBgStandard().c_str());

        };

    auto mdd4fill = [&] {

        auto& doctor = User::doctor();
        auto& practice = User::practice();
        auto data = std::get<MDD4Data>(ref.data);

        report.loadFromFile(":/reports/report_mdd4.lrxml");

        fillCommonData(report, patient, doctor, practice);

        if (patient.type == Patient::LNCH) {
            report.dataManager()->setReportVariable("type", "X");
        }

        refCommonFill();
        /*
        report.dataManager()->setReportVariable("ambNum", QString::fromStdString(FreeFn::leadZeroes(ambSheetNumber, 12)));
        report.dataManager()->setReportVariable("refNum", QString::fromStdString(FreeFn::leadZeroes(ref.number, 12)));
        report.dataManager()->setReportVariable("diagnosis_main", ref.diagnosis.main.code().c_str());
        report.dataManager()->setReportVariable("diagnosis_additional", ref.diagnosis.additional.code().c_str());
        report.dataManager()->setReportVariable("reason", FreeFn::leadZeroes(ref.reason.getIndex() + 1, 2).c_str());
        */

        report.dataManager()->setReportVariable("nhifCode", data.getCode().c_str());
        report.dataManager()->setReportVariable("KSMP", data.getKSMP().c_str());

        if (!data.isOPG()) {
            report.dataManager()->setReportVariable("tooth", ToothUtils::getToothNumber(data.tooth_idx, false));
        }

        };

    auto mh119fill = [&] {


        report.loadFromFile(":/reports/report_mh119.lrxml");

        refCommonFill();

        fillCommonData(report, patient, User::doctor(), User::practice());

        auto data = std::get<MH119Data>(ref.data);

        report.dataManager()->setReportVariable("motives", data.description.c_str());
        report.dataManager()->setReportVariable("doctorPhone", User::doctor().phone.c_str());
        report.dataManager()->setReportVariable("diagnosis", ref.diagnosis.getText().c_str());
        report.dataManager()->setReportVariable("comorbidity", ref.comorbidity.getText().c_str());

        switch (data.specCode)
        {
        case MH119Data::Pediatric:
            report.dataManager()->setReportVariable("specCode", "61");
            break;
        case MH119Data::Surgery:
            report.dataManager()->setReportVariable("specCode", "62/68");
            break;
        }

        report.dataManager()->setReportVariable("practiceName", User::practice().name.c_str());

        int typeYpos[4] = { 440, 480, 525, 570 };

        report.dataManager()->setReportVariable("typeY", typeYpos[data.reason.getIndex()]);

        };

    switch (ref.type)
    {
    case ReferralType::MDD4:
        mdd4fill();
        break;
    case ReferralType::MH119:
        mh119fill();
        break;
    default:
        QApplication::restoreOverrideCursor();
        return;
    }

    report.setShowProgressDialog(true);
    QApplication::restoreOverrideCursor();
    report.printReport();


}