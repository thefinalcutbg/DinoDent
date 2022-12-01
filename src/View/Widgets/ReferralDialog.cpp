#include "ReferralDialog.h"
#include "Presenter/ReferralPresenter.h"
#include "Model/Referrals/Referral.h"
#include "Model/Dental/ToothUtils.h"
#include "KSMPDialog.h"
#include <QPainter>


ReferralDialog::ReferralDialog(ReferralPresenter* p, QWidget *parent)
	: QDialog(parent), presenter(p)
{
	ui.setupUi(this);

	//initialize ui here

	for (auto num : ToothUtils::getToothNumbers()) {
		ui.toothCombo->addItem(QString::number(num));
	}

	for (auto& r : Reason::getNames())
	{
		ui.reasonCombo->addItem(r);
	}

	ui.toothCombo->setDisabled(true);

	connect(ui.toothRadio, &QRadioButton::toggled,
		[&](bool checked) {
			ui.toothCombo->setDisabled(!checked);
		}
	);

	ui.opgRadio->setChecked(true);
	
	connect(ui.mkbButton, &QPushButton::clicked, [=] {
			KSMPDialog d(MKB::getDentalMKBList(), ui.mkbButton->text().toStdString());
			d.exec();
			auto result = d.getResult();

			if (result.size()) {
				ui.mkbButton->setText(result.c_str());
				ui.diagLabel->setText(MKB::getNameFromMKBCode(result).c_str());
				ui.errorLabel->setText("");
			}
		});

	connect(ui.okButton, &QPushButton::clicked, [=] {
			presenter->okPressed();
		}
	);

	ui.errorLabel->setStyleSheet("color:red");

	p->setView(this);
}

ReferralDialog::~ReferralDialog()
{}

void ReferralDialog::setReferral(const Referral & r)
{
	ui.refNumSpin->setValue(r.number);

	ui.reasonCombo->setCurrentIndex(r.reason.getIndex());

	ui.dateEdit->setDate(QDate(r.date.year, r.date.month, r.date.day));


	ui.mkbButton->setText(

		r.diagnosis.main.isValid() ? 
			r.diagnosis.main.code().c_str() 
			: 
			"Изберете диагноза"
	);

	ui.diagLabel->setText(r.diagnosis.main.name().c_str());

	if (r.type != ReferralType::MDD4) {
		ui.mdd4group->setHidden(true);
		return;
	};

	auto& tooth = std::get<MDD4Data>(r.data).tooth;

	if (tooth == -1) {
		ui.opgRadio->setChecked(true);
	}
	else
	{
		ui.toothRadio->setChecked(true);
		ui.toothCombo->setCurrentIndex(tooth);
	}



}

IReferralDialog::CommonData ReferralDialog::getCommon()
{
	auto d = ui.dateEdit->date();

	auto mkbText = ui.mkbButton->text();

	return CommonData{
		.date = Date(d.day(), d.month(), d.year()),
		.number = static_cast<int>(ui.refNumSpin->value()),
		.mkb = ui.mkbButton->text().toStdString(),
		.reason_idx = ui.reasonCombo->currentIndex()
	};
}

MDD4Data ReferralDialog::MDD4data()
{
	if (ui.opgRadio->isChecked()) return MDD4Data(-1);

	return MDD4Data(ui.toothCombo->currentIndex());
}

void ReferralDialog::setErrorLabel(const std::string& str)
{
	ui.errorLabel->setText(str.c_str());
}

void ReferralDialog::setTitle(const std::string & str)
{
	setWindowTitle(str.c_str());
}


void ReferralDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}