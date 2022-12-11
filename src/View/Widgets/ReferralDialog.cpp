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

	ui.refNumSpin->setTotalLength(6);

	for (auto num : ToothUtils::getToothNumbers()) {
		ui.toothCombo->addItem(QString::number(num));
	}

	for (auto& r : Reason::getNames())
	{
		ui.reasonCombo->addItem(r);
	}

	for (auto& r : MH119Reason::getNames())
	{
		ui.mh119combo->addItem(r);
	}

	ui.toothCombo->setDisabled(true);

	connect(ui.toothRadio, &QRadioButton::toggled,
		[&](bool checked) {
			ui.toothCombo->setDisabled(!checked);
		}
	);

	ui.opgRadio->setChecked(true);
	
	QPushButton* mkbButtons[4]{
		ui.mkbMainButton,
		ui.mkbAdditionalButton,
		ui.comorbidityMainButton,
		ui.comorbidityAdditionalButton
	};

	connect(ui.mkbMainButton, &QPushButton::clicked,
	[=] 
	{
		KSMPDialog d(MKB::getDentalMKBList(), ui.mkbMainButton->text().toStdString());
		d.exec();
		auto result = d.getResult();

		if (result.size()) {
			ui.mkbMainButton->setText(result.c_str());
			ui.errorLabel->setText("");
		}
	});

	connect(ui.mkbAdditionalButton, &QPushButton::clicked,
	[=] 
	{
		KSMPDialog d(MKB::getDentalMKBList(), ui.mkbAdditionalButton->text().toStdString());
		d.exec();
		auto result = d.getResult();

		if (result.size()) {
			ui.mkbAdditionalButton->setText(result.c_str());
		}
	});

	connect(ui.comorbidityMainButton, &QPushButton::clicked,
	[=] 
	{
		KSMPDialog d(MKB::getFullMKBList(), ui.comorbidityMainButton->text().toStdString());
		d.exec();
		auto result = d.getResult();

		if (result.size()) {
			ui.comorbidityMainButton->setText(result.c_str());
		}
	});

	connect(ui.comorbidityAdditionalButton, &QPushButton::clicked,
	[=] 
	{
		KSMPDialog d(MKB::getFullMKBList(), ui.comorbidityAdditionalButton->text().toStdString());
		d.exec();
		auto result = d.getResult();

		if (result.size()) {
			ui.comorbidityAdditionalButton->setText(result.c_str());
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

	auto mkbButtonSet = [](QPushButton* mkbButton, const MKB& mkb) {

		mkbButton->setText(
			mkb.isValid() ? mkb.code().c_str()
			: "Изберете"
		);

	};

	mkbButtonSet(ui.mkbMainButton, r.diagnosis.main);
	mkbButtonSet(ui.mkbAdditionalButton, r.diagnosis.additional);
	mkbButtonSet(ui.comorbidityMainButton, r.comorbidity.main);
	mkbButtonSet(ui.comorbidityAdditionalButton, r.comorbidity.additional);

	setRefTypeView(r.type);

	if (r.type == ReferralType::MDD4)
	{
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

	if (r.type == ReferralType::MH119)
	{
		auto& mh119 = std::get<MH119Data>(r.data);
		ui.mh119combo->setCurrentIndex(mh119.reason.getIndex());
		ui.mh119text->setPlainText(mh119.description.c_str());
	}

}

IReferralDialog::CommonData ReferralDialog::getCommon()
{
	auto d = ui.dateEdit->date();


	return CommonData{
		.date = Date(d.day(), d.month(), d.year()),
		.number = static_cast<int>(ui.refNumSpin->value()),
		.mkbMain = ui.mkbMainButton->text().toStdString(),
		.mkbAdditional = ui.mkbAdditionalButton->text().toStdString(),
		.mkbComorbMain = ui.comorbidityMainButton->text().toStdString(),
		.mkbComorbAdd = ui.comorbidityAdditionalButton->text().toStdString(),
		.reason_idx = ui.reasonCombo->currentIndex()
	};
}

MDD4Data ReferralDialog::MDD4data()
{
	return MDD4Data{
		ui.opgRadio->isChecked() ? -1 :
		ui.toothCombo->currentIndex()
	};
}

MH119Data ReferralDialog::MH119data()
{
	return MH119Data{
		.reason = ui.mh119combo->currentIndex(),
		.description = ui.mh119text->toPlainText().toStdString()
	};
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

void ReferralDialog::setRefTypeView(ReferralType t)
{
	ui.reasonCombo->hide();
	ui.label->hide();
	ui.refNumSpin->hide();
	ui.mh119combo->hide();
	ui.mdd4group->hide();
	ui.mh119Group->hide();
	ui.comorbidityGroup->hide();
	ui.reason119Label->hide();
	ui.reasonLabel->hide();

	switch (t)
	{
		case ReferralType::MDD4: 
		{
			ui.label->show();
			ui.refNumSpin->show();
			ui.reasonCombo->show();
			ui.mdd4group->show();
			ui.reasonLabel->show();
			break;
		}

		case ReferralType::MH119:
		{
			ui.label->show();
			ui.refNumSpin->show();
			ui.mh119combo->show();
			ui.mh119Group->show();
			ui.comorbidityGroup->show();
			ui.reason119Label->show();
			break;
		}

		default:
			ui.reasonLabel->show();
			ui.reasonCombo->show();
			ui.comorbidityGroup->show();

	}

	adjustSize();
}
