#include "ReferralDialog.h"

#include <QPainter>

#include "Presenter/ReferralPresenter.h"
#include "Model/Referrals/Referral.h"
#include "Model/Dental/ToothUtils.h"
#include "View/Widgets/TableViewDialog.h"


ReferralDialog::ReferralDialog(ReferralPresenter* p, QWidget *parent)
	: QDialog(parent), presenter(p)
{
	ui.setupUi(this);

	//initialize ui here

	ui.refNumSpin->setTotalLength(6);

	for (auto num : ToothUtils::FDINumbers()) {
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


	ui.mh119specCombo->addItem("61 - Детски лекар по дентална медицина");
	ui.mh119specCombo->addItem("62 и 68 - Орална хирургия/ЛЧХ");


	ui.toothCombo->setDisabled(true);

    connect(ui.toothRadio, &QRadioButton::toggled, this,
		[&](bool checked) {
			ui.toothCombo->setDisabled(!checked);
		}
	);

	ui.opgRadio->setChecked(true);
	
	RightClickButton* mkbButtons[4]{
		ui.mkbMainButton,
		ui.mkbAdditionalButton,
		ui.comorbidityMainButton,
		ui.comorbidityAdditionalButton
	};

	for (auto& b : mkbButtons)
	{
        connect(b, &RightClickButton::rightClicked, this, [&]{ b->setText("Няма"); });
	}


    connect(ui.mkbMainButton, &QPushButton::clicked, this,
    [=, this]
	{
		TableViewDialog d(m_mkbDental,0, ui.mkbMainButton->text().toStdString());
		d.setWindowTitle("Международна класификация на болестите");	
		d.exec();

		auto result = d.getResult();

		if (result.size()) {
			ui.mkbMainButton->setText(result.c_str());
			ui.errorLabel->setText("");
		}
	});

    connect(ui.mkbAdditionalButton, &QPushButton::clicked, this,
    [=, this]
	{
		TableViewDialog d(m_mkbDental, 0, ui.mkbAdditionalButton->text().toStdString());
		d.setWindowTitle("Международна класификация на болестите");
		d.exec();
		auto result = d.getResult();

		if (result.size()) {
			ui.mkbAdditionalButton->setText(result.c_str());
		}
	});

    connect(ui.comorbidityMainButton, &QPushButton::clicked, this,
    [=, this]
	{
		TableViewDialog d(m_mkbFull,0, ui.comorbidityMainButton->text().toStdString());
		d.setWindowTitle("Международна класификация на болестите");
		d.exec();
		auto result = d.getResult();

		if (result.size()) {
			ui.comorbidityMainButton->setText(result.c_str());
		}
	});

    connect(ui.comorbidityAdditionalButton, &QPushButton::clicked, this,
    [=, this]
	{
		TableViewDialog d(m_mkbFull,0, ui.comorbidityAdditionalButton->text().toStdString());
		d.setWindowTitle("Международна класификация на болестите");
		d.exec();
		auto result = d.getResult();

		if (result.size()) {
			ui.comorbidityAdditionalButton->setText(result.c_str());
		}
	});

    connect(ui.okButton, &QPushButton::clicked, this, [=, this] {
			
			ui.dateEdit->validateInput();
			
			if (!ui.dateEdit->isValid()) return;

			presenter->okPressed();
		}
	);

	ui.dateEdit->setErrorLabel(ui.errorLabel);

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
			: "Няма"
		);

	};

	mkbButtonSet(ui.mkbMainButton, r.diagnosis.main);
	mkbButtonSet(ui.mkbAdditionalButton, r.diagnosis.additional);
	mkbButtonSet(ui.comorbidityMainButton, r.comorbidity.main);
	mkbButtonSet(ui.comorbidityAdditionalButton, r.comorbidity.additional);

	setRefTypeView(r.type);

	if (r.type == ReferralType::MDD4)
	{
		auto& tooth_idx = std::get<MDD4Data>(r.data).tooth_idx;

		if (tooth_idx == -1) {
			ui.opgRadio->setChecked(true);
		}
		else
		{
			ui.toothRadio->setChecked(true);
			ui.toothCombo->setCurrentIndex(tooth_idx);
		}
	}

	if (r.type == ReferralType::MH119)
	{
		auto& mh119 = std::get<MH119Data>(r.data);
		ui.mh119specCombo->setCurrentIndex(mh119.specCode);
		ui.mh119combo->setCurrentIndex(mh119.reason.getIndex());
		ui.mh119text->setPlainText(mh119.description.c_str());
	}

}

IReferralDialog::CommonData ReferralDialog::getCommon()
{
	return CommonData{
		.date = ui.dateEdit->getDate(),
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
		.specCode = static_cast<MH119Data::MH119Spec>(ui.mh119specCombo->currentIndex()),
		.reason = ui.mh119combo->currentIndex(),
		.description = ui.mh119text->toPlainText().toStdString()

	};
}

void ReferralDialog::setErrorLabel(const std::string& str)
{
	ui.errorLabel->setText(str.c_str());
}

void ReferralDialog::setDateValidator(DateValidator *d)
{
	ui.dateEdit->setInputValidator(d);
}

void ReferralDialog::setTitle(const std::string & str)
{
	setWindowTitle(str.c_str());
}


void ReferralDialog::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}

void ReferralDialog::setRefTypeView(ReferralType t)
{

	ui.mdd4group->hide();
	ui.group119->hide();
	ui.comorbidityGroup->hide();
	ui.reasonGroup->hide();
	ui.motives119->hide();

	switch (t)
	{
		case ReferralType::MDD4: 
		{
			ui.reasonGroup->show();
			ui.mdd4group->show();
			
			break;
		}

		case ReferralType::MH119:
		{
			ui.group119->show();
			ui.motives119->show();
			ui.comorbidityGroup->show();
			break;
		}

		default:
			ui.reasonGroup->show();
			ui.comorbidityGroup->show();

	}

	adjustSize();
}
